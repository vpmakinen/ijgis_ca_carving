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

#ifndef DATA_SOURCE_TYPES_H_
#define DATA_SOURCE_TYPES_H_

#include <iostream>

namespace io {

    enum class DataSourceType {
        RASTER
    };

    std::ostream & operator<<(std::ostream &os, const DataSourceType & val);

}

#endif
