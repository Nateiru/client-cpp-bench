#pragma once
#include <iostream>

struct Settings {
    std::string can_id_map_path = "/home/zhuziyi/Projects/client-cpp-bench/data/canIdMap.txt";
    std::string csv_data_path = "/home/zhuziyi/Projects/client-cpp-bench/data/a.csv";
    uint32_t insert_point_number = 2000000;
    uint32_t max_insert_point_per_sec = 300000000;
    uint32_t batch_number = 100; 
    uint32_t write_buffer_size = 512;
    std::string write_http_endpoint = "127.0.0.1:4000";
    Settings() = default;
    Settings(std::string csv_data_path_) : csv_data_path(csv_data_path_){}
};

struct BenchConfig {
    Settings settings;
    BenchConfig() = default;
    BenchConfig(std::string csv_data_path_):settings(csv_data_path_){}

    auto can_id_map_path() -> std::string {
        return settings.can_id_map_path;
    }

    auto csv_data_path() -> std::string {
        return settings.csv_data_path;
    }

    auto insert_point_number() -> uint32_t {
        return settings.insert_point_number;
    }

    auto max_insert_point_per_sec() -> uint32_t {
        return settings.max_insert_point_per_sec;
    }

    auto batch_number() -> uint32_t {
        return settings.batch_number;
    }
    
    auto write_buffer_size() -> uint32_t {
        return settings.write_buffer_size;
    }

    auto write_http_endpoint() -> std::string {
        return settings.write_http_endpoint;
    }
};
