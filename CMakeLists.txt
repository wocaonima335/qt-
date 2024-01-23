cmake_minimum_required(VERSION 3.5)

project(TcpClient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
FILE(GLOB_RECURSE FORMS "*.ui")
FILE(GLOB_RECURSE RESOURCES "*.qrc")
FILE(GLOB_RECURSE HEADERS "*.h")
FILE(GLOB_RECURSE SOURCES "*cpp")

SOURCE_GROUP("Form Files" FILES ${FORMS})
SOURCE_GROUP("Resource Files" FILES ${RESOURCES})
SOURCE_GROUP("Head Files" FILES ${HEADERS})
SOURCE_GROUP("Source Files" FILES ${SOURCES})

if(ANDROID)
  add_library(TcpClient SHARED
      ${FORMS}
      ${RESOURCES}
      ${HEADERS}
      ${SOURCES}
  )
else()
  add_executable(TcpClient
      ${FORMS}
      ${RESOURCES}
      ${HEADERS}
      ${SOURCES}
  )
endif()

target_link_libraries(TcpClient PRIVATE Qt5::Widgets)
target_link_libraries(TcpClient PRIVATE Qt5::Network)
target_link_libraries(TcpClient PRIVATE Qt5::Sql)
