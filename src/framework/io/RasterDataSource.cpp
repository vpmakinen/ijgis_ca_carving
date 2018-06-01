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

#include "RasterDataSource.h"

#include <ogrsf_frmts.h>


namespace io {

    RasterDataSource::RasterDataSource():
        DataSource {DataSourceType::RASTER}
    {
    }

    RasterDataSource::~RasterDataSource()
    {
    }

    std::pair<bool, std::string> are_compatible(
        const RasterDataSource * src_a,
        const RasterDataSource * src_b)
    {
        const OGRSpatialReference sra {src_a->CRS().string().c_str()};
        const OGRSpatialReference srb {src_b->CRS().string().c_str()};
        if (!sra.IsSame(&srb)) {
            return {false, "Incompatible projections."};
        }
        if (!src_a->raster_area().is_aligned_with(src_b->raster_area()))
        {
            return {false, "Rasters are not aligned."};
        }
        return {true, {}};
    }

}
