cmake_minimum_required(VERSION 3.21)
project(machine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    PrintSupport
    Charts
    Network
    Multimedia
    TextToSpeech
)

find_package(Qt6 QUIET COMPONENTS HttpServer)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(QRENCODE QUIET qrencode)
endif()

add_executable(machine
    main.cpp
    connexionmachine.cpp
    connexionmachine.h
    machine.cpp
    machine.h
    machine.ui
    resources.qrc
)

if(TARGET Qt6::HttpServer)
    target_sources(machine PRIVATE
        MachineServer.cpp
        MachineServer.h
    )
    target_compile_definitions(machine PRIVATE MACHINE_HAS_HTTPSERVER)
endif()

target_link_libraries(machine PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
    Qt6::Charts
    Qt6::Network
    Qt6::Multimedia
    Qt6::TextToSpeech
)

if(TARGET Qt6::HttpServer)
    target_link_libraries(machine PRIVATE Qt6::HttpServer)
endif()

# whisper.cpp integration (Windows):
# - Option A: set WHISPER_CPP_DIR to the whisper.cpp source folder (containing CMakeLists.txt)
# - Option B: if a whisper target already exists in your superbuild, it will be reused.
set(WHISPER_CPP_DIR "" CACHE PATH "Path to whisper.cpp source directory")

if(TARGET whisper)
    target_link_libraries(machine PRIVATE whisper)
    target_compile_definitions(machine PRIVATE MACHINE_ENABLE_WHISPER)
elseif(WHISPER_CPP_DIR)
    add_subdirectory(${WHISPER_CPP_DIR} ${CMAKE_BINARY_DIR}/whisper_cpp_build)
    if(TARGET whisper)
        target_link_libraries(machine PRIVATE whisper)
        target_compile_definitions(machine PRIVATE MACHINE_ENABLE_WHISPER)
    endif()
endif()

if(QRENCODE_FOUND)
    target_include_directories(machine PRIVATE ${QRENCODE_INCLUDE_DIRS})
    target_link_libraries(machine PRIVATE ${QRENCODE_LIBRARIES})
    target_compile_definitions(machine PRIVATE MACHINE_USE_QRENCODE)
else()
    find_library(QRENCODE_LIBRARY NAMES qrencode libqrencode)
    if(QRENCODE_LIBRARY)
        target_link_libraries(machine PRIVATE ${QRENCODE_LIBRARY})
        target_compile_definitions(machine PRIVATE MACHINE_USE_QRENCODE)
    else()
        message(STATUS "libqrencode introuvable: fallback QR HTTP active.")
    endif()
endif()
