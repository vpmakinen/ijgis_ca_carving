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

#ifndef GEO_AREA_H_
#define GEO_AREA_H_

#include "geo.h"

#include <vector>

#include "ReferenceSystem.h"

namespace geo {

    class Area;

    bool are_overlapping(const Area &, const Area &);

    /**
     * \brief Area determined by georeferences coordinates.
     *
     *       A---------------+      A = (left, top)
     *       |               |      B = (right, bottom)
     *       |               |
     *       |               |
     *       |               |
     *    y  |               |
     *    ^  |               |
     *    |  |               |
     *    |  +---------------B
     *    +----> x
     */
    class Area
    {
        friend bool are_overlapping(const Area &, const Area &);
        friend Area create_intersection(const Area &, const Area &);
        friend Area minimum_cover(const Area &, const Area &);

        public:
            Area();
            Area(
                const GeoCoordinate & topleft,
                const GeoDims & dims,
                const ReferenceSystem &);

            Area(const Area &) = default;

            Area & operator=(const Area &) = default;

            virtual ~Area() = default;

            double left() const;
            double right() const;
            double top() const;
            double bottom() const;
            GeoCoordinate top_left() const;
            ReferenceSystem CRS() const;
            bool contains_point(const geo::GeoCoordinate &) const;

            void CRS(const ReferenceSystem &);

            std::vector<std::string> to_vector_str() const;

            void add_halo(double width);

        protected:
            GeoCoordinate top_left_;
            GeoDims dims_;
            ReferenceSystem ref_system_;

            virtual double min_accepted_overlap() const;
    };

    std::ostream & operator<<(std::ostream &, const Area &);

    Area create_intersection(const Area &a1, const Area &a2);
    Area minimum_cover(const Area & a1, const Area & a2);
}

#endif
