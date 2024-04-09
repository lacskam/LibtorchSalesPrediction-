cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(neiro LANGUAGES CXX)

set(Torch_DIR /home/asdxx/QtPr/Project/libtorch/share/cmake/Torch)
find_package(Torch REQUIRED)
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql Charts Network)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(neiro
  main.cpp
  file.cpp
  file.h
  mslq.cpp
  mslq.h
  plot.cpp
  plot.h
  nn.cpp
  nn.h
  weather.cpp
  weather.h
  /usr/include/nlohmann/json.hpp
)

# Возможно, следует изменить порядок целей (переместить target_link_libraries в конец)
# и использовать цели для Qt
target_link_libraries(neiro PRIVATE ${CURL_LIBRARIES})
target_link_libraries(neiro PRIVATE ${TORCH_LIBRARIES})
target_link_libraries(neiro PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::Network)

set_property(TARGET neiro PROPERTY CXX_STANDARD 17)
