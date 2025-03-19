cmake_minimum_required(VERSION 3.5)

project(Gridlink VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_library(AVCODEC_LIBRARY avcodec REQUIRED)
find_library(AVFORMAT_LIBRARY avformat REQUIRED)
find_library(AVUTIL_LIBRARY avutil REQUIRED)
find_library(SWSCALE_LIBRARY swscale REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        searchview.h searchview.cpp searchview.ui
        mpd_communication.h mpd_communication.cpp
        libraryview.h libraryview.cpp libraryview.ui
        artistview.h artistview.cpp artistview.ui
        albumview.h albumview.cpp albumview.ui
        datahandler.h datahandler.cpp
        config.h config.cpp
        song.h song.cpp
        album.h album.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Gridlink
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        songtablemodel.h
        helpers.h

        mpdstatus.h mpdstatus.cpp
        mpdnotif.h
        songcollection.h
        songcollection.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Gridlink APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Gridlink SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Gridlink
            ${PROJECT_SOURCES}
            searchview.h searchview.cpp searchview.ui
            mpd_communication.h mpd_communication.cpp
            libraryview.h libraryview.cpp libraryview.ui
            artistview.h artistview.cpp artistview.ui
            albumview.h albumview.cpp albumview.ui
            datahandler.h datahandler.cpp
            config.h config.cpp
            song.h song.cpp
            album.h album.cpp
        )
    endif()
endif()

target_link_libraries(Gridlink PRIVATE Qt${QT_VERSION_MAJOR}::Widgets mpdclient
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${SWSCALE_LIBRARY}
)
set_target_properties(Gridlink PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Gridlink
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Gridlink)
endif()
