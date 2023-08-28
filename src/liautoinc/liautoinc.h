#include "nlohmann/json_fwd.hpp"
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <regex>
#include <set>
#include <memory>
#include <tuple>
#include <vector>

#include <unordered_map>

#include <greptime/v1/common.pb.h>
#include <greptime/v1/database.grpc.pb.h>
#include <greptime/v1/database.pb.h>
#include <greptime/v1/column.pb.h>
#include <string>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include <src/database.h>
#include <stdexcept>

namespace liatoinc {

using greptime::v1::Column;
using greptime::v1::Column_SemanticType;
using greptime::v1::ColumnDataType;
using greptime::v1::InsertRequest;
using greptime::v1::Column_Values;

void setCanIdSignalNameList(std::unordered_map<
                int, 
                std::vector<std::tuple<std::string, ColumnDataType>>> &signalNameAndSchemaMap);

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

void addValue(Column_Values *values, ColumnDataType datatype, Variant varValue);

/*
* canIdSizeMap 　canid -> canidSize(表示该canid有多少条数据)
×　timeStampVec　canid->时间戳列表　(表示canid各条数据的时间戳)
*　valuesMap　　canid->{canid各条数据集合，每条数据对应某个时间戳canid的value集合}
*/
void commitData(std::map<int, int>  &canIdSizeMap,
                        std::map<int,std::shared_ptr<std::vector<long>>> &timeStampVec,
                        std::map<int,std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<Variant>>>>>>> &valuesMap);

};