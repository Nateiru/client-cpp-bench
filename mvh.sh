#!/bin/bash

SCRPIT_ROOT_DIR=$(pwd)
DESTINATION_DIR="$SCRPIT_ROOT_DIR/liautoinc/include"

echo $SCRPIT_ROOT_DIR
echo $DESTINATION_DIR

INCLUDE_DIR=(
        "${SCRPIT_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/greptime-proto/c++/greptime"
        "${SCRPIT_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/grpc/third_party/abseil-cpp/absl"
        "${SCRPIT_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/grpc/third_party/protobuf/src/google"
        "${SCRPIT_ROOT_DIR}/contrib/greptimedb-client-cpp/src"
        )

# 复制所需要的 .h 文件
mkdir -p "$SCRPIT_ROOT_DIR/liautoinc/include"
for SOURCE_DIR in "${INCLUDE_DIR[@]}"; do
   cp -r "$SOURCE_DIR" "$DESTINATION_DIR/"
done
cp -r "${SCRPIT_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/grpc/include/"* "$DESTINATION_DIR/"

# 复制静态库
mkdir -p "$SCRPIT_ROOT_DIR/liautoinc/lib"
cp -r "$SCRPIT_ROOT_DIR/build"/*.a  "$SCRPIT_ROOT_DIR/liautoinc/lib"

# 递归函数，用于删除非 .h 文件和空目录
clean_directory() {
    local dir="$1"
    for file in "$dir"/*; do
        if [[ -f "$file" && "${file##*.}" != "h" ]]; then
            rm -f "$file"
        elif [[ -d "$file" ]]; then
            clean_directory "$file"
            rmdir "$file" 2>/dev/null  # 尝试删除空目录，忽略错误
        fi
    done
}

DIRECTORY_TO_CLEAN="$SCRPIT_ROOT_DIR/liautoinc"
clean_directory "$DIRECTORY_TO_CLEAN"
