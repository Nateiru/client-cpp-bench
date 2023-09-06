#include <cstdint>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "liautoinc.h"
#include <src/database.h>

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

void addValue(Column_Values *values, const SignalTypeEnum &type, const SignalValue &typeValue) {
    switch (type) {
        case SignalTypeEnum::boolType: {
            auto value = typeValue.boolValue;
            values->add_bool_values(value);
            break;
        }
        case SignalTypeEnum::int8Type: {
            auto value = typeValue.int8Value;
            values->add_i8_values(value);
            break;
        }
        case SignalTypeEnum::int16Type: {
            auto value = typeValue.int16Value;
            values->add_i16_values(value);
            break;
        }
        case SignalTypeEnum::int32Type: {
            auto value = typeValue.int32Value;
            values->add_i32_values(value);
            break;
        }
        case SignalTypeEnum::int64Type: {
            auto value = typeValue.int64Value;
            values->add_i64_values(value);
            break;
        }
        case SignalTypeEnum::uint8Type: {
            auto value = typeValue.uint8Value;
            values->add_u8_values(value);
            break;
        }
        case SignalTypeEnum::uint16Type: {
            auto value = typeValue.uint8Value;
            values->add_u16_values(value);
            break;
        }
        case SignalTypeEnum::uint32Type: {
            auto value = typeValue.uint32Value;
            values->add_u32_values(value);
            break;
        }
        case SignalTypeEnum::uint64Type: {
            auto value = typeValue.uint64Value;
            values->add_u64_values(value);
            break;
        }
        case SignalTypeEnum::float32Type: {
            auto value = typeValue.float32Value;
            values->add_f32_values(value);
            break;
        }
        case SignalTypeEnum::doubleType: {
            auto value = typeValue.doubleValue;
            values->add_f64_values(value);
            break;
        }
        default:
            break;
    }

}

static ColumnDataType enumToDataType(SignalTypeEnum type) {
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
        case SignalTypeEnum::binType: {
            return ColumnDataType::STRING;
        }
        default:
            break;
    }
    throw std::logic_error("The data type indicated by the schema is inconsistent with the real data type");
    return ColumnDataType::STRING;
}

struct LiAutoIncClient::Database {
    Database(std::string dbname_, std::string greptimedb_endpoint_) : database(dbname_, greptimedb_endpoint_) {}
    greptime::Database database;
};

LiAutoIncClient::LiAutoIncClient(std::string dbname_, std::string greptimedb_endpoint_) : 
                    pimpl(new Database(dbname_, greptimedb_endpoint_)){}

LiAutoIncClient::~LiAutoIncClient() {}

/*
 *  canIdSizeMap 　canid -> canidSize(表示该canid有多少条数据)
 ×　timeStampVec　canid->时间戳列表　(表示canid各条数据的时间戳)
 *　valuesMap　　canid->{canid各条数据集合，每条数据对应某个时间戳canid的value集合}
 *  throw exception if schema is inconsistent with data
 */
void LiAutoIncClient::commitData(std::map<int, int>  &canIdSizeMap,
                        std::map<int,std::shared_ptr<std::vector<long>>> &timeStampVec,
                        std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<SignalValue>>>>> &valuesMap,
                        std::vector<std::string> &binaryValue) {
    for (const auto &_ : canIdSizeMap) {

        const auto & canid = _.first;
        const auto & n = _.second;
        const auto & tsVec = timeStampVec[canid];
        const auto & valuesVec = valuesMap[canid];
        if (!signalNameAndSchemaMap.count(canid)) {
            std::cout << "no this table_" << canid << std::endl;
            continue;
        }
        const auto & nameAndSchema = signalNameAndSchemaMap[canid];


        if(tsVec->size() != valuesVec->size()) {
            throw std::logic_error("The timestamp is inconsistent with the number of data rows");
        }

        if (n == 0) {
            continue;
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
            if (column_name == "") {
                std::cout << "column_name is null" << std::endl;
                return;
            }
            column.set_column_name(column_name);
            column.set_semantic_type(Column_SemanticType::Column_SemanticType_FIELD);
            column.set_datatype(enumToDataType(signal_type_enum));
            auto values = column.mutable_values();
            for (int i = 0; i < n; ++i) {
                assert(m == valuesVec->at(i)->size());
                const SignalValue &field = valuesVec->at(i)->at(j);
                if (signal_type_enum == SignalTypeEnum::binType) {
                    uint32_t idx = field.uint32Value;
                    values->add_string_values(binaryValue[idx]);
                    continue;
                }
                addValue(values, signal_type_enum, field);
            }
            insReq.add_columns()->Swap(&column);
        }
        pimpl->database.stream_inserter->Write(std::move(insReq));
    }
}

void LiAutoIncClient::finish() {
    pimpl->database.stream_inserter->WriteDone();
    grpc::Status status = pimpl->database.stream_inserter->Finish();

    if (status.ok()) {
        std::cout << "success!" << std::endl;
        auto response = pimpl->database.stream_inserter->GetResponse();

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
