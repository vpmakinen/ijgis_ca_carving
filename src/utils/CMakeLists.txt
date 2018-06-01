add_library(system_utils system_utils.cpp)
target_link_libraries(system_utils
    PUBLIC Short2 ext_boost
    PRIVATE logging)

add_library(Timer Timer.cpp)
target_link_libraries(Timer PRIVATE logging)

add_library(TimerTree TimerTree.cpp)
target_link_libraries(TimerTree
    PUBLIC Timer
    PRIVATE logging)

add_library(logging logging.cpp)
target_link_libraries(logging PRIVATE global_parameters)
set_target_properties(logging
    PROPERTIES COMPILE_FLAGS "-Wno-exit-time-destructors \
    -Wno-global-constructors")

add_library(geometrics INTERFACE)
