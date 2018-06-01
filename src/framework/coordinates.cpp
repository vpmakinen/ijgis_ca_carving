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

#include "coordinates.h"

#include <stdexcept>

namespace coordinates {

    RasterCoordinate::RasterCoordinate():
        x_ {0}, y_ {0}
    {
    }

    RasterCoordinate::RasterCoordinate(
            raster_coord_type x,
            raster_coord_type y):
        x_ {x}, y_ {y}
    {
    }

    raster_coord_type RasterCoordinate::col() const
    {
        return x_;
    }

    raster_coord_type RasterCoordinate::row() const
    {
        return y_;
    }

    GlobalDataCoords::GlobalDataCoords(
            raster_coord_type x,
            raster_coord_type y):
        x_ {x}, y_ {y}
    {
    }

    raster_coord_type GlobalDataCoords::col() const
    {
        return x_;
    }

    raster_coord_type GlobalDataCoords::row() const
    {
        return y_;
    }

    GlobalDataCoords GlobalDataCoords::create(size_t x, size_t y)
    {
        if (x > std::numeric_limits<raster_coord_type>::max()) {
            throw std::runtime_error("GlobalDataCoords col too large.");
        }
        if (y > std::numeric_limits<raster_coord_type>::max()) {
            throw std::runtime_error("GlobalDataCoords row too large.");
        }
        return {static_cast<raster_coord_type>(x),
                static_cast<raster_coord_type>(y)};
    }


    raster_coord_diff_type RasterCoordinateDiff::dx() const
    {
        return x_;
    }

    raster_coord_diff_type RasterCoordinateDiff::dy() const
    {
        return y_;
    }

    RasterCoordinateDiff::RasterCoordinateDiff(
            raster_coord_diff_type x,
            raster_coord_diff_type y):
        x_ {x}, y_ {y}
    {
    }

    RasterCoordinateDiff get_neig_circular(int n) {
        switch(n) {
            case 0: return {-1, -1};
            case 1: return { 0, -1};
            case 2: return { 1, -1};
            case 3: return {-1,  0};
            case 4: return { 1,  0};
            case 5: return {-1,  1};
            case 6: return { 0,  1};
            case 7: return { 1,  1};
            default: throw std::runtime_error("Impossible error.");
        }
    }

    RasterCoordinate move_coord(
        const RasterCoordinate &c,
        const RasterCoordinateDiff &d,
        raster_coord_type nx,
        raster_coord_type ny)
    {
        if ((d.dx() < 0 && std::abs(d.dx()) > c.col()) ||
            static_cast<long>(c.col()) + d.dx() >= nx ||
            (d.dy() < 0 && std::abs(d.dy()) > c.row()) ||
            static_cast<long>(c.row()) + d.dy() >= ny)
            return c;
        return {
            static_cast<raster_coord_type>(c.col() + d.dx()),
            static_cast<raster_coord_type>(c.row() + d.dy()) };
    }

    RasterDims::RasterDims(raster_coord_type x, raster_coord_type y):
        x_ {x}, y_ {y}
    {
    }

    raster_coord_type RasterDims::cols() const
    {
        return x_;
    }

    raster_coord_type RasterDims::rows() const
    {
        return y_;
    }

    RasterDims RasterDims::create(size_t x, size_t y)
    {
        if (x > std::numeric_limits<raster_coord_type>::max()) {
            throw std::runtime_error("RasterDims cols too large.");
        }
        if (y > std::numeric_limits<raster_coord_type>::max()) {
            throw std::runtime_error("RasterDims rows too large.");
        }
        return {static_cast<raster_coord_type>(x),
                static_cast<raster_coord_type>(y)};
    }

    template<>
    RasterCoordinate operator+(
        const RasterCoordinate & c,
        const RasterCoordinateDiff &d)
    {
        if ((d.dx() < 0 && static_cast<size_t>(-d.dx()) > c.col()) ||
            (d.dy() < 0 && static_cast<size_t>(-d.dy()) > c.row()))
            throw std::runtime_error("RasterCoordinate + RasterCoordinateDiff returned negative coordinates.");
        size_t col {static_cast<size_t>(c.col() + d.dx())};
        size_t row {static_cast<size_t>(c.row() + d.dy())};
        if (col > std::numeric_limits<raster_coord_type>::max() ||
            row > std::numeric_limits<raster_coord_type>::max())
        {
            throw std::runtime_error("RasterCoordinate + RasterCoordinateDiff returned too large values.");
        }
        return RasterCoordinate {
            static_cast<raster_coord_type>(col),
            static_cast<raster_coord_type>(row)};
    }

    RasterCoordinateDiff operator-(const RasterCoordinate &a, const RasterCoordinate &b)
    {
        return {
            static_cast<raster_coord_diff_type>(a.col()) - static_cast<raster_coord_diff_type>(b.col()),
            static_cast<raster_coord_diff_type>(a.row()) - static_cast<raster_coord_diff_type>(b.row())};
    }

    // FIXME may overflow
    raster_coord_diff_type RasterCoordinateDiff::norm_squared() const
    {
        return x_ * x_ + y_ * y_;
    }

    double RasterCoordinateDiff::norm() const
    {
        return sqrt(static_cast<double>(norm_squared()));
    }
}
