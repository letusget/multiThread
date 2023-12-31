cmake_minimum_required(VERSION 3.0)
project(ThreadPool_cpp)

set(CMAKE_CXX_STANDARD 11)

# 包含头文件目录
include_directories(include)

# 手动列出所有源文件
set(SOURCE_FILES
    ./src/main.cpp
    ./src/TaskQueue.cpp
    ./src/threadPool.cpp
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 查找线程库
find_package(Threads REQUIRED)

# 链接线程库
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(PUBLIC ${PROJECT_NAME} "-pthread")
endif()

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
endif()
