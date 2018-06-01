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

#ifndef GEO_H_
#define GEO_H_

#include <iostream>
#include <cmath>


namespace geo {

    class TypeGeoCoordinate { };
    class TypePixelCenterGeoCoordinate: public TypeGeoCoordinate { };
    class TypePixelTopLeftCoordinate: public TypeGeoCoordinate { };

    class PixelCenterCoordinate;

    class GeoCoordinate
    {
        public:
            using coordinate_type = TypeGeoCoordinate;

            GeoCoordinate();
            GeoCoordinate(double, double);
            GeoCoordinate(const PixelCenterCoordinate &);

            double x() const;
            double y() const;

        private:
            double x_;
            double y_;
    };

    class PixelCenterCoordinate
    {
        public:
            using coordinate_type = TypePixelCenterGeoCoordinate;

            PixelCenterCoordinate(double, double);

            double x() const;
            double y() const;

        private:
            double x_;
            double y_;
    };

    class PixelTopLeftCoordinate
    {
        public:
            using coordinate_type = TypePixelTopLeftCoordinate;

            PixelTopLeftCoordinate(double, double);

            double x() const;
            double y() const;

        private:
            double x_;
            double y_;
    };

    class GeoDims {
        public:
            GeoDims(double, double);

            double width() const;
            double height() const;

        private:
            double x_;
            double y_;
    };

    template<typename T>
    typename std::enable_if<std::is_base_of<TypeGeoCoordinate, typename T::coordinate_type>::value, bool>::type
    operator!=(const T & a, const T & b)
    {
        return 1e-5 < fabs(a.x() - b.x()) || 1e-5 < fabs(a.y() - b.y());
    }

    template<typename T>
    typename std::enable_if<std::is_base_of<TypeGeoCoordinate, typename T::coordinate_type>::value, bool>::type
    operator==(const T & a, const T & b)
    {
        return !(a != b);
    }

    template<typename T>
    typename std::enable_if<std::is_base_of<TypeGeoCoordinate, typename T::coordinate_type>::value, std::ostream &>::type
    operator<<(std::ostream &os, const T & c)
    {
        return os << "(" << std::fixed << c.x() << ", " << std::fixed << c.y() << ")";
    }

    template<typename T, typename U>
    typename std::enable_if<
        std::is_base_of<TypeGeoCoordinate, typename T::coordinate_type>::value &&
        std::is_base_of<TypeGeoCoordinate, typename U::coordinate_type>::value, double>::type
    distance(const T & a, const U & b)
    {
        return std::sqrt(pow(a.x() - b.x(), 2) + pow(a.y() - b.y(), 2));
    }

    template<typename T>
    typename std::enable_if<std::is_base_of<TypeGeoCoordinate, typename T::coordinate_type>::value, GeoCoordinate>::type
    average(const T &a, const T &b)
    {
        return {(a.x() +  b.x()) / 2, (a.y() + b.y()) / 2};
    }

}

#endif
