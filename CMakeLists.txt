cmake_minimum_required(VERSION 3.27)
project(FinalProject)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(Resource)


file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE ALL_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Scene/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/UI/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Enemy/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Turret/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"   # 把根目錄（main.cpp）也包含
)
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/build/.*")

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

IF (WIN32)
    target_include_directories(FinalProject PUBLIC C:/allegro/include)
    target_link_directories(FinalProject PUBLIC C:/allegro/lib)
    target_compile_options(FinalProject PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(FinalProject liballegro_monolith)
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(FinalProject PUBLIC lib)
    target_include_directories(FinalProject PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(FinalProject PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(FinalProject ${ALLEGRO5_LIBRARIES})
    target_link_directories(FinalProject PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
ENDIF()