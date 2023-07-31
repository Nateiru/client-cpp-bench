# Copyright 2023 Greptime Team
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required (VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)              # Compile as C++17.
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++17 support.

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") # Enable optimization level 3


project (cleint_bench VERSION 0.1 LANGUAGES CXX)

# generate complie_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set project root directory
set(CLIENT_BENCH_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CLIENT_BENCH_THIRD_PARTY_INCLUDE_DIR 
        ${CLIENT_BENCH_ROOT_DIR}/contrib/greptimedb-client-cpp
        ${CLIENT_BENCH_ROOT_DIR}/contrib/json/include
        ${CLIENT_BENCH_ROOT_DIR}/contrib/cpp-httplib
        )

set(USE_GREPTIME_CLIENT_THIRD_PARTY_INCLUDE_DIR 
        ${CLIENT_BENCH_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/greptime-proto/c++
        ${CLIENT_BENCH_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/grpc/include
        ${CLIENT_BENCH_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/grpc/third_party/abseil-cpp
        ${CLIENT_BENCH_ROOT_DIR}/contrib/greptimedb-client-cpp/contrib/grpc/third_party/protobuf/src
        )

include_directories(
        ${CLIENT_BENCH_THIRD_PARTY_INCLUDE_DIR}
        ${USE_GREPTIME_CLIENT_THIRD_PARTY_INCLUDE_DIR}
        ${CLIENT_BENCH_ROOT_DIR}
        )

add_subdirectory(contrib)
add_subdirectory(src)
