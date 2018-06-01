add_library(data_source_types data_source_types.cpp)

# data input

add_library(DataSource DataSource.cpp)
target_link_libraries(DataSource
    PUBLIC data_source_types)

add_library(RasterDataSource RasterDataSource.cpp)
target_link_libraries(RasterDataSource
    PUBLIC DataSource CellGridFrame ext_gdal)

add_library(GDAL_dataset_ptr GDAL_dataset_ptr.cpp)
target_link_libraries(GDAL_dataset_ptr
    PUBLIC ext_gdal)

add_library(GDAL_help GDAL_help.cpp)
target_link_libraries(GDAL_help
    PUBLIC ext_gdal Short2
    PRIVATE RasterArea system_utils)

add_library(GDALRasterDataSource GDALRasterDataSource.cpp)
target_link_libraries(GDALRasterDataSource
    PUBLIC RasterDataSource ext_boost ext_gdal
    PRIVATE logging GDAL_dataset_ptr GDAL_help errors)

add_library(GDALRasterCollectionDataSource GDALRasterCollectionDataSource.cpp)
target_link_libraries(GDALRasterCollectionDataSource
    PUBLIC RasterDataSource)

add_library(import_data import_data.cpp)
target_link_libraries(import_data
    PUBLIC CellGrid RasterDataSource
        GDAL_help
    PRIVATE errors GDALRasterDataSource GDALRasterCollectionDataSource)


# data output

add_library(GDALRasterPrinter GDALRasterPrinter.cpp)
target_link_libraries(GDALRasterPrinter
    PUBLIC CellGrid GDAL_dataset_ptr ext_boost ext_gdal)

add_library(write_to_file write_to_file.cpp)
target_link_libraries(write_to_file
    PUBLIC GDALRasterPrinter ext_boost ext_gdal)
