# client-cpp-bench

## Build

```bash
git clone --recurse-submodules --depth 1 --shallow-submodules https://github.com/Nateiru/client-cpp-bench.git

cd client-cpp-bench

# create a new build directory where the project is compiled
mkdir build && cd build

cmake ..

make -j$(nproc)
```

## LiAutoInc
```bash
./mvh.sh

cd liautoinc

make -j$(nproc)

./main
```





## Data

```bash
# config.toml
can_id_map_path = /home/zhuziyi/Projects/client-cpp-bench/data/canIdMap.txt
csv_data_path = /home/zhuziyi/Projects/client-cpp-bench/data/data.csv
insert_point_number = 2000000
max_insert_point_per_sec = 300000000
write_task_number = 1
write_buffer_size = 512
batch_number = 100
write_http_endpoint = 127.0.0.1:4000
```
- can_id_map_path 是 Schama 文件的绝对路径
- csv_data_path 是 CSV 数据文件
- max_insert_point_per_sec 控制压测程序插入的点数
- batch_number 设置攒批大小

## Run

```bash
# 编译后的文件在 build/ 目录下
cd client-cpp-bench/build

# 参数是配置文件的绝对路径
./src/bench /home/zhuziyi/Projects/client-cpp-bench/config.toml
```
