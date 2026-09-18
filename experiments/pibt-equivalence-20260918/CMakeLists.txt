cmake_minimum_required(VERSION 3.16)
project(pibt_equivalence LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
find_package(Boost REQUIRED)
set(CGAR ${CMAKE_CURRENT_SOURCE_DIR}/../../cgar)
# Preserve the vendored upstream; expose one private method only in the build copy.
set(UPSTREAM ${CMAKE_CURRENT_BINARY_DIR}/upstream)
file(COPY vendor/pibt2/ DESTINATION ${UPSTREAM})
file(READ ${UPSTREAM}/pibt2/include/pibt.hpp PIBT_HEADER)
string(REPLACE "private:" "private:\n  friend class PibtStepHarness;" PIBT_HEADER "${PIBT_HEADER}")
file(WRITE ${UPSTREAM}/pibt2/include/pibt.hpp "${PIBT_HEADER}")
set(GRAPH ${UPSTREAM}/third_party/grid-pathfinding/graph)
add_library(reference STATIC
    ${UPSTREAM}/pibt2/src/pibt.cpp ${UPSTREAM}/pibt2/src/solver.cpp
    ${UPSTREAM}/pibt2/src/problem.cpp ${UPSTREAM}/pibt2/src/plan.cpp
    ${UPSTREAM}/pibt2/src/paths.cpp
    ${GRAPH}/src/graph.cpp ${GRAPH}/src/node.cpp ${GRAPH}/src/pos.cpp)
target_include_directories(reference PUBLIC ${UPSTREAM}/pibt2/include ${GRAPH}/include)
add_executable(pibt_reference upstream_driver.cpp)
target_include_directories(pibt_reference PRIVATE ${CGAR}/inc)
target_link_libraries(pibt_reference PRIVATE reference)
add_executable(cgar_reference cgar_driver.cpp ${CGAR}/cgar_planner/cgar.cpp)
target_include_directories(cgar_reference PRIVATE ${CGAR}/inc ${CGAR}/cgar_planner ${Boost_INCLUDE_DIRS})
