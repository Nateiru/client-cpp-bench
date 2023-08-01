#include "core.h"
#include <cassert>
#include <src/config.hpp>
#include <tuple>
#include <vector>

SignalToCanId CanIdToSignalMap::build_index() {
    SignalToCanId signal_to_can_id_map;
    for (const auto &[can_id, signal_names] : inner)
        for (const auto &signal_name : signal_names) 
            signal_to_can_id_map.emplace(signal_name, can_id);
    return signal_to_can_id_map;
}

std::vector<Sql> CanIdToSignalMap::createTableSqls(uint32_t buffer_size) {
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

std::vector<Sql> CanIdToSignalMap::clearTableSqls(uint32_t buffer_size) {
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

void InsertEntry::add_point(TableName table_name, FieldName field_name, FieldVal field_val) {
    if (tables.find(table_name) == tables.end()) {
        tables.emplace(table_name, std::unordered_map<FieldName, FieldVal>());
    }
    auto &fields = tables[table_name];
    fields[field_name] = field_val;
    // fields.emplace(field_name, field_val);
}

uint32_t InsertEntry::get_point_num() {
    uint32_t ret = 0;
    for (const auto & table : tables) 
        ret += table.second.size();
    return ret;
}


void LineWriter::add_row(TableName table_name, Timestamp ts, const std::unordered_map<FieldName, FieldVal> &fields) {
    if (mp.find(table_name) == mp.end()) {
        mp.emplace(table_name, std::make_tuple(std::unordered_map<FieldName, std::vector<FieldVal>>(),
                                               std::vector<Timestamp>(),
                                               0));
    }
    auto &[field_map, ts_vec, cur_row] = mp[table_name];
    // 如果前面 insert 的没有对应对 field
    // 那么说明该 field 前面都是 null
    for(const auto &[name, val] : fields) {
        auto it = field_map.find(name);
        if (it == field_map.end()) {
            std::vector<FieldVal> field_val(cur_row, 0.0);
            field_val.emplace_back(val);
            field_map.emplace(name, field_val);
        } 
        else {
            it->second.emplace_back(val);
        }
    }
    cur_row += 1;
    ts_vec.emplace_back(ts);

    size_t row_count = cur_row;
    std::for_each(field_map.begin(), field_map.end(), [&, cur_row = row_count](auto& field) {
        auto& val_vec = field.second;
        if (val_vec.size() < cur_row) {
            assert((val_vec.size() + 1 == cur_row));
            val_vec.emplace_back(0.0);
        }
    });
} 

auto LineWriter::build() -> std::vector<InsertRequest> {
    std::vector<InsertRequest> insert_vec;
    insert_vec.reserve(mp.size());
    Column column;
    for (auto &[table_name, t3] : mp) {
        InsertRequest insert_request;
        auto &[field_map, ts_vec, row_count] = t3;
        insert_request.set_table_name(table_name);
        insert_request.set_row_count(row_count);
        assert(row_count == ts_vec.size());
        // timestamp
        {
            column.Clear();
            column.set_column_name("ts");
            column.set_semantic_type(Column_SemanticType::Column_SemanticType_TIMESTAMP);
            column.set_datatype(ColumnDataType::TIMESTAMP_MILLISECOND);
            auto values = column.mutable_values();
            for (const auto& ts : ts_vec) {
                values->add_ts_millisecond_values(ts);
            }
            insert_request.add_columns()->Swap(&column);
        }
        // field
        for (const auto&[field_name, field_vals] : field_map) {
            assert(row_count == field_vals.size());
            column.Clear();
            column.set_column_name(field_name);
            column.set_semantic_type(Column_SemanticType::Column_SemanticType_FIELD);
            column.set_datatype(ColumnDataType::FLOAT32);
            auto values = column.mutable_values();
            for (const auto& field_val : field_vals) {
                values->add_f32_values(field_val);
            }
            insert_request.add_columns()->Swap(&column);
        }
        insert_vec.emplace_back(std::move(insert_request));
    }
    return insert_vec;
}
