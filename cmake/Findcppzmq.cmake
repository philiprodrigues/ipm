# JCF, Oct-19-2020
# This is a snippet Eric created, posted in a Slack PM Sep-23-2020

if(EXISTS $ENV{ZMQ_LIB})
 # UPS
 message("Finding CPPZMQ from UPS $ENV{ZMQ_LIB}")
  include_directories($ENV{ZMQ_INC})
  find_library(ZMQ NAMES libzmq.so HINTS $ENV{ZMQ_LIB})
  message("find_library ZMQ found ${ZMQ}")
  set(cppzmq_FOUND TRUE)
else()
	# Spack
	find_package(cppzmq REQUIRED)
	set(ZMQ zmq)
endif()