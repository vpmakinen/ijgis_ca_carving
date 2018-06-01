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

#ifndef COORDINATES_H_
#define COORDINATES_H_

#include <iostream>
#include <stdexcept>
#include <limits>
#include <cmath>


namespace coordinates {

    class CoordinateType { };
    class TypeRaster: public CoordinateType { };

    class TypePartition: public CoordinateType { };

    class TypeGlobalRaster: public CoordinateType { };

    using raster_coord_type = unsigned int;
    using raster_coord_diff_type = long int;
    using partition_coord_type = unsigned short;
    using partition_coord_diff_type = int;

    class RasterCoordinate;
    class RasterCoordinateDiff;
    class RasterDims;
    class GlobalDataCoords;

    template<typename T>
    typename std::enable_if<std::is_base_of<CoordinateType, typename T::coordinate_type>::value, bool>::type
    operator==(const T &lhs, const T &rhs)
    {
        return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_;
    }

    template<typename T>
    typename std::enable_if<std::is_base_of<CoordinateType, typename T::coordinate_type>::value, bool>::type
    operator!=(const T &lhs, const T &rhs)
    {
        return ! (lhs == rhs);
    }

    template<typename T>
    typename std::enable_if<std::is_base_of<CoordinateType, typename T::coordinate_type>::value, bool>::type
    operator<(const T &lhs, const T &rhs)
    {
        if (lhs.y_ == rhs.y_) return lhs.x_ < rhs.x_;
        return lhs.y_ < rhs.y_;
    }

    template<typename T>
    typename std::enable_if<std::is_base_of<CoordinateType, typename T::coordinate_type>::value, std::ostream &>::type
    operator<<(std::ostream &os, const T &c)
    {
        return os << "(" << c.x_ << ", " << c.y_ << ")";
    }

    class RasterCoordinate {
        public:
            using coordinate_type = TypeRaster;
            using datatype = raster_coord_type;

            friend bool operator==<RasterCoordinate>(const RasterCoordinate &, const RasterCoordinate &);
            friend bool operator< <RasterCoordinate>(const RasterCoordinate &, const RasterCoordinate &);
            friend std::ostream & operator<< <RasterCoordinate>(std::ostream &, const RasterCoordinate &);

            RasterCoordinate();
            RasterCoordinate(raster_coord_type, raster_coord_type);

            raster_coord_type col() const;
            raster_coord_type row() const;

        private:
            raster_coord_type x_;
            raster_coord_type y_;
    };

    inline size_t to_raster_index(size_t col, size_t row, size_t width)
    {
        return row * width + col;
    }

    inline size_t to_raster_index(const RasterCoordinate &c, size_t nx)
    {
        return to_raster_index(c.col(), c.row(), nx);
    }

    class RasterCoordinateDiff {
        public:
            using base_type = RasterCoordinate;

            RasterCoordinateDiff(
                raster_coord_diff_type,
                raster_coord_diff_type);

            raster_coord_diff_type norm_squared() const;
            double norm() const;

            raster_coord_diff_type dx() const;
            raster_coord_diff_type dy() const;
        private:
            raster_coord_diff_type x_;
            raster_coord_diff_type y_;
    };

    RasterCoordinateDiff get_neig_circular(int n);

    RasterCoordinateDiff operator-(const RasterCoordinate &, const RasterCoordinate &);

    class RasterDims {
        public:
            using datatype = raster_coord_type;

            RasterDims(raster_coord_type, raster_coord_type);

            raster_coord_type cols() const;
            raster_coord_type rows() const;

            static RasterDims create(size_t, size_t);

        private:
            raster_coord_type x_;
            raster_coord_type y_;
    };

    class GlobalDataCoords {
        public:
            using coordinate_type = TypeGlobalRaster;
            using datatype = raster_coord_type;

            friend bool operator==<GlobalDataCoords>(const GlobalDataCoords &, const GlobalDataCoords &);
            friend bool operator< <GlobalDataCoords>(const GlobalDataCoords &, const GlobalDataCoords &);
            friend std::ostream & operator<< <GlobalDataCoords>(std::ostream &, const GlobalDataCoords &);

            GlobalDataCoords(raster_coord_type, raster_coord_type);

            static GlobalDataCoords create(size_t, size_t);

            raster_coord_type col() const;
            raster_coord_type row() const;

        private:
            raster_coord_type x_;
            raster_coord_type y_;
    };

    template<typename C>
    typename C::base_type operator+(
        const typename C::base_type &p,
        const C &d);

    template<typename C>
    typename C::base_type operator-(
        const typename C::base_type &p,
        const C &d)
    {
        return p + C {-d.dx(), -d.dy()};
    }

    template<>
    RasterCoordinate operator+(
        const RasterCoordinate &,
        const RasterCoordinateDiff &);

    RasterCoordinate move_coord(
        const RasterCoordinate &,
        const RasterCoordinateDiff &,
        raster_coord_type nx,
        raster_coord_type ny);

    template<typename C>
    C neighbour_cyclic(
        const C & coord,
        int dir,
        const typename C::datatype & nx,
        const typename C::datatype & ny)
    {
        if (dir < 0 || dir > 8) return coord;

        int dx {-1 * (dir == 0 || dir == 3 || dir == 5) + (dir == 2 || dir == 4 || dir == 7)};
        int dy {-1 * (dir < 3) + (dir >= 5)};
        return coordinates::move_coord(coord, RasterCoordinateDiff {dx, dy}, nx, ny);
    }

}

#endif
