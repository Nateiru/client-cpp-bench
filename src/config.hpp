#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
struct Settings {
    std::string can_id_map_path = "/home/zhuziyi/Projects/client-cpp-bench/data/canIdMap.txt";
    std::string csv_data_path = "/home/zhuziyi/Projects/client-cpp-bench/data/data.csv";
    uint64_t insert_point_number = 80000000;
    uint32_t max_insert_point_per_sec = 300000000;
    uint32_t batch_number = 100; 
    uint32_t write_buffer_size = 512;
    std::string write_http_endpoint = "127.0.0.1:4000";
    Settings() = default;

    Settings(std::string config_file_path_) {
        std::ifstream file(config_file_path_);
        if (!file) {
            std::cerr << "Failed to open file: " << config_file_path_ << std::endl;
            return;
        }
        auto trim = [](std::string &s) -> void {
            if (!s.empty()) {
                s.erase(0,s.find_first_not_of(" "));
                s.erase(s.find_last_not_of(" ") + 1);
            }
        };
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key, value;

            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                trim(key);
                trim(value);
                if (key.find("can_id_map_path") != std::string::npos) {
                    can_id_map_path = value;
                }
                else if (key.find("csv_data_path") != std::string::npos) {
                    csv_data_path = value;
                }
                else if (key.find("insert_point_number") != std::string::npos) {
                    insert_point_number = std::stoull(value);
                }
                else if (key.find("max_insert_point_per_sec") != std::string::npos) {
                    max_insert_point_per_sec = std::stoul(value);
                }
                else if (key.find("batch_number") != std::string::npos) {
                    batch_number = std::stoul(value);
                }
                else if (key.find("write_buffer_size") != std::string::npos) {
                    write_buffer_size = std::stoul(value);
                }
                else if (key.find("write_http_endpoint") != std::string::npos) {
                   write_http_endpoint = value; 
                }
            }
        }
    }
};

struct BenchConfig {
    Settings settings;
    BenchConfig() = default;
    BenchConfig(std::string config_file_path_):settings(config_file_path_){}

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

    auto batch_number() -> uint64_t {
        return settings.batch_number;
    }
    
    auto write_buffer_size() -> uint32_t {
        return settings.write_buffer_size;
    }

    auto write_http_endpoint() -> std::string {
        return settings.write_http_endpoint;
    }
};
