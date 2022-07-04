cmake_minimum_required(VERSION 3.21)
project(myHTTPServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread")

include_directories(LogSystem tools test)

add_executable(myHTTPServer test/logSystemTest.cpp LogSystem/Logging.cpp LogSystem/Logging.h LogSystem/LogStream.cpp LogSystem/LogStream.h tools/nonCopyable.h LogSystem/AsyncLogging.cpp LogSystem/AsyncLogging.h tools/MutexLock.h tools/Condition.h tools/CountDownLatch.cpp tools/CountDownLatch.h tools/Thread.cpp tools/Thread.h tools/CurrentThread.h LogSystem/LogFile.cpp LogSystem/LogFile.h LogSystem/FileUtil.cpp LogSystem/FileUtil.h test/logSystemTest.cpp)
