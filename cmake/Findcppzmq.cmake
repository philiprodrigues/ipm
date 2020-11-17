# JCF, Oct-19-2020
# This is a snippet Eric created, posted in a Slack PM Sep-23-2020
#
# Modified by P. Rodrigues, 2020-11-17, based on the guidance in
# https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

if(EXISTS $ENV{ZMQ_LIB})
  # UPS
  message("Finding CPPZMQ from UPS $ENV{ZMQ_INC}")
  find_path(cppzmq_INCLUDE_DIR
    NAMES zmq.hpp
    PATHS $ENV{ZMQ_INC}
    )
  message("Found zmq.hpp in ${cppzmq_INCLUDE_DIR}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(cppzmq
    FOUND_VAR cppzmq_FOUND
    REQUIRED_VARS
    cppzmq_INCLUDE_DIR
    )
  
  if(cppzmq_FOUND AND NOT TARGET cppzmq::cppzmq)
    # cppzmq is header-only, so we use an INTERFACE "library", as
    # described in
    # https://cmake.org/cmake/help/latest/command/add_library.html#interface-libraries
    add_library(cppzmq::cppzmq INTERFACE IMPORTED)
    set_target_properties(cppzmq::cppzmq PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${cppzmq_INCLUDE_DIR}"
      )
  endif()
  
else()
  # Spack
  find_package(cppzmq REQUIRED CONFIG)
endif()

