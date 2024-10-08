cmake_minimum_required(VERSION 3.16)
project(CustomisedCamera)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE -Ofast)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3  -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -Wall")

# set sdk path
set(SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/infinite_sense_core/X86)
find_package(catkin QUIET)
if (catkin_FOUND)
    message(STATUS "ROS found, Compiling ROS nodes")
    find_package(catkin REQUIRED COMPONENTS
            roscpp
            sensor_msgs
            cv_bridge
            image_transport
            geometry_msgs
    )
    file(GLOB_RECURSE SOURCE_FILES
            main_ros.cpp
            customised_camera.cpp
    )
    add_executable(${PROJECT_NAME}_ros_node ${SOURCE_FILES})
    target_link_directories(${PROJECT_NAME}_ros_node PRIVATE
        ${SDK_PATH}/lib
    )
    target_link_libraries(${PROJECT_NAME}_ros_node
            ${catkin_LIBRARIES}
            infinite_sense_core
    )
    target_include_directories(${PROJECT_NAME}_ros_node PRIVATE
            ${catkin_INCLUDE_DIRS}
            ${SDK_PATH}/include
    )
    target_compile_definitions(${PROJECT_NAME}_ros_node PUBLIC -DROS_PLATFORM)
endif ()

