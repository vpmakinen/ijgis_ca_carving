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

#include "GDALRasterPrinter.h"

namespace io {

    namespace GDAL {

        dataset_ptr create_data_file(
            const boost::filesystem::path & file,
            const char * gdal_driver,
            const GDALDataType & data_type,
            int n_layers,
            const geo::RasterArea & area)
        {
            GDALDriver * poDriver = GetGDALDriverManager()->GetDriverByName(gdal_driver);
            if (!poDriver) {
                throw std::runtime_error("Failed to create the GDAL driver.");
            }
            dataset_ptr ds;
            ds.reset(
                poDriver->Create(file.string().c_str(),
                    static_cast<int>(area.pixel_width()),
                    static_cast<int>(area.pixel_height()),
                    n_layers,
                    data_type,
                    nullptr));

            double geo_transform[6];
            geo_transform[0] = area.left();
            geo_transform[3] = area.top();
            geo_transform[1] = area.cell_size();
            geo_transform[5] = -area.cell_size();
            geo_transform[2] = 0;
            geo_transform[4] = 0;
            ds->SetGeoTransform(geo_transform);

            ds->SetProjection(area.CRS().string().c_str());
            return ds;
        }

    }

}
