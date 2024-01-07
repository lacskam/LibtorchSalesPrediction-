cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(neiro LANGUAGES CXX)

# Укажите путь к директории, содержащей TorchConfig.cmake
set(Torch_DIR /home/asdxx/QtPr/Project/libtorch/share/cmake/Torch)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(neiro
  main.cpp
)

target_link_libraries(neiro PRIVATE ${TORCH_LIBRARIES} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
set_property(TARGET neiro PROPERTY CXX_STANDARD 17)
