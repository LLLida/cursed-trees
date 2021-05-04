cmake_minimum_required(VERSION 3.15)

project(cursed-trees VERSION 0.1)
set(default_build_type "Release")

# Compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -Wextra -Wpedantic")

# ncurses library for rendering
set(CURSES_USE_NCURSES TRUE)
find_package(Curses REQUIRED)

# library for ECS
add_subdirectory(entt)

# fmtlib for formatting
add_subdirectory(fmt)

# library for random with intuitive API
set(Random_BuildTests OFF)
add_subdirectory(random)

# json for serialization
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON)
add_subdirectory(nlohmann_json-shrinked)

# improve std::list performance
add_subdirectory(Memory-Pool)

# command line arguments parsing
set(ARGS_BUILD_EXAMPLE OFF)
set(ARGS_BUILD_UNITTESTS OFF)
add_subdirectory(args)

if (NOT WIN32)
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
# option(BUILD_EMACS_MODULE ON)
set(BUILD_EMACS_MODULE ON)
if (BUILD_EMACS_MODULE)
  add_subdirectory(emacs-module.hpp)

  set(MODULE_NAME ${PROJECT_NAME}-module)
  # add_emacs_module(${MODULE_NAME}
  # 	"src/Emacs/Module.cpp" "src/Emacs/Functions.cpp"
  # 	"src/Game/Genetic.cpp" "src/Game/Tree.cpp" "src/Game/World.cpp")
  add_library(${MODULE_NAME} SHARED
	"src/Emacs/Module.cpp" "src/Emacs/Functions.cpp"
	"src/Game/Genetic.cpp" "src/Game/Tree.cpp" "src/Game/World.cpp")
  set_target_properties(${MODULE_NAME} PROPERTIES
	PREFIX "")
  target_link_libraries(${MODULE_NAME} PRIVATE emacs-module-base)
  target_link_libraries(${MODULE_NAME} PRIVATE
	fmt::fmt
	EnTT::EnTT
	effolkronium_random
	lida::Memory-Pool)
endif (BUILD_EMACS_MODULE)
