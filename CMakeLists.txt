cmake_minimum_required(VERSION 3.16)
project(HabitLog VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

set(LIB_SOURCES
    src/models/Habit.cpp
    src/services/HabitManager.cpp
    src/services/Statistics.cpp
    src/storage/JsonStorage.cpp
    src/ui/TerminalUI.cpp
)
add_executable(habitlog src/main.cpp ${LIB_SOURCES})
target_include_directories(habitlog PRIVATE include third_party)
target_compile_options(habitlog PRIVATE -Wall -Wextra -O2)

add_executable(test_habits tests/test_habits.cpp
    src/models/Habit.cpp
    src/services/HabitManager.cpp
    src/services/Statistics.cpp
    src/storage/JsonStorage.cpp
)
target_include_directories(test_habits PRIVATE include third_party)
enable_testing()
add_test(NAME HabitTests COMMAND test_habits)
