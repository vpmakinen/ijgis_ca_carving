add_library(CarvingDefs INTERFACE)
target_link_libraries(CarvingDefs INTERFACE
    FlowRoutingAlgorithm CarvingAlgorithm)

add_library(CarvingCmdOpts ProgramCmdOpts.cpp)
target_link_libraries(CarvingCmdOpts
    BaseCmdOpts)

add_library(InsertCulvertsRoadStreamInters
    insert_culverts_to_road_stream_intersections.cpp)
target_link_libraries(InsertCulvertsRoadStreamInters CarvingDefs)

add_library(InsertCulvertsExpensiveCarvings
    insert_culverts_to_expensive_carvings.cpp)
target_link_libraries(InsertCulvertsExpensiveCarvings CarvingDefs)

add_library(algorithm_carving program.cpp)
target_link_libraries(algorithm_carving
    PRIVATE CarvingDefs
    CarvingCmdOpts
    InsertCulvertsRoadStreamInters
    InsertCulvertsExpensiveCarvings
    import_data
    FlowRoutingAlgorithm
    FlowAccumulationAlgorithm
    InsertCulvertAlgorithm
    write_to_file)

add_executable(carving.bin main.cpp)
target_link_libraries(carving.bin
    CarvingCmdOpts
    algorithm_carving
    ext_boost)
