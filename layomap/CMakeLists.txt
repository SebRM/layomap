# I really have no idea about cmake but I got good help from ChatGPT.... and it works.

cmake_minimum_required(VERSION 3.1)
project(layomap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Specify where the header files are
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_executable(layomap main.cpp)

target_link_libraries(layomap
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  # Mapper libraries:
  Mapper_Common
  #mapper-gdal
  scaling-icon-engine
  printsupport
  #mapper-sensors
  #nmea_position_source
  #powershell_position_source
  #fake_position_source
  QtSingleApplication
  #cove-vectorizer
  #cove
  #cove-potrace
  # Add other necessary libraries here...
)