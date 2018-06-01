add_library(Culvert INTERFACE)

add_library(LinkedCells INTERFACE)

add_library(CarvingEngine INTERFACE)

add_library(CarvingEngineCPU INTERFACE)
target_link_libraries(CarvingEngineCPU INTERFACE CarvingEngine
    LinkedCells geo system_utils coordinates)

add_library(CarvingAlgorithm INTERFACE)
target_link_libraries(CarvingAlgorithm INTERFACE
    AbstractAlgorithm CellGrid Culvert)

add_library(InsertCulvertAlgorithm InsertCulvertAlgorithm.cpp)
target_link_libraries(InsertCulvertAlgorithm
    AbstractAlgorithm CellGrid Culvert geometrics system_utils)
