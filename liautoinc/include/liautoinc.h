#include <map>
#include <tuple>
#include <memory>
#include <vector>
#include <cstdint>
#include <variant>
#include <src/database.h>
#include <greptime/v1/common.pb.h>
#include <greptime/v1/column.pb.h>
#include <greptime/v1/database.pb.h>
#include <greptime/v1/column.grpc.pb.h>
#include <greptime/v1/database.grpc.pb.h>

namespace liautoinc {

using greptime::v1::Column;
using greptime::v1::Column_SemanticType;
using greptime::v1::ColumnDataType;
using greptime::v1::InsertRequest;
using greptime::v1::Column_Values;
using greptime::Database;
using greptime::StreamInserter;

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

class LiAutoIncClient {

public:
    /*
    * 构造函数传入【数据库名】和【IP:端口号】
    * 端口指 greptimedb gRPC service 端口
    * 比如：liautoinc::LiAutoIncClient liautoinc_client("public", "localhost:4001");
    */
    LiAutoIncClient(std::string dbname_, std::string greptimedb_endpoint_) : 
                    database(dbname_, greptimedb_endpoint_){}

    /*
    * 设置列名和schema
    */
    void setCanIdSignalNameList(std::unordered_map<
                    int, 
                    std::vector<std::tuple<std::string, ColumnDataType>>> signalNameAndSchemaMap);


    /*
    *  canIdSizeMap 　canid -> canidSize(表示该canid有多少条数据)
    ×　timeStampVec　canid->时间戳列表　(表示canid各条数据的时间戳)
    *　valuesMap　　canid->{canid各条数据集合，每条数据对应某个时间戳canid的value集合}
    */
    void commitData(std::map<int, int>  &canIdSizeMap,
                            std::map<int,std::shared_ptr<std::vector<long>>> &timeStampVec,
                            std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<Variant>>>>>>> &valuesMap);

    /*
    * commitData一只写入数据，最终需要调用 finish 返回写入的状态
    * 调用 finish 后整个 Client 写入结束
    */
    void finish() {
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

private:

    void addValue(Column_Values *values, ColumnDataType datatype, Variant varValue);

    std::unordered_map<int, std::vector<std::tuple<std::string, ColumnDataType>>> signalNameAndSchemaMap;

    Database database;
};
}
