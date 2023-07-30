#include <httplib.h>
#include <chrono>
#include "httpsql.h"

void create_table(const Core &core, const Sql &sql) {
    auto http_endpoint = core.config->write_http_endpoint();
    httplib::Client client(http_endpoint.c_str());
    httplib::Params params;
    params.emplace("sql", sql);

    auto res = client.Post("/v1/sql?db=public", params);

    // if (res && res->status == 200) {
    //     std::cout << "Create table success! SQL: " << sql << std::endl;
    // } else {
    //     if (res) {
    //         std::cerr << "Failed to create table, status code: " << res->status << std::endl;
    //     } else {
    //         std::cerr << "Failed to create table, error: " << res.error() << std::endl;
    //     }
    // }
}

void create_tables(const Core &core) {
    auto can_id_map = core.can_id_signals_map;
    auto buffer_size = core.config->write_buffer_size();
    auto sql_vec = can_id_map->createTableSqls(buffer_size);

    for (auto &sql : sql_vec) {
        create_table(core, sql);
    }
}

void prepare(const Core &core) {
    create_tables(core);
}

void truncate(const Core &core) {
    auto start_time = std::chrono::high_resolution_clock::now();

    auto can_id_map = core.can_id_signals_map;
    auto buffer_size = core.config->write_buffer_size();
    auto sql_vec = can_id_map->clearTableSqls(buffer_size);

    for (auto &sql : sql_vec) {
        create_table(core, sql);
    }
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    std::cout << "清空表时间：" << duration_us.count() << " 微秒" << std::endl;
    std::cout << "清空表时间：" << duration_ms.count() << " 毫秒" << std::endl;
    std::cout << "清空表时间：" << duration_s.count() << " 秒" << std::endl;

}
