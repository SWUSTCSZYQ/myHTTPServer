cmake_minimum_required(VERSION 3.21)
project(myHTTPServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread")

include_directories(LogSystem tools test server uri)

add_executable(myHTTPServer main.cpp LogSystem/Logging.cpp LogSystem/Logging.h LogSystem/LogStream.cpp LogSystem/LogStream.h tools/nonCopyable.h LogSystem/AsyncLogging.cpp LogSystem/AsyncLogging.h tools/MutexLock.h tools/Condition.h tools/CountDownLatch.cpp tools/CountDownLatch.h tools/Thread.cpp tools/Thread.h tools/CurrentThread.h LogSystem/LogFile.cpp LogSystem/LogFile.h LogSystem/FileUtil.cpp LogSystem/FileUtil.h server/util.cpp server/util.h uri/RequestData.cpp uri/RequestData.h server/Epoll.cpp server/Epoll.h server/Timer.cpp server/Timer.h server/threadpool.cpp server/threadpool.h router/Trie.cpp router/Trie.h router/web.cpp router/web.h router/router.cpp router/router.h router/context.cpp router/context.h)
