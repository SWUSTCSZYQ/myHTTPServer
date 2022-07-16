cmake_minimum_required(VERSION 3.21)
project(myHTTPServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread")

include_directories(LogSystem tools test server uri cache)

add_executable(myHTTPServer test/consistentHash_test.cpp LogSystem/Logging.cpp LogSystem/Logging.h LogSystem/LogStream.cpp LogSystem/LogStream.h tools/nonCopyable.h LogSystem/AsyncLogging.cpp LogSystem/AsyncLogging.h tools/MutexLock.h tools/Condition.h tools/CountDownLatch.cpp tools/CountDownLatch.h tools/Thread.cpp tools/Thread.h tools/CurrentThread.h LogSystem/LogFile.cpp LogSystem/LogFile.h LogSystem/FileUtil.cpp LogSystem/FileUtil.h server/util.cpp server/util.h uri/RequestData.cpp uri/RequestData.h server/Epoll.cpp server/Epoll.h server/Timer.cpp server/Timer.h server/Threadpool.cpp server/Threadpool.h router/Trie.cpp router/Trie.h router/Web.cpp router/Web.h router/Router.cpp router/Router.h router/Context.cpp router/Context.h cache/LeastRecentlyUsed.h cache/Cache.cpp cache/Cache.h cache/CacheControl.cpp cache/CacheControl.h cache/CacheHttpServer.cpp cache/CacheHttpServer.h cache/ConsistentHash.cpp cache/ConsistentHash.h cache/HashFunc.h cache/MD5HashFunc.cpp cache/MD5HashFunc.h cache/md5.cpp cache/md5.h cache/Node.cpp cache/Node.h)
