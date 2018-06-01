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

#ifndef GDAL_RASTER_PRINTER_H_
#define GDAL_RASTER_PRINTER_H_

#include <gdal_priv.h>
#include <boost/filesystem.hpp>

#include "RasterArea.h"
#include "global_parameters.h"
#include "GDAL_dataset_ptr.h"
#include "GDAL_help.h"
#include "logging.h"

namespace io {

    namespace GDAL {

        dataset_ptr create_data_file(
            const boost::filesystem::path & file,
            const char * gdal_driver,
            const GDALDataType & data_type,
            int n_layers,
            const geo::RasterArea & area);

        template<typename CG>
        void write(
            CG & cg,
            const boost::filesystem::path &file,
            const std::string & format,
            const geo::Area &area_to_write)
        {
            if (global_parameters::dryRun) return;
            geo::RasterArea area_to_write_ {
                area_to_write,
                cg.area().ul_corner(),
                cg.area().cell_size()};

            int file_created {0};
            dataset_ptr ds;
            if (!file_created) {
                auto ext = file.extension().string();
                ds = GDAL::create_data_file(
                    file, format.c_str(), cg.data_type(), 1,
                    area_to_write_);
                file_created = true;
            } else {
                ds.reset(static_cast<GDALDataset*>(GDALOpenEx(
                    file.string().c_str(),
                    GDAL_OF_RASTER | GDAL_OF_UPDATE,
                    nullptr,
                    nullptr,
                    nullptr)));
            }

            GDALRasterBand * band = ds->GetRasterBand(1);
            if (cg.has_no_data_value()) {
                if (band->SetNoDataValue(static_cast<double>(
                    cg.no_data_value())) == CE_Failure) {
                    logging::pErr() << "Failed to set nodata_value for the created data set.";
                }
            }
            GDAL::array_file_rw(
                cg.data(),
                cg.area(),
                cg.area(),
                band,
                area_to_write_,
                GDAL::RW_MODE::WRITE);
        }

    }

}

#endif
