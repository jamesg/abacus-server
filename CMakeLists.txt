cmake_minimum_required(VERSION 2.6)

project("Abacus Servers")
enable_testing()

find_package(CURL)
find_package(Threads)
find_package(OpenSSL)
find_package(Boost COMPONENTS date_time filesystem random regex system thread)
find_package(ImageMagick COMPONENTS Magick++)

include_directories("atlas/lib/mongoose/include")
include_directories("atlas/include")
include_directories("commandline/include")
include_directories("hades/include")
include_directories("hades/lib/sqlite3/include")
include_directories("src")
include_directories("apollo/include")
include_directories("chronos/include")
include_directories("demeter/include")
include_directories("helios/include")
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

add_definitions("-std=c++11")

# Hades components
add_subdirectory(hades)

# Atlas components
add_subdirectory(atlas)

# Apollo components
add_subdirectory(apollo)

# Chronos components
add_subdirectory(chronos)

# Demeter components
add_subdirectory(demeter)

# Helios components
add_subdirectory(helios)

add_subdirectory(web)

# Abacus components
add_subdirectory(src)

add_executable(abacus src/main/abacus.cpp)
add_dependencies(abacus-server-lib abacus-web)
target_link_libraries(
    abacus
    abacus-server-lib
    )
