cmake_minimum_required(VERSION 3.16)

project(ChatClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#message(${CMAKE_PREFIX_PATH})
#message(${CMAKE_MAKE_PROGRAM})
#message(${PATH})
list(APPEND CMAKE_PREFIX_PATH  D:/Programs/vcpkg/installed/x64-windows/ )

#set(Protobuf_DIR D:/Programs/vcpkg/installed/x64-windows/share/)
#set(Vulkan_INCLUDE_DIR D:/Programs/vcpkg/installed/x64-windows/)
#message(${Protobuf_DIR})
#message(${CMAKE_PREFIX_PATH})
#set(Protobuf_INCLUDE_DIR  "D:/Programs/vcpkg/installed/x64-windows/include")
#set(Protobuf_LIBRARY "D:/Programs/vcpkg/installed/x64-windows/debug/bin/libprotobufd.dll")




find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets NetWork WebSockets Protobuf)

#message(${PROTOBUF_INCLUDE_DIR})

file(GLOB PB_FILES "./proto/*.proto")
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PB_FILES})
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#message(111${Protobuf_SRC_ROOT_FOLDER})



set(MODEL_FILES
    model/data.h
    model/datacenter.h model/datacenter.cpp
)

set(NETWORK_FILES
    network/netclient.h network/netclient.cpp
)

set(PROJECT_SOURCES
        main.cpp
        mainwidget.cpp
        mainwidget.h
)
#message(${PROTO_SRCS})
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChatClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${MODEL_FILES}
        ${NETWORK_FILES}
        local_resource.qrc
        sessionfriendarea.h sessionfriendarea.cpp
        messageshowarea.h messageshowarea.cpp
        messageeditarea.h messageeditarea.cpp
        usrinfowidget.h usrinfowidget.cpp
        otherusrinfowidget.h otherusrinfowidget.cpp
        sessiondetailswidget.h sessiondetailswidget.cpp
        choosefrienddialog.h choosefrienddialog.cpp
        groupsessiondetailwidget.h groupsessiondetailwidget.cpp
        framelesswidget.h framelesswidget.cpp
        messagehistorywidget.h messagehistorywidget.cpp
        loginwidget.h loginwidget.cpp
        toast.h toast.cpp

    )
qt_add_protobuf(ChatClient PROTO_FILES ${PB_FILES})

FILE(GLOB QPB_FILES ${CMAKE_CURRENT_BINARY_DIR}/*.qpb.* )
FILE(COPY ${PROTO_SRCS} ${PROTO_HDIR} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/proto/qpb/)
message(${QPB_FILES})

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ChatClient APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ChatClient SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ChatClient
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ChatClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Network Qt6::WebSockets Qt6::Protobuf)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ChatClient)
endif()
set_target_properties(ChatClient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ChatClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChatClient)
endif()
