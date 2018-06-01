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

#ifndef RASTER_DATA_SOURCE_H_
#define RASTER_DATA_SOURCE_H_

#include <gdal.h>

#include "DataSource.h"
#include "CellGridFrame.h"

namespace io {

    class RasterDataSource: public DataSource {
        public:
            RasterDataSource();
            virtual ~RasterDataSource();

            virtual GDALDataType data_type() const = 0;
            virtual double cell_size() const = 0;
            virtual geo::PixelTopLeftCoordinate ul_corner() const = 0;
            virtual geo::RasterArea raster_area() const = 0;

            virtual void fill_array(
                char * array,
                CellGridFrame & cg,
                GDALDataType value_type) const = 0;
    };

    std::pair<bool, std::string> are_compatible(
        const RasterDataSource *,
        const RasterDataSource *);

}

#endif
