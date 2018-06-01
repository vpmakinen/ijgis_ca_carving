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

#include "data_source_types.h"

#include <stdexcept>

namespace io {

    std::ostream & operator<<(std::ostream &os, const DataSourceType & val)
    {
        switch (val) {
            case DataSourceType::RASTER:
                return os << "RASTER";
            default:
                throw std::runtime_error("Encountered unknown data source type.");
        }
    }

}
