cmake_minimum_required(VERSION 3.12)
project(ipm VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

find_package(appfwk REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(cppzmq REQUIRED)
find_package(ers REQUIRED)
find_package(nlohmann_json REQUIRED)

##############################################################################
daq_point_build_to( include )  


##############################################################################
daq_point_build_to( src )

add_library(IPM src/Direct.cpp src/Publisher.cpp src/Subscriber.cpp src/ZmqSender.cpp src/ZmqReceiver.cpp)
target_link_libraries(IPM appfwk::appfwk ${ZMQ})

##############################################################################
daq_point_build_to( test )

add_library(ipm_VectorIntIPMSender_duneDAQModule test/VectorIntIPMSenderDAQModule.cpp)
target_link_libraries(ipm_VectorIntIPMSender_duneDAQModule IPM)

add_library(ipm_VectorIntIPMReceiver_duneDAQModule test/VectorIntIPMReceiverDAQModule.cpp)
target_link_libraries(ipm_VectorIntIPMReceiver_duneDAQModule IPM)

file(COPY test/singleProcess.json DESTINATION test)
file(COPY test/simpleZmqSender.json DESTINATION test)
file(COPY test/simpleZmqReceiver.json DESTINATION test)

##############################################################################
daq_point_build_to( unittest )

daq_add_unit_test(ipmSender_test IPM )
daq_add_unit_test(ipmReceiver_test IPM )
daq_add_unit_test(Direct_test    IPM )
daq_add_unit_test(Publisher_test    IPM )
daq_add_unit_test(Subscriber_test    IPM )
daq_add_unit_test(ZmqReceiver_test IPM)
daq_add_unit_test(ZmqSender_test IPM)

daq_install(TARGETS IPM)
