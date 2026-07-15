# Author: Pavlo Nykolyn
cmake_minimum_required(VERSION 3.22.1)
# some environments may not include compilers for languages other than C. Besides, given that I wrote the program in C, it makes sense to force it
project(mikroRouteAPI
        LANGUAGES C)
# HEADER file directories
set(HEADER_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADER_SUBLOCATION_UTILITIES ${HEADER_LOCATION}/utilities)
set(HEADER_SUBLOCATION_CLI ${HEADER_LOCATION}/cli)
set(HEADER_SUBLOCATION_COMMUNICATION ${HEADER_LOCATION}/communication)
set(HEADER_SUBLOCATION_API ${HEADER_LOCATION}/api)
# source file directories
set(SRC_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_SUBLOCATION_UTILITIES ${SRC_LOCATION}/utilities)
set(SRC_SUBLOCATION_CLI ${SRC_LOCATION}/cli)
set(SRC_SUBLOCATION_COMMUNICATION ${SRC_LOCATION}/communication)
set(SRC_SUBLOCATION_API ${SRC_LOCATION}/api)
if(IS_DIRECTORY ${HEADER_LOCATION} AND IS_DIRECTORY ${SRC_LOCATION})
   add_executable(mikroClient)
   target_include_directories(mikroClient
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(mikroClient
                  PRIVATE mikroClient.c
                  PRIVATE ${HEADER_SUBLOCATION_API}/mikro_word.h
                  PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}/phy_socketInterface.h
                  PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}/sys_wrapperSocketHead.h
                  PRIVATE ${HEADER_SUBLOCATION_CLI}/mikro_cli_parser.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/byteUtilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/header_wrapper.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/iterators.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/generic.h)
   add_library(mikro_string OBJECT)
   target_include_directories(mikro_string
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(mikro_string
                  PRIVATE ${SRC_SUBLOCATION_UTILITIES}/mikro_string.c
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/generic.h)
   add_library(mikro_cli_parser OBJECT)
   target_include_directories(mikro_cli_parser
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(mikro_cli_parser
                  PRIVATE ${SRC_SUBLOCATION_CLI}/mikro_cli_parser.c
                  PRIVATE ${HEADER_SUBLOCATION_CLI}/mikro_cli_parser.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/generic.h)
   add_library(mikro_word OBJECT)
   target_include_directories(mikro_word
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(mikro_word
                  PRIVATE ${SRC_SUBLOCATION_API}/mikro_word.c
                  PRIVATE ${HEADER_SUBLOCATION_API}/mikro_word.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/byteUtilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/iterators.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/generic.h)
   add_library(err_system OBJECT)
   target_include_directories(err_system
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(err_system
                  PRIVATE ${SRC_SUBLOCATION_UTILITIES}/err_system.c
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/err_system.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/sys_wrapperErrHead.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/header_wrapper.h)
   add_library(phy_socket OBJECT)
   target_include_directories(phy_socket
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(phy_socket
                  PRIVATE ${SRC_SUBLOCATION_COMMUNICATION}/phy_socket.c
                  PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}/phy_socketInterface.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}/sys_wrapperSocketHead.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/err_system.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/generic.h)
   add_library(byteUtilities OBJECT)
   target_include_directories(byteUtilities
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(byteUtilities
                  PRIVATE ${SRC_SUBLOCATION_UTILITIES}/byteUtilities.c
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/byteUtilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/byteDefs.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/iterators.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/header_wrapper.h
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/generic.h)
   add_library(mikro_messages OBJECT)
   target_include_directories(mikro_messages
                              PRIVATE ${HEADER_SUBLOCATION_UTILITIES}
                              PRIVATE ${HEADER_SUBLOCATION_CLI}
                              PRIVATE ${HEADER_SUBLOCATION_COMMUNICATION}
                              PRIVATE ${HEADER_SUBLOCATION_API})
   target_sources(mikro_messages
                  PRIVATE ${SRC_SUBLOCATION_UTILITIES}/mikro_messages.c
                  PRIVATE ${HEADER_SUBLOCATION_UTILITIES}/mikro_messages.h)
   target_link_libraries(mikroClient
                         PRIVATE mikro_string
                         PRIVATE mikro_cli_parser
                         PRIVATE mikro_word
                         PRIVATE phy_socket
                         PRIVATE byteUtilities
                         PRIVATE err_system
                         PRIVATE mikro_messages)
else()
   message(FATAL_ERROR "${HEADER_LOCATION} is not a valid directory")
endif()
   
