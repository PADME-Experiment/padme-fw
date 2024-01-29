cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project (EE_MUMU_COMBINATION C CXX)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(CMAKE_CXX_FLAGS_INIT "-D_GNU_SOURCE -fPIC -pthread -pipe -ansi") #${CMAKE_CXX_FLAGS}


set(ACCEPTED_BUILD_TYPES DEBUG RELEASE DEBUG)
set(CMAKE_BUILD_TYPE RELEASE)
#if (NOT CMAKE_BUILD_TYPE)
  #message(STATUS "Setting build type to 'Release' as none was specified")
  #set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
#endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_BUILD_TYPE)
list(FIND ACCEPTED_BUILD_TYPES ${UPPER_BUILD_TYPE} IS_BUILD_TYPE_ACCEPTED)
if(${IS_BUILD_TYPE_ACCEPTED} EQUAL -1)
        message(FATAL_ERROR "CMAKE_BUILD_TYPE of type ${CMAKE_BUILD_TYPE} is not accepted. Available values are Debug,Release,RelWithDebInfo,MinSizeRel")
else()
        message(STATUS "Compilation type: " ${CMAKE_BUILD_TYPE})
endif()


#Get and configure ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
if( ${ROOT_FOUND} )
    include(${ROOT_USE_FILE})
    message (STATUS "Found ROOT ${ROOT_VERSION} at ${ROOT_DIR}")
  else()
    message (FATAL_ERROR "ROOT not found")
endif()

# Use correct gcc libraries
#link_directories($ENV{COMPILER_PATH}/lib64)
#link_directories($ENV{COMPILER_PATH}/lib)
#link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_96/ROOT/6.18.00/x86_64-centos7-gcc8-opt/lib)
link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_98/ROOT/v6.22.00/x86_64-centos7-gcc8-opt/lib)
link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_96/GSL/2.5/x86_64-centos7-gcc8-opt/lib)
link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_96/vdt/0.4.3/x86_64-centos7-gcc8-opt/lib)
link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_96/Davix/0.7.3/x86_64-centos7-gcc8-opt/lib64)
link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_96/tbb/2019_U7/x86_64-centos7-gcc8-opt/lib)
link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_94/glib/2.52.2/x86_64-centos7-gcc8-opt/lib)
link_directories(/cvmfs/sft.cern.ch/lcg/releases/LCG_96/png/1.6.37/x86_64-centos7-gcc8-opt/lib)
#link_directories(/lib64)


#Define install directories
if("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
    #Overwrite the default
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
endif()

#Get sources and headers
file(GLOB sources src/*.cxx)
file(GLOB headers include/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(MainObjLib OBJECT ${sources})
set_property(TARGET ${MainObjLib} PROPERTY POSITION_INDEPENDENT_CODE 1)


#Generate ROOT dictionary
ROOT_GENERATE_DICTIONARY(PersistencyDICT ${headers} OPTIONS -inlineInputHeader -Iinclude LINKDEF LinkDef.h MODULE Persistency)# -noIncludePaths)

add_library(Persistency SHARED PersistencyDICT.cxx)
target_link_libraries(Persistency ${ROOT_LIBRARIES})

#Install libraries and headers
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libPersistency_rdict.pcm DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libPersistency.rootmap DESTINATION lib)
install(TARGETS Persistency EXPORT PersistencyTargets DESTINATION lib)

set(list_executables roofit_full)

#Fake libs
add_library(srcObjLib OBJECT ${sources})
set_property(TARGET ${srcObjLib} PROPERTY POSITION_INDEPENDENT_CODE 1)

#Then the real libraries
add_library(truesrc SHARED $<TARGET_OBJECTS:srcObjLib>)
add_library(truesrc-static STATIC $<TARGET_OBJECTS:srcObjLib>)

foreach(exec ${list_executables})
    add_executable(${exec} ${exec}.cxx $<TARGET_OBJECTS:MainObjLib>)
    target_link_libraries(${exec} ${ROOT_LIBRARIES} Persistency truesrc-static RooFit RooFitCore) #rimosso truesrc
    install(TARGETS ${exec} DESTINATION bin)
endforeach()

