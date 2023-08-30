#include <string>
#include <optional>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "liautoinc.h"

#include <map>
#include <tuple>
#include <memory>
#include <vector>
#include <cstdint>
#include <variant>

#include <greptime/v1/common.pb.h>
#include <greptime/v1/column.pb.h>
#include <greptime/v1/database.pb.h>
#include <greptime/v1/column.grpc.pb.h>
#include <greptime/v1/database.grpc.pb.h>

namespace liautoinc {

void LiAutoIncClient::setCanIdSignalNameList(std::unordered_map<
                int, 
                std::vector<std::tuple<std::string, ColumnDataType>>> signalNameAndSchemaMap_) {

    signalNameAndSchemaMap = std::move(signalNameAndSchemaMap_);
}

using Variant = std::variant<
                bool,
                int8_t,
                int16_t,
                int32_t,
                int64_t,
                uint8_t,
                uint16_t,
                uint32_t,
                uint64_t,
                float,
                double,
                std::string
                >;

void LiAutoIncClient::addValue(Column_Values *values, ColumnDataType datatype, Variant varValue) {
    switch (datatype) {
        case ColumnDataType::BOOLEAN: {
            if (!std::holds_alternative<bool>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<bool>(varValue);
            values->add_bool_values(value);
            break;
        }
        case ColumnDataType::INT8: {
            if (!std::holds_alternative<int8_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<int8_t>(varValue);
            values->add_i8_values(value);
            break;
        }
        case ColumnDataType::INT16: {
            if (!std::holds_alternative<int16_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<int16_t>(varValue);
            values->add_i16_values(value);
            break;
        }
        case ColumnDataType::INT32: {
            if (!std::holds_alternative<int32_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<int32_t>(varValue);
            values->add_i32_values(value);
            break;
        }
        case ColumnDataType::INT64: {
            if (!std::holds_alternative<int64_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<int64_t>(varValue);
            values->add_i64_values(value);
            break;
        }
        case ColumnDataType::UINT8: {
            if (!std::holds_alternative<uint8_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<uint8_t>(varValue);
            values->add_u8_values(value);
            break;
        }
        case ColumnDataType::UINT16: {
            if (!std::holds_alternative<uint16_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<uint16_t>(varValue);
            values->add_u16_values(value);
            break;
        }
        case ColumnDataType::UINT32: {
            if (!std::holds_alternative<uint32_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<uint32_t>(varValue);
            values->add_u32_values(value);
            break;
        }
        case ColumnDataType::UINT64: {
            if (!std::holds_alternative<uint64_t>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<uint64_t>(varValue);
            values->add_u64_values(value);
            break;
        }
        case ColumnDataType::FLOAT32: {
            if (!std::holds_alternative<float>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<float>(varValue);
            values->add_f32_values(value);
            break;
        }
        case ColumnDataType::FLOAT64: {
            if (!std::holds_alternative<double>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<double>(varValue);
            values->add_f64_values(value);
            break;
        }
        case ColumnDataType::STRING: {
            if (!std::holds_alternative<std::string>(varValue)) {
                throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
            }
            auto value = std::get<std::string>(varValue);
            values->add_string_values(value);
            break;
        }
        default:
            break;
    }

}

/*
 *  canIdSizeMap 　canid -> canidSize(表示该canid有多少条数据)
 ×　timeStampVec　canid->时间戳列表　(表示canid各条数据的时间戳)
 *　valuesMap　　canid->{canid各条数据集合，每条数据对应某个时间戳canid的value集合}
 *  throw exception if schema is inconsistent with data
 */
void LiAutoIncClient::commitData(std::map<int, int>  &canIdSizeMap,
                        std::map<int,std::shared_ptr<std::vector<long>>> &timeStampVec,
                        std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<Variant>>>>>>> &valuesMap) {
    for (const auto &[canid, n] : canIdSizeMap) {

        const auto & tsVec = timeStampVec[canid];
        const auto & valuesVec = valuesMap[canid];
        const auto & nameAndSchema = signalNameAndSchemaMap[canid];

        if(tsVec->size() != valuesVec->size()) {
            throw std::logic_error("The timestamp is inconsistent with the number of data rows");
        }

        if (n == 0) {
            return;
        }
        int m = valuesVec->at(0)->size();

        std::string table_name = "table_" + std::to_string(canid);

        InsertRequest insReq;
        insReq.set_table_name(table_name);
        insReq.set_row_count(n);
        // timestamp
        {
            Column column;
            column.set_column_name("ts");
            column.set_semantic_type(Column_SemanticType::Column_SemanticType_TIMESTAMP);
            column.set_datatype(ColumnDataType::TIMESTAMP_MILLISECOND);
            auto values = column.mutable_values();
            for (const auto& ts : *tsVec) {
                values->add_ts_millisecond_values(ts);
            }
            insReq.add_columns()->Swap(&column);
        }

        // n rows, m columns
        for (int j = 0; j < m; ++j) {
            const auto &[column_name, datatype] = nameAndSchema[j];
            Column column;
            column.set_column_name(column_name);
            column.set_semantic_type(Column_SemanticType::Column_SemanticType_FIELD);
            column.set_datatype(datatype);
            auto values = column.mutable_values();
            for (int i = 0; i < n; ++i) {
                assert(m == valuesVec->at(i)->size());
                const std::vector<Variant> &fields = *valuesVec->at(i)->at(j);
                if (fields.size() == 0) {
                    // null
                    throw std::logic_error("field is empty");
                } else if(fields.size() == 1) {
                    const Variant &value = fields[0];
                    addValue(values, datatype, value);

                } else {
                    // array [1, 2] -> std::string
                    nlohmann::json jsonArray = nlohmann::json::array();
                    for (const auto &v : fields) {
                        std::visit([&jsonArray](auto&& arg) {
                            jsonArray.push_back(arg);
                        }, v);
                    }
                    std::string value = jsonArray.dump();
                    // std::cout << "json array: " << value << std::endl;
                    values->add_string_values(value);
                }
            }
            insReq.add_columns()->Swap(&column);
        }
        database.stream_inserter.Write(std::move(insReq));
    }
}

}
