/**
 * Copyright
 *   2015-     Finnish Geospatial Research Institute,
 *             National Land Survey of Finland
 *
 * Programmers: Ville Mäkinen
 *
 * This file is released under the GNU Lesser General Public
 * Licence version 2.1.
 */

#ifndef IMPORT_DATA_H_
#define IMPORT_DATA_H_

#include <memory>

#include <boost/filesystem.hpp>
#include <gdal_priv.h>

#include "CellGrid.h"
#include "RasterDataSource.h"
#include "GDAL_help.h"


namespace io {

    std::unique_ptr<RasterDataSource> create_raster_data_source(
        const boost::filesystem::path & path);

    template<typename T, typename C>
    bool fill_array(
        CellGrid<T, C> & cg,
        const DataSource & ds)
    {
        switch (ds.source_type()) {
            case DataSourceType::RASTER: {
                auto & rds = dynamic_cast<const RasterDataSource &>(ds);
                if (rds.data_type() != cg.data_type()) {
                    throw std::runtime_error("The data types of the "
                        "CellGrid and the data source are not "
                        "compatible.");
                }
                auto & source = dynamic_cast<const RasterDataSource &>(rds);
                cg.pre_data_input(source);
                source.fill_array(
                    reinterpret_cast<char*>(cg.data()),
                    cg,
                    GDAL::toGDALDataType<T>());
                return true; }
            default:
                throw std::runtime_error("Impossible error.");
        }
    }

}

#endif
