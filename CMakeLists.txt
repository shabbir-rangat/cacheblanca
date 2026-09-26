cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Add all your .cpp files here
add_executable(my_program src/main.cpp src/tcp_server.cpp src/eventloop.cpp  src/dispatcher.cpp 

  src/resp.cpp src/Store.cpp src/cbdecoder.cpp 

)


