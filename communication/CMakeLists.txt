cmake_minimum_required(VERSION 2.8)
project(SIOX)

# find_package(PostgreSQL REQUIRED)

include_directories(
	"${PROJECT_BINARY_DIR}"
# 	"${PostgreSQL_INCLUDE_DIR}"
# 	"util"
)

link_directories(
# 	"${PostgreSQL_LIBRARY_DIR}"
# 	"${PROJECT_SOURCE_DIR}/util"
)

# set(CMAKE_C_FLAGS "-ggdb -Wall -lpq -lrt")
set(CMAKE_C_FLAGS "-ggdb -Wall")

# add_library(libiorec
# 	libiorec.h
# 	libiorec.c 
# 	io_history.h
# 	io_history.c
# 	psql_storage.h
# 	psql_storage.c
# 	util/misc.h
# 	util/misc.c
# 	util/stack.h
# 	util/stack.c
# 	util/red_black_tree.h
# 	util/red_black_tree.c
# )

# add_executable(io_test io_test.c)
add_executable(sioxd 
	sioxd.cpp
	CallbackWorker.h
	CallbackWorker.cpp
	ConnectionFactory.h
	ConnectionFactory.cpp
	IPCConnection.h
	IPCConnection.cpp
	IPCListener.h
	IPCListener.cpp
	Listener.h
	Listener.cpp
	ListenerFactory.h
	ListenerFactory.cpp
	Message.h
	Message.cpp
	MessageCallback.h
	MessageCallback.cpp
	MessageClient.h
	MessageClient.cpp
	MessageHandler.h
	MessageHandler.cpp
	MessageQueue.h
	MessageQueue.cpp
	MessageServer.h
	MessageServer.cpp
	Messenger.h
	Messenger.cpp
	QueuedMessageConnection.h
	QueuedMessageConnection.cpp
	TCPConnection.h
	TCPConnection.cpp
	TCPListener.h
	TCPListener.cpp
	URI.h
	URI.cpp
	Worker.h
	Worker.cpp
	WorkerPool.h
	WorkerPool.cpp
)

# target_link_libraries(io_test libiorec)
target_link_libraries(sioxd)
