cmake_minimum_required(VERSION 3.16)

project(SmartOilFinance VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS 
    Widgets 
    Sql 
    Charts 
    Core 
    Gui 
    LinguistTools
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS 
    Widgets 
    Sql 
    Charts 
    Core 
    Gui 
    LinguistTools
)

set(TS_FILES SmartOilFinance_fr_FR.ts)

# Fichiers sources du projet
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    database.cpp
    database.h
    transaction.cpp
    transaction.h
    businesslogic.cpp
    businesslogic.h
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SmartOilFinance
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SmartOilFinance APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(SmartOilFinance SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SmartOilFinance
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(SmartOilFinance PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Sql 
    Qt${QT_VERSION_MAJOR}::Charts
)

# Qt for iOS sets some variables when executing cmake, so we have to do
# some conditionals to ensure that publishing can still be done on Linux
# and macOS.
if(QT_VERSION_MAJOR EQUAL 6)
    if(NOT ANDROID)
        set_target_properties(SmartOilFinance PROPERTIES
            WIN32_EXECUTABLE ON
            MACOSX_BUNDLE ON
        )
    endif()
    qt_finalize_executable(SmartOilFinance)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(SmartOilFinance PRIVATE -Wall -Wextra -Wpedantic)
endif()
