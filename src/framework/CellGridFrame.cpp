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

#include "CellGridFrame.h"

#include <string>
#include <string.h> //memset
#include <assert.h>
#include <cstdlib>
#include <sstream>


using ct = coordinates::raster_coord_type;

CellGridFrame::CellGridFrame(
        const CellGridFrame & model,
        const std::string &name__):
            //pInfo(model.pInfo),
            area_ {model.area_},
            name_ {name__}
{
}

/**
 * Construct a cellGrid object with given arguments.
 */
CellGridFrame::CellGridFrame(
        const geo::RasterArea & area__,
        const std::string &name__):
            area_ {area__},
            name_ {name__}
{
}

CellGridFrame::~CellGridFrame()
{
}

/**
 * \brief Return the geocoordinate of the center of the pointer cell
 */
geo::PixelCenterCoordinate CellGridFrame::to_geocoordinate(
    const coordinates::RasterCoordinate &c) const
{
    return area_.to_geocoordinate(c);
}

size_t CellGridFrame::to_raster_index(
        const coordinates::RasterCoordinate & c) const
{
    return coordinates::to_raster_index(c, px_width());
}

ct CellGridFrame::px_width() const
{
    return area_.pixel_width();
}

ct CellGridFrame::px_height() const
{
    return area_.pixel_height();
}

size_t CellGridFrame::px_size() const
{
    return static_cast<size_t>(px_height()) *
        static_cast<size_t>(px_width());
}

geo::RasterArea CellGridFrame::area() const
{
    return area_;
}

std::string CellGridFrame::name() const
{
    return name_;
}
