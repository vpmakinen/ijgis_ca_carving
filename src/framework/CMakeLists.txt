add_subdirectory(io)
add_subdirectory(program_options)

add_library(global_parameters global_parameters.cpp)

add_library(errors errors.cpp)
target_link_libraries(errors CmdError)

add_library(ReferenceSystem ReferenceSystem.cpp)
target_link_libraries(ReferenceSystem PRIVATE ext_gdal)

add_library(coordinates coordinates.cpp)

add_library(directions directions.cpp)
target_link_libraries(directions PRIVATE coordinates)

add_library(geo geo.cpp)

add_library(Area Area.cpp)
target_link_libraries(Area
    PUBLIC geo ReferenceSystem)

add_library(RasterArea RasterArea.cpp)
target_link_libraries(RasterArea
    PUBLIC Area coordinates
    PRIVATE logging)

add_library(Short2 Short2.cpp)

add_library(CellGridFrame CellGridFrame.cpp)
target_link_libraries(CellGridFrame
    PUBLIC directions coordinates RasterArea)

add_library(AbstractAlgorithm AbstractAlgorithm.cpp)
target_link_libraries(AbstractAlgorithm
    PUBLIC TimerTree)

add_library(CellGrid INTERFACE)
target_link_libraries(CellGrid INTERFACE
    CellGridFrame system_utils
    logging ext_gdal)
