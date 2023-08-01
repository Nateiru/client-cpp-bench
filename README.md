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

## Run

```bash
# 编译后的文件在 build/ 目录下
cd client-cpp-bench/build

# 参数是配置文件的绝对路径
./src/ssbench /home/zhuziyi/Projects/client-cpp-bench/config.toml
```