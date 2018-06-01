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

#include "DataSource.h"


namespace io {

    DataSource::DataSource(DataSourceType type):
        source_type_ {type}
    {
    }

    DataSource::~DataSource()
    {
    }

}
