project( licenseclient )
cmake_minimum_required( VERSION 2.8 )

# set default build type
if(NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
	set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall")
endif()

include( CheckCXXCompilerFlag )
check_cxx_compiler_flag( "-std=c++11"   COMPILER_SUPPORTS_CXX11 )
if( COMPILER_SUPPORTS_CXX11 )
    if( CMAKE_COMPILER_IS_GNUCXX )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11" )
    else()
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
    endif()
endif()

#########################

file(GLOB_RECURSE HEADER_FILES *.h *.hpp)
source_group("Include" FILES ${HEADER_FILES})

aux_source_directory(. DIR_SRCS)
include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${PROJECT_SOURCE_DIR}/../../include)
link_directories(${PROJECT_SOURCE_DIR}/../../lib)
add_library( ${PROJECT_NAME} SHARED ${DIR_SRCS} ${HEADER_FILES})


if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions(-DROCK_LINUX_PLATFORM -DBOOST_LOG_DYN_LINK)
	
	target_link_libraries(${PROJECT_NAME}
		pthread
		boost_system
		boost_filesystem
		boost_log
		loghelper
		json_message_processor
		tcpclient
	)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_definitions( /DROCK_HANDLER_EXPORTS )
	target_link_libraries(${PROJECT_NAME}
		loghelper
		json_message_processor
		tcpclient
	)

endif()

########################

INSTALL(TARGETS ${PROJECT_NAME} 
	RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/../../bin
	LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/../../lib
    ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/../../lib
)