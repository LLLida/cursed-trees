cmake_minimum_required(VERSION 3.15)

project(cursed-trees VERSION 0.1)
set(default_build_type "Release")

# Compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -Wextra -Wpedantic")

# library for ECS
add_subdirectory(entt)

# library for random with intuitive API
set(Random_BuildTests OFF)
add_subdirectory(random)

# improve std::list performance
add_subdirectory(Memory-Pool)

# MS Windows hasn't ncurses so don't build the executable
if (NOT WIN32)
  # ncurses library for rendering
  set(CURSES_USE_NCURSES TRUE)
  find_package(Curses REQUIRED)

  # fmtlib for formatting
  add_subdirectory(fmt)

  # json for serialization
  set(JSON_BuildTests OFF CACHE INTERNAL "")
  set(JSON_MultipleHeaders ON)
  add_subdirectory(nlohmann_json-shrinked)

  # command line arguments parsing
  set(ARGS_BUILD_EXAMPLE OFF)
  set(ARGS_BUILD_UNITTESTS OFF)
  add_subdirectory(args)

  add_executable(${PROJECT_NAME} "src/main.cpp"
    "src/Graphics/Curses.cpp" "src/Graphics/Widgets.cpp"
    "src/Game/Genetic.cpp" "src/Game/Tree.cpp" "src/Game/World.cpp" "src/Game/Serialization.cpp")
  target_include_directories(${PROJECT_NAME} PRIVATE ${CURSES_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} PRIVATE 
    fmt::fmt 
    EnTT::EnTT 
    effolkronium_random 
    nlohmann_json::nlohmann_json 
    lida::Memory-Pool
    args
    ${CURSES_LIBRARIES})
endif (NOT WIN32)

# Emacs module
find_program(HAS_EMACS emacs)
if (${HAS_EMACS} STREQUAL "HAS_EMACS-NOTFOUND")
  option(BUILD_EMACS_MODULE "Build Emacs module" OFF)
else ()
  option(BUILD_EMACS_MODULE "Build Emacs module" ON)
endif ()
if (${BUILD_EMACS_MODULE})
  add_subdirectory(emacs-module.hpp)
  set(MODULE_NAME ${PROJECT_NAME}-module)
  add_library(${MODULE_NAME} SHARED
    "src/Emacs/Module.cpp" "src/Emacs/Functions.cpp"
    "src/Game/Genetic.cpp" "src/Game/Tree.cpp" "src/Game/World.cpp")
  mark_emacs_module(${MODULE_NAME})
  target_link_libraries(${MODULE_NAME} PRIVATE
	EnTT::EnTT
	effolkronium_random
	lida::Memory-Pool)
endif ()
