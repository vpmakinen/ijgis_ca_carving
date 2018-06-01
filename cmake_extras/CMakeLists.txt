# tests during the configuration

# GDAL
try_run(GDAL_RUN_RESULT GDAL_COMPILE_SUCCESSFUL
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_gdal.cpp
    COMPILE_DEFINITIONS -isystem${GDAL_INCLUDE_DIRS} -std=c++11 -DCMAKE_TRY_RUN
    LINK_LIBRARIES ${GDAL_LIBRARY}
    COMPILE_OUTPUT_VARIABLE GDAL_COMP_OUT
    RUN_OUTPUT_VARIABLE GDAL_RUN_OUT
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/test_gdal_data.shp"
)
string(STRIP "${GDAL_RUN_OUT}" GDAL_ROUT)
if ("${GDAL_COMPILE_SUCCESSFUL}" STREQUAL "FALSE")
    message(FATAL_ERROR "GDAL test program compilation failed.")
elseif ("${GDAL_RUN_RESULT}" STREQUAL "FAILED_TO_RUN")
    message(FATAL_ERROR "GDAL test program failed to run.")
elseif (NOT "${GDAL_ROUT}" STREQUAL "SUCCESS")
    message(FATAL_ERROR "GDAL test program does not work properly.")
else()
    message(STATUS "GDAL found and working properly.")
endif()

add_executable(test_gdal test_gdal.cpp)
target_link_libraries(test_gdal ${GDAL_LIBRARY})
target_include_directories(test_gdal SYSTEM PRIVATE ${GDAL_INCLUDE_DIRS})

add_custom_command(
    OUTPUT test_gdal.log
    COMMAND test_gdal
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/test_gdal_data.shp)

add_custom_target(run_test_gdal ALL
    DEPENDS test_gdal.log)
