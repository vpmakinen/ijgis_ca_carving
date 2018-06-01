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

#ifndef GDAL_RASTER_DATA_SOURCE_H_
#define GDAL_RASTER_DATA_SOURCE_H_

#include "RasterDataSource.h"

#include <boost/filesystem.hpp>
#include <gdal_priv.h>

namespace io {

    class GDALRasterDataSource: public RasterDataSource
    {
        public:
            virtual ~GDALRasterDataSource();

            GDALRasterDataSource(const boost::filesystem::path &);

            void fill_array(
                char * /*array*/,
                CellGridFrame & /*cg*/,
                GDALDataType /*value_type*/) const override;

            geo::ReferenceSystem CRS() const override;
            bool has_no_data_value() const override;
            double no_data_value() const override;
            std::string no_data_value_str() const override;

            GDALDataType data_type() const override;

            double cell_size() const override;
            geo::PixelTopLeftCoordinate ul_corner() const override;

            geo::Area area() const override;
            geo::RasterArea raster_area() const override;

            bool has_data_inside_area(const geo::Area &) const override;

            std::vector<std::string> file_list() const;

        private:
            std::string name_;
            double cell_size_;
            bool has_no_data_value_;
            std::string no_data_value_str_;
            double no_data_value_;
            geo::RasterArea geo_area_;
            std::vector<std::string> file_list_;
            GDALDataType data_type_;
    };

}

#endif
