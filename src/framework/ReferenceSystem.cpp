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

#include "ReferenceSystem.h"

#include <algorithm>

#include <gdal_priv.h>
#include <ogrsf_frmts.h>


namespace geo {

    ReferenceSystem::ReferenceSystem():
        wkt_str_ {"not_defined"}
    {
    }

    ReferenceSystem::ReferenceSystem(const std::string &s):
        wkt_str_ {s}
    {
        OGRSpatialReference ref {s.c_str()};
        if (ref.Validate() != OGRERR_NONE)
            wkt_str_ = "not_defined";
    }

    ReferenceSystem & ReferenceSystem::operator=(ReferenceSystem r)
    {
        std::swap(wkt_str_, r.wkt_str_);
        return *this;
    }

    bool operator==(const ReferenceSystem &a, const ReferenceSystem &b)
    {
        return a.wkt_str_ == b.wkt_str_;
    }

    bool operator!=(const ReferenceSystem &a, const ReferenceSystem &b)
    {
        return !(a == b);
    }

    std::string ReferenceSystem::string() const
    {
        return wkt_str_;
    }

}
