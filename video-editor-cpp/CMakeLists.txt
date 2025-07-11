cmake_minimum_required(VERSION 3.10)
project(VideoEditor)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MLT REQUIRED mlt++ mlt-framework)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB TIMELINE_SRC src/timeline/*.cpp)
file(GLOB PREVIEW_SRC src/preview/*.cpp)
file(GLOB UTILS_SRC src/utils/*.cpp)
file(GLOB TEXT_SRC src/text/*.cpp)

set(SOURCES
    src/main.cpp
    src/timeline/TimelineWidget.cpp
    src/timeline/TrackWidget.cpp
    src/timeline/ClipWidget.cpp
    src/VideoPreviewWidget.cpp
    src/utils/FileUtils.cpp
    src/text/TextOverlay.cpp
    src/text/TextOverlayManager.cpp
    src/text/TextEditDialog.cpp
)

add_executable(video_editor ${SOURCES})

target_include_directories(video_editor PRIVATE src src/timeline src/preview src/utils src/text ${MLT_INCLUDE_DIRS})
target_link_libraries(video_editor PRIVATE Qt5::Widgets Qt5::Gui Qt5::Core ${OpenCV_LIBS} ${MLT_LIBRARIES})
target_compile_options(video_editor PRIVATE ${MLT_CFLAGS_OTHER})

# TODO: Add libraries for video/audio processing (e.g., FFmpeg, OpenCV)
