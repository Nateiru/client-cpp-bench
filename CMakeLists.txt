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
set(CMAKE_CXX_STANDARD 11)              # Compile as C++11.
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++11 support.

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

set(CMAKE_SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# bundle_static_library(liautoinc zhuziyi)
function(bundle_static_library tgt_name bundled_tgt_name)
  # 将 ${tgt_name} 添加到 static_libs
  list(APPEND static_libs ${tgt_name})

  # 递归找到所有静态库
  function(_recursively_collect_dependencies input_target)
    # 将 _input_link_libraries 设置为 input_target
    set(_input_link_libraries LINK_LIBRARIES)
    # 获得 input_target 的 TYPE 比如 静态库、可执行文件、INTERFACE_LIBRARY 接口库 .....
    get_target_property(_input_type ${input_target} TYPE)

    if (${_input_type} STREQUAL "INTERFACE_LIBRARY")
      set(_input_link_libraries INTERFACE_LINK_LIBRARIES)
    endif()
    # 找到 input_target 在 CMake 中 target_link_libraries 的链接库
    get_target_property(public_dependencies ${input_target} ${_input_link_libraries})
    message(STATUS " ===== ${input_target} is ${_input_link_libraries}: [${public_dependencies}] ===== ")

    foreach(dependency IN LISTS public_dependencies)
      # 检查 ${dependency} 是否是一个有效的目标
      if(TARGET ${dependency})
        # 如果一个目标被别名引用，ALIASED_TARGET 属性会返回这个别名的目标名称。
        get_target_property(alias ${dependency} ALIASED_TARGET)
        if (TARGET ${alias})
          # 将 dependency 设置为别名目标的名称，以便后续使用。
          set(dependency ${alias})
        endif()
        get_target_property(_type ${dependency} TYPE)
        if (${_type} STREQUAL "STATIC_LIBRARY")
          list(APPEND static_libs ${dependency})
        endif()

        get_property(library_already_added
          GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency})
        if (NOT library_already_added)
          set_property(GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency} ON)
          _recursively_collect_dependencies(${dependency})
        endif()
      endif()
    endforeach()
    set(static_libs ${static_libs} PARENT_SCOPE)
  endfunction()

  _recursively_collect_dependencies(${tgt_name})

  # 移除重复的元素
  list(REMOVE_DUPLICATES static_libs)

  message("所依赖的静态库：${static_libs}")

  # CMAKE_BINARY_DIR 是 build 目录
  message("BINARY_DIR路径：${CMAKE_BINARY_DIR}")

  set(bundled_tgt_full_name 
    ${CMAKE_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${bundled_tgt_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

  if (CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
    file(WRITE ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in
      "CREATE ${bundled_tgt_full_name}\n" )
    # 将 static_libs zzy -> ADDLIB 形如 ADDLIB $<TARGET_FILE:zzy1>
    foreach(tgt IN LISTS static_libs)
      message("foreach: ${tgt}")
      file(APPEND ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in
        "ADDLIB $<TARGET_FILE:${tgt}>\n")
    endforeach()
    
    file(APPEND ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in "SAVE\n")
    file(APPEND ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in "END\n")

    file(GENERATE
      OUTPUT ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar
      INPUT ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar.in)

    set(ar_tool ${CMAKE_AR})
    if (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
      set(ar_tool ${CMAKE_CXX_COMPILER_AR})
    endif()

    message("${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar")

    set(script_path "${CMAKE_SCRIPT_DIR}/rename_script.cmake")
    set(parameter1 "${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar")
    set(parameter2 "${CMAKE_BINARY_DIR}/temp")

    add_custom_command(
      COMMAND ${CMAKE_COMMAND} -DPARAMETER1=${parameter1} -DPARAMETER2=${parameter2} -P ${script_path} 
      COMMAND ${ar_tool} -M < ${CMAKE_BINARY_DIR}/${bundled_tgt_name}.ar
      OUTPUT ${bundled_tgt_full_name}
      COMMENT "Bundling ${bundled_tgt_name}"
      VERBATIM)
  endif()

  add_custom_target(bundling_target ALL DEPENDS ${bundled_tgt_full_name})
  add_dependencies(bundling_target ${tgt_name})

  add_library(${bundled_tgt_name} STATIC IMPORTED)
  set_target_properties(${bundled_tgt_name} 
    PROPERTIES 
      IMPORTED_LOCATION ${bundled_tgt_full_name}
      INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${tgt_name},INTERFACE_INCLUDE_DIRECTORIES>)
  add_dependencies(${bundled_tgt_name} bundling_target)

endfunction()

add_subdirectory(contrib)
add_subdirectory(src)
