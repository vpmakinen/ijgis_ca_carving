include_directories(utils)
include_directories(utils/assembledata)
include_directories(utils/initializearray)
include_directories(utils/arrayinspector)
include_directories(framework)
include_directories(framework/io)
include_directories(framework/program_options)

add_subdirectory(framework)
add_subdirectory(utils)

include_directories(flowrouting)
include_directories(carving)
include_directories(flow_accumulation)

add_subdirectory(flowrouting)
add_subdirectory(carving)
add_subdirectory(flow_accumulation)

add_subdirectory(program_carving)
