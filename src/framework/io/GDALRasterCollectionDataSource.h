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

#ifndef GDAL_RASTER_COLLECTION_DATA_SOURCE_H_
#define GDAL_RASTER_COLLECTION_DATA_SOURCE_H_

#include <memory>

#include "RasterDataSource.h"

namespace io {

    class GDALRasterCollectionDataSource: public RasterDataSource
    {
        public:
            GDALRasterCollectionDataSource();
            virtual ~GDALRasterCollectionDataSource();

            void fill_array(
                char *,
                CellGridFrame &,
                GDALDataType) const override;

            geo::ReferenceSystem CRS() const override;
            bool has_no_data_value() const override;
            double no_data_value() const override;
            std::string no_data_value_str() const override;

            GDALDataType data_type() const override;

            double cell_size() const override;
            geo::PixelTopLeftCoordinate ul_corner() const override;
            bool has_data_inside_area(const geo::Area &) const override;

            geo::Area area() const override;
            geo::RasterArea raster_area() const override;

            void add_data_source(RasterDataSource *);
            unsigned int data_set_count() const;

        private:
            std::vector<std::unique_ptr<RasterDataSource>> sources_;
            geo::RasterArea geo_area_;
    };

}

#endif
