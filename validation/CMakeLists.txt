# Author: Pavlo Nykolyn
cmake_minimum_required(VERSION 3.22.1)
# some environments may not include compilers for languages other than C. Besides, given that I wrote the program in C, it makes sense to force it
project(mikroValidator
        LANGUAGES C)
# header file directories
set(HEADER_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADER_PARENT_LOCATION_UTILITIES ${CMAKE_CURRENT_SOURCE_DIR}/../include/utilities)
set(HEADER_PARENT_LOCATION_API ${CMAKE_CURRENT_SOURCE_DIR}/../include/api)
# source file directories
set(SRC_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_PARENT_LOCATION_UTILITIES ${CMAKE_CURRENT_SOURCE_DIR}/../src/utilities)
set(SRC_PARENT_LOCATION_API ${CMAKE_CURRENT_SOURCE_DIR}/../src/api)
if(IS_DIRECTORY ${HEADER_LOCATION} AND IS_DIRECTORY ${SRC_LOCATION})
   add_executable(mikroValidator)
   target_include_directories(mikroValidator
                              PRIVATE ${HEADER_LOCATION}
                              PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}
                              PRIVATE ${HEADER_PARENT_LOCATION_API})
   target_sources(mikroValidator
                  PRIVATE validate.c
                  PRIVATE ${HEADER_LOCATION}/mikro_validation.h
                  PRIVATE ${HEADER_PARENT_LOCATION_API}/mikro_word.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/byteUtilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/header_wrapper.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/iterators.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/generic.h)
   add_library(mikro_string OBJECT)
   target_include_directories(mikro_string
                              PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}
                              PRIVATE ${HEADER_PARENT_LOCATION_API})
   target_sources(mikro_string
                  PRIVATE ${SRC_PARENT_LOCATION_UTILITIES}/mikro_string.c
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/generic.h)
   add_library(mikro_word OBJECT)
   target_include_directories(mikro_word
                              PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}
                              PRIVATE ${HEADER_PARENT_LOCATION_API})
   target_sources(mikro_word
                  PRIVATE ${SRC_PARENT_LOCATION_API}/mikro_word.c
                  PRIVATE ${HEADER_PARENT_LOCATION_API}/mikro_word.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/byteUtilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/iterators.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/generic.h)
   add_library(byteUtilities OBJECT)
   target_include_directories(byteUtilities
                              PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}
                              PRIVATE ${HEADER_PARENT_LOCATION_API})
   target_sources(byteUtilities
                  PRIVATE ${SRC_PARENT_LOCATION_UTILITIES}/byteUtilities.c
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/byteUtilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/byteDefs.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/iterators.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/header_wrapper.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/generic.h)
   add_library(mikro_messages OBJECT)
   target_include_directories(mikro_messages
                              PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}
                              PRIVATE ${HEADER_PARENT_LOCATION_API})
   target_sources(mikro_messages
                  PRIVATE ${SRC_PARENT_LOCATION_UTILITIES}/mikro_messages.c
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_messages.h)
   add_library(mikro_validation OBJECT)
   target_include_directories(mikro_validation
                              PRIVATE ${HEADER_LOCATION}
                              PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}
                              PRIVATE ${HEADER_PARENT_LOCATION_API})
   target_sources(mikro_validation
                  PRIVATE ${SRC_LOCATION}/mikro_validation.c
                  PRIVATE ${HEADER_LOCATION}/mikro_validation.h
                  PRIVATE ${HEADER_PARENT_LOCATION_API}/mikro_word.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_string.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/byteUtilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_wrapper_utilities.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_logging.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_errors.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/mikro_messages.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/iterators.h
                  PRIVATE ${HEADER_PARENT_LOCATION_UTILITIES}/generic.h)
   target_link_libraries(mikroValidator
                         PRIVATE mikro_validation
                         PRIVATE mikro_string
                         PRIVATE mikro_word
                         PRIVATE byteUtilities
                         PRIVATE mikro_messages)
else()
   message(FATAL_ERROR "either ${HEADER_LOCATION} or ${SRC_LOCATION} is not a valid directory")
endif()