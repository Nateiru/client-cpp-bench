#include <stdexcept>
#include <nlohmann/json.hpp>

#include "liautoinc.h"

#include <greptime/v1/column.pb.h>
#include <greptime/v1/column.grpc.pb.h>

namespace liautoinc {

using greptime::v1::Column;
using greptime::v1::Column_SemanticType;
using greptime::v1::ColumnDataType;
using greptime::v1::InsertRequest;
using greptime::v1::Column_Values;

void LiAutoIncClient::setCanIdSignalNameList(std::unordered_map<
                int, 
                std::vector<std::pair<std::string, SignalTypeEnum>>> signalNameAndSchemaMap_) {

    signalNameAndSchemaMap = std::move(signalNameAndSchemaMap_);
}

void addValue(Column_Values *values, const SignalValue &typeValue) {
    switch (typeValue.type) {
        case SignalTypeEnum::boolType: {
            auto value = typeValue.value.boolValue;
            values->add_bool_values(value);
            break;
        }
        case SignalTypeEnum::int8Type: {
            auto value = typeValue.value.int8Value;
            values->add_i8_values(value);
            break;
        }
        case SignalTypeEnum::int16Type: {
            auto value = typeValue.value.int16Value;
            values->add_i16_values(value);
            break;
        }
        case SignalTypeEnum::int32Type: {
            auto value = typeValue.value.int32Value;
            values->add_i32_values(value);
            break;
        }
        case SignalTypeEnum::int64Type: {
            auto value = typeValue.value.int64Value;
            values->add_i64_values(value);
            break;
        }
        case SignalTypeEnum::uint8Type: {
            auto value = typeValue.value.uint8Value;
            values->add_u8_values(value);
            break;
        }
        case SignalTypeEnum::uint16Type: {
            auto value = typeValue.value.uint8Value;
            values->add_u16_values(value);
            break;
        }
        case SignalTypeEnum::uint32Type: {
            auto value = typeValue.value.uint32Value;
            values->add_u32_values(value);
            break;
        }
        case SignalTypeEnum::uint64Type: {
            auto value = typeValue.value.uint64Value;
            values->add_u64_values(value);
            break;
        }
        case SignalTypeEnum::float32Type: {
            auto value = typeValue.value.float32Value;
            values->add_f32_values(value);
            break;
        }
        case SignalTypeEnum::doubleType: {
            auto value = typeValue.value.doubleValue;
            values->add_f64_values(value);
            break;
        }
        default:
            break;
    }

}

void addStringValue(Column_Values *values, const std::vector<SignalValue> &typeValues) {
    nlohmann::json jsonArray = nlohmann::json::array();
    auto type = typeValues.back().type;
    switch (type) {
        case SignalTypeEnum::boolType: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.boolValue;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::int8Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.int8Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::int16Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.int16Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::int32Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.int32Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::int64Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.int64Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::uint8Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.uint8Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::uint16Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.uint8Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::uint32Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.uint32Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::uint64Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.uint64Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::float32Type: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.float32Value;
                jsonArray.push_back(value);
            }
            break;
        }
        case SignalTypeEnum::doubleType: {
            for (const auto & typeValue : typeValues) { 
                auto value = typeValue.value.doubleValue;
                jsonArray.push_back(value);
            }
            break;
        }
        default:
            break;
    }
    values->add_string_values(jsonArray.dump());
}

ColumnDataType enumToDataType(SignalTypeEnum type) {
    switch (type) {
        case SignalTypeEnum::boolType: {
            return ColumnDataType::BOOLEAN;
        }
        case SignalTypeEnum::int8Type: {
            return ColumnDataType::INT8;
        }
        case SignalTypeEnum::int16Type: {
            return ColumnDataType::INT16;
        }
        case SignalTypeEnum::int32Type: {
            return ColumnDataType::INT32;
        }
        case SignalTypeEnum::int64Type: {
            return ColumnDataType::INT64;
        }
        case SignalTypeEnum::uint8Type: {
            return ColumnDataType::UINT8;
        }
        case SignalTypeEnum::uint16Type: {
            return ColumnDataType::UINT16;
        }
        case SignalTypeEnum::uint32Type: {
            return ColumnDataType::UINT32;
        }
        case SignalTypeEnum::uint64Type: {
            return ColumnDataType::UINT64;
        }
        case SignalTypeEnum::float32Type: {
            return ColumnDataType::FLOAT32;
        }
        case SignalTypeEnum::doubleType: {
            return ColumnDataType::FLOAT64;
        }
        default:
            break;
    }
    throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
    return ColumnDataType::STRING;
}

/*
 *  canIdSizeMap 　canid -> canidSize(表示该canid有多少条数据)
 ×　timeStampVec　canid->时间戳列表　(表示canid各条数据的时间戳)
 *　valuesMap　　canid->{canid各条数据集合，每条数据对应某个时间戳canid的value集合}
 *  throw exception if schema is inconsistent with data
 */
void LiAutoIncClient::commitData(std::map<int, int>  &canIdSizeMap,
                        std::map<int,std::shared_ptr<std::vector<long>>> &timeStampVec,
                        std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<SignalValue>>>>>>> &valuesMap) {
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
            const auto &column_name = nameAndSchema[j].first;
            const auto &signal_type_enum = nameAndSchema[j].second;
            Column column;
            column.set_column_name(column_name);
            column.set_semantic_type(Column_SemanticType::Column_SemanticType_FIELD);
            column.set_datatype(enumToDataType(signal_type_enum));
            auto values = column.mutable_values();
            for (int i = 0; i < n; ++i) {
                assert(m == valuesVec->at(i)->size());
                const std::vector<SignalValue> &fields = *valuesVec->at(i)->at(j);
                if (fields.size() == 0) {
                    // null
                    throw std::logic_error("field is empty");
                } else if(fields.size() == 1) {
                    // only fields[0]
                    addValue(values, fields[0]);
                } else {
                    // array [1, 2] -> std::string
                    column.set_datatype(ColumnDataType::STRING);
                    addStringValue(values, fields);
                }
            }
            insReq.add_columns()->Swap(&column);
        }
        database.stream_inserter.Write(std::move(insReq));
    }
}

void LiAutoIncClient::finish() {
    database.stream_inserter.WriteDone();
    grpc::Status status = database.stream_inserter.Finish();

    if (status.ok()) {
        std::cout << "success!" << std::endl;
        auto response = database.stream_inserter.GetResponse();

        std::cout << "notice: [";
        std::cout << response.affected_rows().value() << "] ";
        std::cout << "rows of data are successfully inserted into the public database"<< std::endl;
    } else {
        std::cout << "fail!" << std::endl;
        std::string emsg = "error message: " + status.error_message() + "\nerror details: " + status.error_details() + "\n"; 
        throw std::runtime_error(emsg);
    }
}
}
