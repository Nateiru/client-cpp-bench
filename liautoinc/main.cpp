#include <liautoinc.h>
#include <cstdint>
#include <memory>

int main() {

    std::string vname1 = "IntN";
    liautoinc::ColumnDataType datatype1 = liautoinc::ColumnDataType::INT32;
    liautoinc::Variant value1 = (int32_t)1;

    liautoinc::ColumnDataType datatype2 = liautoinc::ColumnDataType::FLOAT64;
    std::string vname2 = "DoubleN";
    liautoinc::Variant value2 = (double)1.1;

    liautoinc::ColumnDataType datatype3 = liautoinc::ColumnDataType::STRING;
    std::string vname3 = "ArrayN";

    liautoinc::Variant value3 = (uint32_t)0;
    std::vector<std::string> binVec = {"[1,1,1,1]"};
    

    int canid = 114514;

    std::unordered_map<int, std::vector<std::tuple<std::string, liautoinc::ColumnDataType>>> signalNameAndSchemaMap;

    // column and schema
    signalNameAndSchemaMap.emplace(
        canid,
        std::vector<std::tuple<std::string, liautoinc::ColumnDataType>>{
            std::make_tuple(vname1, datatype1),
            std::make_tuple(vname2, datatype2),
            std::make_tuple(vname3, datatype3)
        }
    );

    // values
    std::map<int, int> canIdSizeMap;
    canIdSizeMap.emplace(canid, 1);

    std::map<int,std::shared_ptr<std::vector<long>>> timeStampVec;
    timeStampVec.emplace(
        canid,
        std::make_shared<std::vector<long>>(
            std::vector<long>{
                111111111,
            }
        )
    );

    std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<liautoinc::Variant>>>>> valuesMap;

    std::vector<liautoinc::Variant> row1{
        value1,
        value2,
        value3
    };

    std::vector<std::shared_ptr<std::vector<liautoinc::Variant>>> row {
        std::make_shared<std::vector<liautoinc::Variant>>(row1)
    };
    
    valuesMap.emplace(canid, std::make_shared<std::vector<std::shared_ptr<std::vector<liautoinc::Variant>>>>(row));

    liautoinc::LiAutoIncClient liautoinc_client("public", "127.0.0.1:4001");
    try {
        liautoinc_client.setCanIdSignalNameList(signalNameAndSchemaMap);
        liautoinc_client.commitData(canIdSizeMap, timeStampVec, valuesMap, binVec);
        liautoinc_client.finish();
    }
    catch (const std::exception& e) // caught by reference to base
    {
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
    }
    return 0;
}
