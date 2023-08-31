CXX = /opt/android-ndk-r23c/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++
CXXFLAGS = -Wall -Wextra -std=c++17 -static

ROOT_DIR = $(shell pwd)

INCLUDE_DIR = $(ROOT_DIR)/include

LIB_DIR = $(ROOT_DIR)/lib
LIBS = -lliautoinc

main: main.o
	$(CXX) $(CXXFLAGS) -L$(LIB_DIR) -o main main.o $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c main.cpp -o main.o

clean: 
	rm -rf main main.o
