cmake_minimum_required(VERSION 3.21)
project(myHTTPServer_1_1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread")

include_directories(tools uri LogSystem router)

add_executable(myHTTPServer_1_1 main.cpp Server.cpp Server.h Epoll.cpp Epoll.h Timer.cpp Timer.h tools/MutexLock.h tools/Condition.h tools/CountDownLatch.cpp tools/CountDownLatch.h tools/Thread.cpp tools/Thread.h tools/CurrentThread.h uri/RequestData.cpp uri/RequestData.h utils.cpp utils.h ThreadPool.cpp ThreadPool.h LogSystem/Logging.cpp LogSystem/Logging.h LogSystem/LogStream.cpp LogSystem/LogStream.h LogSystem/AsyncLogging.cpp LogSystem/AsyncLogging.h LogSystem/LogFile.cpp LogSystem/LogFile.h LogSystem/FileUtil.cpp LogSystem/FileUtil.h router/Trie.cpp router/Trie.h router/Router.cpp router/Router.h router/Context.cpp router/Context.h router/RouterNode.cpp router/RouterNode.h)
