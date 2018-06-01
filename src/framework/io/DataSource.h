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

#ifndef DATA_SOURCE_H_
#define DATA_SOURCE_H_

#include <string>

#include "data_source_types.h"

namespace geo {

    class Area;
    class RasterArea;
    class ReferenceSystem;

}

namespace io {

    class DataSource
    {
        public:
            DataSource(DataSourceType type);
            virtual ~DataSource();

            DataSourceType source_type() const;

            virtual geo::ReferenceSystem CRS() const = 0;
            virtual bool has_no_data_value() const = 0;
            virtual double no_data_value() const = 0;
            virtual std::string no_data_value_str() const = 0;
            virtual bool has_data_inside_area(const geo::Area &) const = 0;

            virtual geo::Area area() const = 0;

        private:
            DataSourceType source_type_;
    };

}

#endif
