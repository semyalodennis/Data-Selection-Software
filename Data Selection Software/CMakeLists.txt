cmake_minimum_required(VERSION 3.5)

project(Data_Selection_GUI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

#Find opencv
find_package(OpenCV CONFIG REQUIRED)

#Find Gdal
find_package(GDAL REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Resources.qrc
        GDALopencv.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Data_Selection_GUI
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Data_Selection_GUI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Data_Selection_GUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Custom property to disable console window on Windows
set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)

#Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE ${GDAL_INCLUDE_DIRS})

#Qt
target_link_libraries(Data_Selection_GUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

#Opencv
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

#Gdal
target_link_libraries(${PROJECT_NAME} PRIVATE ${GDAL_LIBRARIES})

