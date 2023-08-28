#include "liautoinc.h"

int main() {

    std::string vname1 = "IntN";
    liatoinc::ColumnDataType datatype1 = liatoinc::ColumnDataType::INT32;
    std::vector<liatoinc::Variant> value1 = {(int32_t)1};

    liatoinc::ColumnDataType datatype2 = liatoinc::ColumnDataType::FLOAT64;
    std::string vname2 = "DoubleN";
    std::vector<liatoinc::Variant> value2 = {(double)1.1};

    liatoinc::ColumnDataType datatype3 = liatoinc::ColumnDataType::STRING;
    std::string vname3 = "ArrayN";
    std::vector<liatoinc::Variant> value3 = {(double)1.1, (double)2.2, (double)3.3, (double)4.4};
    
    int canid = 114514;

    std::unordered_map<int, std::vector<std::tuple<std::string, liatoinc::ColumnDataType>>> signalNameAndSchemaMap;

    // column and schema
    signalNameAndSchemaMap.emplace(
        canid,
        std::vector<std::tuple<std::string, liatoinc::ColumnDataType>>{
            std::make_tuple(vname1, datatype1),
            std::make_tuple(vname2, datatype2),
            std::make_tuple(vname3, datatype3)
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

    std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<liatoinc::Variant>>>>>>> valuesMap;

    std::vector<std::shared_ptr<std::vector<liatoinc::Variant>>> row1 {
        std::make_shared<std::vector<liatoinc::Variant>>(value1),
        std::make_shared<std::vector<liatoinc::Variant>>(value2),
        std::make_shared<std::vector<liatoinc::Variant>>(value3)
    };
    std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<liatoinc::Variant>>>>> row {
        std::make_shared<std::vector<std::shared_ptr<std::vector<liatoinc::Variant>>>>(row1),
        std::make_shared<std::vector<std::shared_ptr<std::vector<liatoinc::Variant>>>>(row1),
    };
    
    valuesMap.emplace(canid, std::make_shared<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<liatoinc::Variant>>>>>>(row));

    try {
        liatoinc::setCanIdSignalNameList(signalNameAndSchemaMap);
        liatoinc::commitData(canIdSizeMap, timeStampVec, valuesMap);
    }
    catch (const std::exception& e) // caught by reference to base
    {
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
    }
    return 0;
}
