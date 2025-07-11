file(GLOB TIMELINE_SRC src/timeline/*.cpp)
file(GLOB PREVIEW_SRC src/preview/*.cpp)
file(GLOB UTILS_SRC src/utils/*.cpp)
file(GLOB TEXT_SRC src/text/*.cpp)

add_executable(video_editor src/main.cpp ${TIMELINE_SRC} ${PREVIEW_SRC} ${UTILS_SRC} ${TEXT_SRC}) 