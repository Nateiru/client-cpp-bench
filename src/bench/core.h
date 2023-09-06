#pragma once
#include <greptime/v1/common.pb.h>
#include <greptime/v1/database.grpc.pb.h>
#include <greptime/v1/database.pb.h>
#include <greptime/v1/column.pb.h>
#include <src/bench/config.hpp>
#include <string>

using CanId = std::string;
using SignalName = std::string;
using SignalNames = std::vector<SignalName>;
using SignalToCanId = std::unordered_map<SignalName, CanId>;
using Sql = std::string;


struct CanIdToSignalMap {
    std::unordered_map<CanId, SignalNames> inner;

    CanIdToSignalMap() = default;

    SignalToCanId build_index();

    std::vector<Sql> createTableSqls(uint32_t buffer_size);

    std::vector<Sql> clearTableSqls(uint32_t buffer_size);
};

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

    void add_point(TableName table_name, FieldName field_name, FieldVal field_val);

    uint32_t get_point_num();
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
    LineWriter() = default;

    void add_row(TableName table_name, Timestamp ts, const std::unordered_map<FieldName, FieldVal> &fields);

    void commit();

    auto build() -> std::vector<InsertRequest>;
    
};

struct Core {
    std::shared_ptr<BenchConfig> config;
    std::shared_ptr<CanIdToSignalMap> can_id_signals_map;
    std::shared_ptr<SignalToCanId> index;
    Core(BenchConfig &config_, CanIdToSignalMap &can_id_signals_map_, SignalToCanId &index_) :
        config(std::make_shared<BenchConfig>(config_)), 
        can_id_signals_map(std::make_shared<CanIdToSignalMap>(can_id_signals_map_)),
        index(std::make_shared<SignalToCanId>(index_)){};
    
};
