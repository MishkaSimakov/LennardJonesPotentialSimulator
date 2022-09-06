cmake_minimum_required(VERSION 3.17)
project(PhysicsSimulation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Ofast)

include_directories(src)

add_executable(PhysicsSimulation src/main.cpp src/Drawers/WindowDrawer.h src/Atom.h src/World.h src/Loggers/FileLogger.h src/Helpers/progressbar.h src/Drawers/ImageDrawer.h src/Simulation.h src/Drawers/Drawer.h src/Loggers/Logger.h src/Loggers/TerminalLogger.h src/Helpers/LennardJones.h src/Helpers/InteractionInfo.h src/Helpers/WorldData.h src/Helpers/RungeKutta.h)

# include SFML
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML 2.5.1 REQUIRED system window graphics)

target_link_libraries(PhysicsSimulation ${SFML_LIBRARIES})

include_directories(${SFML_INCLUDE_DIR})

file(GLOB BINARY_DEP_DLLS "${SFML_INCLUDE_DIR}/../lib/*.dll")
file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})

# include OpenCL
#find_package(OpenCL REQUIRED)
#target_link_libraries(PhysicsSimulation OpenCL::OpenCL)

# Copy resources to build
file(COPY fonts images DESTINATION ${CMAKE_BINARY_DIR})
