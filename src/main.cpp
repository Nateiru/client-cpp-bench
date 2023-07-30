#include <cassert>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <thread>

#include <google/protobuf/extension_set.h>
#include <google/protobuf/stubs/common.h>
#include <greptime/v1/common.pb.h>
#include <greptime/v1/database.grpc.pb.h>
#include <greptime/v1/database.pb.h>
#include <greptime/v1/column.pb.h>
#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>

#include <grpcpp/client_context.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/support/async_stream.h>

#include <grpcpp/channel.h>
#include <grpcpp/support/status.h>
#include <src/database.h>

#include <src/config.hpp>

using json = nlohmann::json;

// ==================== 读取表结构，http构建表 ==================== //
using CanId = std::string;
using SignalName = std::string;
using SignalNames = std::vector<SignalName>;
using SignalToCanId = std::unordered_map<SignalName, CanId>;
using Sql = std::string;


struct CanIdToSignalMap {
    std::unordered_map<CanId, SignalNames> inner;

    // void debug() {
    //     for (const auto &[can_id, signal_names] : inner) {
    //         std::cout << "CAN ID: " << can_id << ":\n";
    //         std::cout << "Sigmals:\n";
    //         for (const auto &signal_name:signal_names) 
    //             std::cout << " - " << signal_name << '\n';
    //         std::cout << std::endl;
    //     }
    // }
    SignalToCanId build_index() {
        SignalToCanId signal_to_can_id_map;
        for (const auto &[can_id, signal_names] : inner)
            for (const auto &signal_name : signal_names) 
                signal_to_can_id_map.emplace(signal_name, can_id);
        return signal_to_can_id_map;
    }

    std::vector<Sql> createTableSqls(uint32_t buffer_size) {
        const auto &can_id_map = inner;

        std::vector<Sql> statements;
        statements.reserve(can_id_map.size());

        for (const auto& [can_id, signal_names] : can_id_map) {
            std::string statement;

            statement += "create table if not exists table_";
            statement += can_id;
            statement += "(ts TIMESTAMP time index, ";

            for (size_t idx = 0; idx < signal_names.size(); ++idx) {
                auto& name = signal_names[idx];
                if (idx != signal_names.size() - 1) {
                    statement += name;
                    statement += " float, ";
                } else {
                    statement += name;
                    statement += " float)";
                }
            }

            statement += "WITH(write_buffer_size='";
            statement += std::to_string(buffer_size);
            statement += "KB')";

            statements.emplace_back(std::move(statement));
        }

        return statements;
    }

    std::vector<Sql> clearTableSqls(uint32_t buffer_size) {
        const auto &can_id_map = inner;

        std::vector<Sql> statements;
        statements.reserve(can_id_map.size());

        for (const auto& [can_id, signal_names] : can_id_map) {
            std::string statement;

            statement += "DELETE FROM table_";
            statement += can_id;
            statement += ";";
            statements.emplace_back(std::move(statement));
        }

        return statements;
    }
};

CanIdToSignalMap parse_can_id_signal_file(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }
    // 从文件流解析json
    json content;
    file >> content;

    // 将json对象转换为CanIdToSignalMap
    auto map = content.get<std::unordered_map<CanId, SignalNames>>();
    return {map};
}

// ==================== 读取CSV文件，构造InsertRequest ==================== //

using TableName = std::string;
using Timestamp = int64_t;
using FieldName = std::string;
using FieldVal = float_t;

/// 每个时间戳是一组 insert sql 可能插入不同的表，表的某些列可能是 null
struct InsertEntry {
    Timestamp ts;
    std::unordered_map<TableName, std::unordered_map<FieldName, FieldVal>> tables;

    InsertEntry():ts(-1) {
        tables.clear();
    }

    InsertEntry(Timestamp ts_):ts(ts_) {
        tables.clear();
    }

    void set_ts(Timestamp ts_) {
        ts = ts_;
    }

    void add_point(TableName table_name, FieldName field_name, FieldVal field_val) {
        if (tables.find(table_name) == tables.end()) {
            tables.emplace(table_name, std::unordered_map<FieldName, FieldVal>());
        }
        auto &fields = tables[table_name];
        fields.emplace(field_name, field_val);
    }

    uint32_t get_point_num() {
        uint32_t ret = 0;
        for (const auto & table : tables) 
            ret += table.second.size();
        return ret;
    }
};

/// 攒批
using InsertBatch = std::vector<InsertEntry>;

using greptime::v1::Column;
using greptime::v1::Column_SemanticType;
using greptime::v1::ColumnDataType;
using greptime::v1::InsertRequest;
using greptime::v1::InsertRequests;

struct LineWriter {
    // 每个 Table 是一个 InsertRequest
    std::unordered_map<TableName, 
                       std::tuple<std::unordered_map<FieldName, std::vector<FieldVal>>,
                                  std::vector<Timestamp>, 
                                  uint32_t>
                                  > mp;

    void add_row(TableName table_name, Timestamp ts, std::unordered_map<FieldName, FieldVal> fields) {
        
        auto &[field_map, ts_vec, cur_row] = mp[table_name];
        // 如果前面 insert 的没有对应对 field
        // 那么说明该 field 前面都是 null
        for(auto [name, val] : fields) {
            if (field_map.find(name) == field_map.end()) {
                field_map.emplace(name, std::vector<FieldVal>(0.0, cur_row));
            }            
            field_map[name].emplace_back(val);
        }
        cur_row += 1;
        ts_vec.emplace_back(ts);
    } 

    void commit() {
        // 可能新增一个 field
        // 其他每行的也需要增加 field 值是null
        for (auto &[table_name, table_map] : mp) {
            auto &field_map = std::get<0>(table_map);
            auto &cur_row = std::get<2>(table_map);
            
            std::for_each(field_map.begin(), field_map.end(), [cur_row](auto& fields) {
                auto& val_vec = fields.second;
                if (val_vec.size() < cur_row) {
                    val_vec.emplace_back(0.0);
                }
            });
        }
    }

    auto build() -> std::vector<InsertRequest> {
        std::vector<InsertRequest> insert_vec;
        insert_vec.reserve(mp.size());

        for (auto &[table_name, t3] : mp) {
            InsertRequest insert_request;
            auto &[field_map, ts_vec, row_count] = t3;
            insert_request.set_table_name(table_name);
            insert_request.set_row_count(ts_vec.size());
            // timestamp
            {
                Column column;
                column.set_column_name("ts");
                column.set_semantic_type(Column_SemanticType::Column_SemanticType_TIMESTAMP);
                column.set_datatype(ColumnDataType::TIMESTAMP_MILLISECOND);
                auto values = column.mutable_values();
                for (const auto& ts : ts_vec) {
                    values->add_ts_millisecond_values(ts);
                }
                insert_request.add_columns()->CopyFrom(std::move(column));
            }
            // field
            for (const auto&[field_name, field_vals] : field_map) {
                Column column;
                column.set_column_name(field_name);
                column.set_semantic_type(Column_SemanticType::Column_SemanticType_FIELD);
                column.set_datatype(ColumnDataType::FLOAT32);
                auto values = column.mutable_values();
                for (const auto& field_val : field_vals) {
                    values->add_f32_values(field_val);
                }
                insert_request.add_columns()->CopyFrom(std::move(column));
            }
            insert_vec.emplace_back(std::move(insert_request));
        }
        return insert_vec;
    }

};

auto to_insert_requests(const std::vector<InsertRequest>& vec_insert_requests) -> InsertRequests {
    InsertRequests insert_requests;
    for (auto insert_request : vec_insert_requests) {
        insert_requests.add_inserts()->CopyFrom(insert_request);
    }
    return insert_requests;
}

struct Core {
    std::shared_ptr<BenchConfig> config;
    std::shared_ptr<CanIdToSignalMap> can_id_signals_map;
    std::shared_ptr<SignalToCanId> index;
    Core(BenchConfig &config_, CanIdToSignalMap &can_id_signals_map_, SignalToCanId &index_) :
        config(std::make_shared<BenchConfig>(config_)), 
        can_id_signals_map(std::make_shared<CanIdToSignalMap>(can_id_signals_map_)),
        index(std::make_shared<SignalToCanId>(index_)){};
    
};

// 单生产者-多消费者
std::queue<InsertEntry> buffer;
std::mutex mtx;
std::condition_variable cv;
const int BUFFER_SIZE = 100000;
bool is_producer_done = false;


void producer(const Core& core) {
    std::ifstream file(core.config->csv_data_path());
    std::string line;
    Timestamp time_flag = -1;
    // 第一行忽略
    std::getline(file, line);
    int cnt = 0;
    InsertEntry insert_entry;
    // uint32_t curr_points = 0;
    while(std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;

        std::getline(ss, item, ',');
        Timestamp ts = std::atol(item.c_str());
        std::getline(ss, item, ',');
        FieldName signal = item;
        std::getline(ss, item, ',');
        FieldVal val = atof(item.c_str());

        if (time_flag == -1) {
            time_flag = ts;
            insert_entry.set_ts(ts);
        }
        else if (time_flag != ts) {
            // auto points = insert_entry.get_point_num();

            // if (points == 0) {
            //     points = 1;
            // }

            // curr_points += points;
            // if (curr_points > core.config->insert_point_number()) {
            //     break;
            // }
            // TODO: processor
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, []{
                    return buffer.size() < BUFFER_SIZE;
                });
                buffer.push(insert_entry);
                cv.notify_all();
            }
            insert_entry = InsertEntry(ts);
            time_flag = ts;
        }
        if (core.index->find(signal) == core.index->end()) {
            continue;
        }
        auto can_id = core.index->at(signal);
        auto table_name = "table_" + can_id;
        insert_entry.add_point(table_name, signal, val);
    }
    if (insert_entry.ts != -1) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{
            return buffer.size() < BUFFER_SIZE;
        });
        buffer.push(insert_entry);
        cv.notify_all();
    }
    is_producer_done = true;
}

void consumer(const Core& core) {

    // client rpc
    auto channel = grpc::CreateChannel("localhost:4001", grpc::InsecureChannelCredentials());
    greptime::Database database("public", channel);

    InsertBatch cache;
    while (true) {

        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{
            return !buffer.empty() || is_producer_done;
        });

        if (!buffer.empty()) {
            auto insert_entry = buffer.front();
            buffer.pop(); 
            cache.emplace_back(insert_entry);
            cv.notify_all();
            lock.unlock();
        }
        else {
            assert(is_producer_done);
            lock.unlock();
            if (cache.size() == 0) {
                break;
            }
        }

        if (cache.size() < core.config->batch_number() && !is_producer_done) {
            continue;
        }

        LineWriter line_writer;

        for (auto &insert_entry : cache) {
            auto ts = insert_entry.ts;
            for (auto &[table_name, fields] : insert_entry.tables) {
                line_writer.add_row(table_name, ts, fields);
                line_writer.commit();
            }
        }
        cache.clear();
        auto insert_vec = line_writer.build();
        auto insert_requests = to_insert_requests(insert_vec);
        database.Insert(insert_requests);
    }

    database.InsertsDone();

    grpc::Status status = database.Finish();

    /** =========================== 4.handle return response =========================== **/
    if (status.ok()) {
        std::cout << "success!" << std::endl;
        auto response = database.GetResponse();

        std::cout << "notice: [";
        std::cout << response.affected_rows().value() << "] ";
        std::cout << "rows of data are successfully inserted into the public database"<< std::endl;
    } else {
        std::cout << "fail!" << std::endl;
        std::cout << status.error_message() << std::endl;
        std::cout << status.error_details() << std::endl;
    }
}

#include <httplib.h>

void create_table(const Core &core, const Sql &sql) {
    auto http_endpoint = core.config->write_http_endpoint();
    httplib::Client client(http_endpoint.c_str());
    httplib::Params params;
    params.emplace("sql", sql);

    auto res = client.Post("/v1/sql?db=public", params);

    // if (res && res->status == 200) {
    //     std::cout << "Create table success! SQL: " << sql << std::endl;
    // } else {
    //     if (res) {
    //         std::cerr << "Failed to create table, status code: " << res->status << std::endl;
    //     } else {
    //         std::cerr << "Failed to create table, error: " << res.error() << std::endl;
    //     }
    // }
}

void create_tables(const Core &core) {
    auto can_id_map = core.can_id_signals_map;
    auto buffer_size = core.config->write_buffer_size();
    auto sql_vec = can_id_map->createTableSqls(buffer_size);

    for (auto &sql : sql_vec) {
        create_table(core, sql);
    }
}

void prepare(const Core &core) {
    create_tables(core);
}

#include <chrono>

void truncate(const Core &core) {
    auto start_time = std::chrono::high_resolution_clock::now();

    auto can_id_map = core.can_id_signals_map;
    auto buffer_size = core.config->write_buffer_size();
    auto sql_vec = can_id_map->clearTableSqls(buffer_size);

    for (auto &sql : sql_vec) {
        create_table(core, sql);
    }
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    std::cout << "清空表时间：" << duration_us.count() << " 微秒" << std::endl;
    std::cout << "清空表时间：" << duration_ms.count() << " 毫秒" << std::endl;
    std::cout << "清空表时间：" << duration_s.count() << " 秒" << std::endl;

}

void bench(const Core &core) {
    
    auto start_time = std::chrono::high_resolution_clock::now();

    std::thread procucer_thread(producer, core);
    std::vector<std::thread> consumer_threads;
    #define CONSUMER_NUM 16

    for (int i = 0; i < CONSUMER_NUM; i++) {
        consumer_threads.emplace_back(std::thread(consumer, core));
    }

    procucer_thread.join();

    for (int i = 0; i < CONSUMER_NUM; i++) {
        consumer_threads[i].join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    std::cout << "程序运行时间：" << duration_us.count() << " 微秒" << std::endl;
    std::cout << "程序运行时间：" << duration_ms.count() << " 毫秒" << std::endl;
    std::cout << "程序运行时间：" << duration_s.count() << " 秒" << std::endl;


}
int main(int argc, const char *argv[]) {
    /// =========== 1.构造配置变量 =========== 
    auto config = BenchConfig();
    auto can_id_map = parse_can_id_signal_file(config.can_id_map_path());
    auto index = can_id_map.build_index();
    // auto sqls = can_id_map.createTableSqls(512);

    /// =========== 2.http创建表 ===========
    // prepare(core);

    /// =========== 3.bench测试 ==========
    if (argc == 2) {
        std::string csv_data_path = std::string(argv[1]);
        auto config = BenchConfig(csv_data_path); 
        Core core{config, can_id_map, index};
        bench(core); 
    }
    else {
        Core core{config, can_id_map, index};
        truncate(core);
    }
    return 0;
}
