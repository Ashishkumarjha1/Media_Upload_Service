# Create the ModuleInterface shared library from ModuleInterface.cpp and ServiceHandler.cpp.
add_library(ModuleInterface SHARED
    ModuleInterface.cpp
    src/transactionLogs.cpp
    src/media_upload_service.cpp
)
#LOGGER
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=format -DNDEBUG -O3 -g")
# Set path to NanoLog library
set(NANOLOG_RUNTIME_DIR ${CMAKE_SOURCE_DIR}/Logger/NanoLog/runtime)
# Include NanoLog directory
include_directories(${NANOLOG_RUNTIME_DIR})

#include logger directory
include_directories(${CMAKE_SOURCE_DIR}/Logger/logger)



# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories for json from the ExternalLibraries directory
target_include_directories(ModuleInterface PRIVATE
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/Json/json/include
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/dmal/inc
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/dmal/ExternalLibZMQ/include
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/paho/include
    ${CMAKE_SOURCE_DIR}/ServiceDiscovery/interface
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/ExternalLibAvahi/include/avahi
    ${CMAKE_SOURCE_DIR}/module/inc
    ${CMAKE_SOURCE_DIR}/module/ExternalLibSQL/mysql-connector-c++-8.2.0-linux-glibc2.28-x86-64bit/include/jdbc

)

target_link_libraries(ModuleInterface PRIVATE
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/ExternalLibAvahi/lib/libavahi-client.so
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/ExternalLibAvahi/lib/libavahi-common.so
    ${CMAKE_SOURCE_DIR}/ServiceDiscovery/lib/libServiceDiscovery.so
    ${CMAKE_SOURCE_DIR}/module/ExternalLibSQL/mysql-connector-c++-8.2.0-linux-glibc2.28-x86-64bit/lib64/libmysqlcppconn.so
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/paho/lib/libpaho-mqtt3a.so
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/paho/lib/libpaho-mqtt3as.so
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/paho/lib/libpaho-mqtt3c.so
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/paho/lib/libpaho-mqttpp3.so
    #${CMAKE_SOURCE_DIR}/ExternalLibraries/JSONcpp/libjsoncpp.so
    #${CMAKE_SOURCE_DIR}/ExternalLibraries/dmal/JSONcpp/libjsoncpp.so
    ${CMAKE_SOURCE_DIR}/ExternalLibraries/libtransactionLogs.so
    ${OpenCV_LIBS}
    #LOGGER
    ${NANOLOG_RUNTIME_DIR}/libNanoLog.a 
    rt
    pthread
)

target_include_directories(ModuleInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(ModuleInterface PROPERTIES
    OUTPUT_NAME "ModuleInterface"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
