cmake_minimum_required(VERSION 3.16)
project(zmq_demo)

set(CMAKE_CXX_STANDARD 14)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3  -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -Wall")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake CACHE INTERNAL "" FORCE)


set(SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../sdk/infinite_sense_core/x86)
find_package(OpenCV REQUIRED)
find_package(ZeroMQ QUIET)
if (ZeroMQ_FOUND)
  message(STATUS "ZMQ found")
  add_subdirectory(proto_msg)
  find_package(ZeroMQ REQUIRED)
  add_executable(${PROJECT_NAME}_zmq_node
      main_zmq.cpp
  )
  target_link_directories(${PROJECT_NAME}_zmq_node PRIVATE
      BEFORE ${SDK_PATH}/lib
  )
  target_include_directories(${PROJECT_NAME}_zmq_node PRIVATE
      ${ZeroMQ_INCLUDE_DIRS}
      ${SDK_PATH}/include
      ${OpenCV_INCLUDE_DIRS}
  )
  target_link_libraries(${PROJECT_NAME}_zmq_node PRIVATE
      ${ZeroMQ_LIBRARIES}
      infinite_sense_core
      proto_msg
      ${OpenCV_LIBS}
  )
endif ()
