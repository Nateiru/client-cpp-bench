#pragma once

#include <map>
#include <memory>
#include <vector>
#include <cstdint>
#include <unordered_map>

namespace liautoinc {

enum SignalTypeEnum {
    boolType = 0,
    int8Type,
    int16Type,
    int32Type,
    int64Type,
    uint8Type,
    uint16Type,
    uint32Type,
    uint64Type,
    float32Type,
    doubleType,
    binType,
};

union SignalValue {
    bool boolValue;
    int8_t int8Value;
    int16_t int16Value;
    int32_t int32Value;
    int64_t int64Value;
    uint8_t uint8Value;
    uint16_t uint16Value;
    uint32_t uint32Value;
    uint64_t uint64Value;
    float float32Value;
    double doubleValue;
};

class LiAutoIncClient {

public:
    /*
    * 构造函数传入【数据库名】和【IP:端口号】
    * 端口指 greptimedb gRPC service 端口
    * 比如：liautoinc::LiAutoIncClient liautoinc_client("public", "localhost:4001");
    */
    LiAutoIncClient(std::string dbname_, std::string greptimedb_endpoint_);

    /*
    * 设置列名和schema
    */
    void setCanIdSignalNameList(std::unordered_map<
                    int, 
                    std::vector<std::pair<std::string, SignalTypeEnum>>> signalNameAndSchemaMap);


    /*
    *  canIdSizeMap 　canid -> canidSize(表示该canid有多少条数据)
    ×　timeStampVec　canid->时间戳列表　(表示canid各条数据的时间戳)
    *　valuesMap　　canid->{canid各条数据集合，每条数据对应某个时间戳canid的value集合}
    */
    void commitData(std::map<int, int>  &canIdSizeMap,
                            std::map<int,std::shared_ptr<std::vector<long>>> &timeStampVec,
                            std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<SignalValue>>>>> &valuesMap,
                            std::vector<std::string> &binaryValue);

    /*
    * commitData一只写入数据，最终需要调用 finish 返回写入的状态
    * 调用 finish 后整个 Client 写入结束
    */
    void finish();

    ~LiAutoIncClient();
private:
    std::unordered_map<int, std::vector<std::pair<std::string, SignalTypeEnum>>> signalNameAndSchemaMap;

    struct Database;
    std::unique_ptr<Database> pimpl;
};
}
