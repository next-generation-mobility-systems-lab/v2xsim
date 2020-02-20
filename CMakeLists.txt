cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Find Gazebo
find_package(gazebo REQUIRED)
include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

# Build our plugin
add_library(veh_plugin SHARED veh_plugin.cc)
add_library(veh2_plugin SHARED veh2_plugin.cc)
add_library(veh3_plugin SHARED veh3_plugin.cc)
add_library(veh4_plugin SHARED veh4_plugin.cc)


target_link_libraries(veh_plugin ${GAZEBO_LIBRARIES})
target_link_libraries(veh2_plugin ${GAZEBO_LIBRARIES})
target_link_libraries(veh3_plugin ${GAZEBO_LIBRARIES})
target_link_libraries(veh4_plugin ${GAZEBO_LIBRARIES})

# Build the stand-alone test program
add_executable(vel vel.cc)

if (${gazebo_VERSION_MAJOR} LESS 6)
  # These two
  include(FindBoost)
  find_package(Boost ${MIN_BOOST_VERSION} REQUIRED system filesystem regex)
  target_link_libraries(vel ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})
else()
  target_link_libraries(vel ${GAZEBO_LIBRARIES})
endif()