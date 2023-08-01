#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <thread>

#include <src/database.h>
#include <nlohmann/json.hpp>
#include "core.h"
#include "httpsql.h"

#include "httplib.h"

using json = nlohmann::json;

auto to_insert_requests(std::vector<InsertRequest>& vec_insert_requests) -> InsertRequests {
    InsertRequests insert_requests;
    for (auto &insert_request : vec_insert_requests) {
        insert_requests.add_inserts()->Swap(&insert_request);
    }
    return std::move(insert_requests);
}

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

// 生产者-消费者
std::queue<InsertEntry> buffer;
std::mutex mtx;
std::condition_variable cv;
const int BUFFER_SIZE = 100000;
std::atomic<bool> is_producer_done(false);

void producer(const Core& core) {
    std::ifstream file(core.config->csv_data_path());
    std::string line;
    Timestamp time_flag = -1;
    // 第一行忽略
    std::getline(file, line);
    uint64_t cnt = 0;
    uint64_t insert_point_number = core.config->insert_point_number();
    InsertEntry insert_entry;
    while(std::getline(file, line)) {

        auto pos = line.find(',');
        Timestamp ts = std::atol(line.substr(0, pos).c_str());
        line.erase(0, pos + 1);

        pos = line.find(',');
        FieldName signal = line.substr(0, pos);
        line.erase(0, pos + 1);
    
        pos = line.find(',');
        FieldVal val = atof(line.substr(0, pos).c_str());
        
        if (core.index->find(signal) == core.index->end()) {
            continue;
        }

        if (time_flag == -1) {
            assert(ts >= 0);
            time_flag = ts;
            insert_entry.set_ts(ts);
        }
        else if (time_flag != ts) {
            // TODO: processor
            uint64_t points = insert_entry.get_point_num();
            if (points == 0) points++;
            cnt += points;
            if (cnt > insert_point_number) {
                break;
            }

            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, []{
                    return buffer.size() < BUFFER_SIZE;
                });
                buffer.push(insert_entry);
                cv.notify_one();
            }
            
            insert_entry = InsertEntry(ts);
            time_flag = ts;
        }
        
        auto can_id = core.index->at(signal);
        auto table_name = "table_" + can_id;
        insert_entry.add_point(table_name, signal, val);
    }
    // 最后一组 timestamp
    if (insert_entry.ts != -1 && insert_entry.get_point_num() > 0) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{
            return buffer.size() < BUFFER_SIZE;
        });
        buffer.push(insert_entry);
        cv.notify_one();
    }
    is_producer_done = true;
    cv.notify_one();
}

void consumer(const Core& core) {

    // client rpc
    auto channel = grpc::CreateChannel("localhost:4001", grpc::InsecureChannelCredentials());
    greptime::Database database("public", channel);

    InsertBatch cache;
    cache.reserve(core.config->batch_number());
    while (true) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{
                return !buffer.empty() || is_producer_done;
            });

            if (!buffer.empty()) {
                auto insert_entry = buffer.front();
                buffer.pop(); 
                cache.emplace_back(insert_entry);
                cv.notify_one();
            }
            else {
                assert(is_producer_done);
                if (cache.size() == 0) {
                    break;
                }
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
            }
        }
        cache.clear();
        auto insert_vec = line_writer.build();
        auto insert_requests = to_insert_requests(insert_vec);
        database.Insert(insert_requests);
    }

    database.InsertsDone();

    grpc::Status status = database.Finish();

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

void bench(const Core &core) {
    
    std::cout << "bench start" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    std::thread procucer_thread(producer, core);
    std::vector<std::thread> consumer_threads;

    #define CONSUMER_NUM 1
    consumer_threads.reserve(CONSUMER_NUM);

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


    if (argc == 2) {
        std::string csv_data_path = std::string(argv[1]);
        auto config = BenchConfig(csv_data_path); 
        Core core{config, can_id_map, index};

    /// =========== 2.http创建表 ===========
        prepare(core);

    /// =========== 3.bench测试 ==========
        bench(core); 
    }
    else {
        Core core{config, can_id_map, index};
        truncate(core);
    }
    return 0;
}
