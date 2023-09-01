#include "liautoinc.h"

using namespace::liautoinc;

int main() {

    std::string vname1 = "IntN";
    SignalTypeEnum datatype1 = SignalTypeEnum::int32Type;
    std::vector<SignalValue> value1 = {SignalValue((int32_t)1)};

    std::string vname2 = "DoubleN";
    SignalTypeEnum datatype2 = SignalTypeEnum::doubleType;
    std::vector<SignalValue> value2 = {SignalValue((double)1.1)};

    std::string vname3 = "ArrayN";
    SignalTypeEnum datatype3 = SignalTypeEnum::doubleType;
    std::vector<SignalValue> value3 = {SignalValue((double)1.1), SignalValue((double)2.2), SignalValue((double)3.3), SignalValue((double)4.4)};
    
    int canid = 114514;
    std::unordered_map<int, std::vector<std::pair<std::string, SignalTypeEnum>>> signalNameAndSchemaMap;

    // column and schema
    signalNameAndSchemaMap.emplace(
        canid,
        std::vector<std::pair<std::string, SignalTypeEnum>>{
            std::make_pair(vname1, datatype1),
            std::make_pair(vname2, datatype2),
            std::make_pair(vname3, datatype3)
        }
    );

    // values
    std::map<int, int> canIdSizeMap;
    canIdSizeMap.emplace(canid, 2);

    std::map<int,std::shared_ptr<std::vector<long>>> timeStampVec;
    timeStampVec.emplace(
        canid,
        std::make_shared<std::vector<long>>(
            std::vector<long>{
                111111111,
                111111112,
            }
        )
    );

    std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<SignalValue>>>>>>> valuesMap;

    std::vector<std::shared_ptr<std::vector<SignalValue>>> row1 {
        std::make_shared<std::vector<SignalValue>>(value1),
        std::make_shared<std::vector<SignalValue>>(value2),
        std::make_shared<std::vector<SignalValue>>(value3)
    };
    std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<SignalValue>>>>> row {
        std::make_shared<std::vector<std::shared_ptr<std::vector<SignalValue>>>>(row1),
        std::make_shared<std::vector<std::shared_ptr<std::vector<SignalValue>>>>(row1),
    };
    
    valuesMap.emplace(canid, std::make_shared<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<SignalValue>>>>>>(row));

    liautoinc::LiAutoIncClient liautoinc_client("public", "127.0.0.1:4001");
    try {
        liautoinc_client.setCanIdSignalNameList(signalNameAndSchemaMap);
        liautoinc_client.commitData(canIdSizeMap, timeStampVec, valuesMap);
        liautoinc_client.commitData(canIdSizeMap, timeStampVec, valuesMap);
        liautoinc_client.finish();
    }
    catch (const std::exception& e) // caught by reference to base
    {
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
    }
    return 0;
}
