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

#ifndef GDAL_HELP_H_
#define GDAL_HELP_H_

#include <string>

#include <gdal_priv.h>

#include "Short2.h"
#include "DataSource.h"

namespace geo {
    class RasterArea;
}

namespace io {

    namespace GDAL {

        enum class RW_MODE {
            READ,
            WRITE
        };

        template<typename T>
        GDALDataType toGDALDataType();

        template<typename T>
        void array_file_rw(
            T * array,
            const geo::RasterArea & array_area,
            const geo::RasterArea & active_area,
            GDALRasterBand * band,
            const geo::RasterArea & file_raster_area,
            RW_MODE mode)
        {
            array_file_rw_(
                reinterpret_cast<char*>(array),
                array_area,
                active_area,
                toGDALDataType<T>(),
                band,
                file_raster_area,
                mode);
        }


        void array_file_rw_(
            char * array,
            const geo::RasterArea & array_area,
            const geo::RasterArea & active_area,
            GDALDataType value_type,
            GDALRasterBand *,
            const geo::RasterArea & file_raster_area,
            RW_MODE mode);

        template<typename T>
        GDALDataType toGDALDataType()
        {
            throw std::runtime_error("Unknown datatype");
        }

        template<> GDALDataType toGDALDataType<char>();
        template<> GDALDataType toGDALDataType<unsigned char>();
        template<> GDALDataType toGDALDataType<short>();
        template<> GDALDataType toGDALDataType<unsigned short>();
        template<> GDALDataType toGDALDataType<int>();
        template<> GDALDataType toGDALDataType<unsigned int>();
        template<> GDALDataType toGDALDataType<float>();

        template<typename T>
        T get_nodata(const io::DataSource & ds)
        {
            return static_cast<T>(ds.no_data_value());
        }

        template<> int2 get_nodata(const io::DataSource &);

    }

}

#endif
