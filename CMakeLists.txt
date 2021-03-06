# CMake file for generating all project files
# Newer version required for VS_DEBUGGER_WORKING_DIRECTORY
if(WIN32)
  cmake_minimum_required(VERSION 3.10)
else()
  cmake_minimum_required(VERSION 3.6)
endif()

project(Leviathan)

# version number
set(LEVIATHAN_VERSION_STABLE 0)
set(LEVIATHAN_VERSION_MAJOR 8)
set(LEVIATHAN_VERSION_MINOR 0)
set(LEVIATHAN_VERSION_PATCH 0)

set(LEVIATHAN_VERSION_STR ${LEVIATHAN_VERSION_STABLE}.${LEVIATHAN_VERSION_MAJOR}.${LEVIATHAN_VERSION_MINOR}.${LEVIATHAN_VERSION_PATCH})
set(LEVIATHAN_VERSION ${LEVIATHAN_VERSION_STABLE}.${LEVIATHAN_VERSION_MAJOR}${LEVIATHAN_VERSION_MINOR}${LEVIATHAN_VERSION_PATCH})

set(WORK_DIR "${PROJECT_BINARY_DIR}/bin")
file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR} PROJECT_NATIVE_BINARY_DIR)

# Leviathan base folder which is required for many folders to be correctly found
set(LEVIATHAN_SRC "${CMAKE_CURRENT_LIST_DIR}")

# modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMake" "${PROJECT_SOURCE_DIR}/LeviathanCMake")

# load cotire
include(cotire)

# Load utility files
include(LeviathanUtility)

# Just in case some unity builds are broken
#set_property(DIRECTORY PROPERTY COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY")

######### Options
    
option(CREATE_UE4_PLUGIN "If set to ON creates a static version without any external libraries" OFF)

set(LEVIATHAN_FULL_BUILD NOT CREATE_UE4_PLUGIN)

option(CREATE_STATIC "Set to ON to create static libraries instead of shared" OFF)

option(CREATE_SHIPPING "SET THIS ON WHEN BUILDING END USER BINARIES. Will disable some expensive checks" OFF)
    
# \todo this option to copy right libs
option(CREATE_SDK "For creating SDKs for developers" ON)

# Copies boost libraries to distributions
option(COPY_BOOST_TO_PACKAGE "For copying all boost libraries to run on system that don't have it" OFF)

# Whether to use Javascript or php search in doxygen
option(DOCUMENTATION_LOCAL "Set to OFF to create web documentation" ON)

# Strip symbols from install
option(STRIP_SYMBOLS_ON_INSTALL "For stripping debug symbols on install" OFF)

option(BUILD_SAMPLES "Set to OFF to disable building demo programs" ON)

# Check is leap motion used
option(USE_LEAP "Set to on when using Leap motion, will auto disable if files are not copied" ON)

# Option for only building the documentation (for lighter jenkins)
option(ONLY_DOCUMENTATION "If true only documentation can be built correctly" OFF)

option(MAKE_RELEASE "Enabled breakpad crash reporting and some game specific options" OFF)

option(LEVIATHAN_USE_ACTUAL_OBJECT_POOLS
  "Set to false to use new/delete instead of object pools. Provided for debugging memory issues with valgrind" ON)

# Build configuration

if(ONLY_DOCUMENTATION)

  set(MAKE_RELEASE OFF)
  
endif()

if(LEVIATHAN_FULL_BUILD)
  set(USE_ANGELSCRIPT ON)
  set(USE_BOOST ON)
  set(USE_OGRE ON)
  set(USE_NEWTON ON)
  set(USE_SFML ON)
  set(USE_SDL2 ON)

  set(CMAKE_INSTALL_PREFIX "./Install")

  if(CREATE_SDK)

    set(CMAKE_INSTALL_PREFIX "../SDK")
  
  endif()
else()

  message(STATUS "Creating UE4 Compatible Static Library")
  set(USE_ANGELSCRIPT OFF)
  set(USE_BOOST OFF)
  set(USE_OGRE OFF)
  set(USE_NEWTON OFF)
  set(COPY_BOOST_TO_PACKAGE OFF)
  set(CREATE_STATIC ON)
  set(MAKE_RELEASE OFF)
  set(USE_SFML OFF)
  set(USE_SDL2 OFF)
  
  set(CMAKE_INSTALL_PREFIX "./UE4_Plugin")
  
endif()



# Check that leap motion can be enabled
if(EXISTS "${LEVIATHAN_SRC}/Leap/include/Leap.h")

  message(STATUS "Leap motion includes found")
  set(CAN_USE_LEAP ON)

  
else()

  if(NOT CREATE_UE4_PLUGIN)
    message(STATUS
      "Leap motion files aren't installed in leviathan/Leap, Leap motion support is disabled")
  endif()
  
  set(CAN_USE_LEAP OFF)
  
endif()

if(USE_LEAP AND CAN_USE_LEAP)
  set(USING_LEAP ON)
else()
  set(USING_LEAP OFF)
endif()


include(LeviathanCompileOptions)
include(LeviathanSetRPath)

if(NOT ONLY_DOCUMENTATION)
  # Find required libraries
  include(LeviathanFindLibraries)
endif()

# Configure engine include file
configure_file("${LEVIATHAN_SRC}/Engine/Include.h.in" "${PROJECT_BINARY_DIR}/Include.h")
# Make a readonly copy of it
file(COPY "${PROJECT_BINARY_DIR}/Include.h" DESTINATION "${LEVIATHAN_SRC}/Engine"
  NO_SOURCE_PERMISSIONS
  FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)




# configure the doxygen file
find_package(Doxygen)
if(DOXYGEN_FOUND)

  if(DOCUMENTATION_LOCAL)
    set(DOXYGEN_SERVER_SEARCH "FALSE")
  else()
    set(DOXYGEN_SERVER_SEARCH "TRUE")
  endif()
  
  configure_file("${PROJECT_SOURCE_DIR}/LeviathanDoxy.in"
    "${PROJECT_BINARY_DIR}/LeviathanDoxy" @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/LeviathanDoxy
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

else()
  if(ONLY_DOCUMENTATION)
    message(SEND_ERROR "Documentation only build is missing doxygen")
  endif()
endif()

if(NOT ONLY_DOCUMENTATION)
  include(LeviathanDefaultFileCopy)

  # Controls how LeviathanCoreProject.cmake works
  set(CREATE_CONSOLE_APP OFF)

  # First project must be the file generator
  add_subdirectory(FileGenerator)
  
  # engine project 
  add_subdirectory(Engine)
  
  # test project
  add_subdirectory(LeviathanTest)
  
  if(NOT CREATE_UE4_PLUGIN)

    # Standalone editor
    add_subdirectory(Editor)

    if(BUILD_SAMPLES)
      # example pong master server
      add_subdirectory(PongMasterServer)
      
      # example pong server
      add_subdirectory(PongServer)

      # pong example game
      add_subdirectory(Pong)

      # demo examples
      add_subdirectory(Demos)
    endif()
  endif()

  # Set the tests as the default startup project
  set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY
    VS_STARTUP_PROJECT LeviathanTest)
endif()

# .desktop files
if(UNIX)

  # These need to be generated by an install script and copied to the
  # system folder to actually work at all
  
  # add_custom_target(Test_desktopfile "./CreateLinuxIcon.rb"
  #   "${CMAKE_BINARY_DIR}/bin/LeviathanTest" "--name" "LeviathanTesting application" "-t"
  #   DEPENDS LeviathanTest WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/" VERBATIM)

  # add_custom_target(Pong_desktopfile "./CreateLinuxIcon.rb"
  #   "${CMAKE_BINARY_DIR}/bin/Pong" "${CMAKE_SOURCE_DIR}/Pong/ICON1.ico"
  #   "--name" "Pong client application" "--prefix" "Pong"
  #   DEPENDS Pong WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/" VERBATIM)

  # add_custom_target(PongServer_desktopfile "./CreateLinuxIcon.rb"
  #   "${CMAKE_BINARY_DIR}/bin/PongServer" "${CMAKE_SOURCE_DIR}/PongServer/ICON1.ico"
  #   "--name" "Pong server application" "--prefix" "PongServer"
  #   DEPENDS PongServer WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/" VERBATIM)

  #   add_custom_target(PongMasterServer_desktopfile "./CreateLinuxIcon.rb"
  #   "${CMAKE_BINARY_DIR}/bin/PongMasterServer" "${CMAKE_SOURCE_DIR}/PongMasterServer/ICON1.ico"
  #   "--name" "Pong masterserver application" "--prefix" "PongMaster"
  #   DEPENDS PongMasterServer WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/" VERBATIM)
  
  # add_custom_target(All_desktopfile ALL DEPENDS Test_desktopfile Pong_desktopfile
  #   PongServer_desktopfile PongMasterServer_desktopfile)

  # install(FILES "${CMAKE_BINARY_DIR}/bin/LeviathanTest.desktop" DESTINATION bin)
  
endif()

