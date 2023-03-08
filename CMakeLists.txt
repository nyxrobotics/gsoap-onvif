project(onvif_ptz_test)
cmake_minimum_required(VERSION 2.8)

list(
  APPEND
  CUDA_NVCC_FLAGS
  "-std=c++11"
)
set(CUDA_PROPAGATE_HOST_FLAGS OFF)
set(CMAKE_CXX_FLAGS "-std=c++0x")
find_package(OpenCV REQUIRED)
# If you cannot fint Curses, run ```sudo apt install libncurses5-dev```
find_package(Curses REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/factory/workloop.cmake)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include ${CURSES_INCLUDE_DIR})
file(
  GLOB
  INC
  include/*.h
  include/*.hpp
)
file(
  GLOB
  SRC
  src/*.c
  src/*.cc
  src/*.cpp
)

add_executable(onvif_ptz_test ${INC} ${SRC})

target_link_libraries(
  onvif_ptz_test
  wl
  ${OpenCV_LIBS}
  ${CURSES_LIBRARIES}
)
