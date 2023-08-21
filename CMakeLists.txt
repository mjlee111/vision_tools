cmake_minimum_required(VERSION 3.5)

project(vision_tools LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(OpenCV_INCLUDE_DIRS
  /usr/local/include/opencv4
  /usr/local/include/opencv4/opencv2
)

set(OpenCV_LIB_DIR
  /usr/local/lib
)

set(OpenCV_LIBS
  opencv_core
  opencv_highgui
  opencv_imgcodecs
  opencv_videoio
  opencv_imgproc
  opencv_features2d
  opencv_flann
  opencv_calib3d
)

include_directories(${catkin_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})
set(UI_FILES
    ui/vision.ui
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES})

add_executable(vision_tools
  src/main.cpp
  src/vision.cpp
  include/vision.h
    ${UI_HEADERS}
)



target_link_libraries(vision_tools PRIVATE Qt5::Widgets ${OpenCV_LIBS})
