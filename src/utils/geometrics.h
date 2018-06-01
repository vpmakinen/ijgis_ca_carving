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

#ifndef GEOMETRICS_H_
#define GEOMETRICS_H_

#include <cmath>

/**
 * \brief Return a point that has been rotated w.r.t. the origin,
 * counterclockwise, by angle (in radians).
 */
template<typename C>
C rotate(const C &c, double angle)
{
    return {cos(angle) * c.x() - sin(angle) * c.y(),
            sin(angle) * c.x() + cos(angle) * c.y()};
}

/**
 * \brief Return the distance of the point p to the line defined by the points
 * a and b.
 */
template<typename C>
double distance_from_line(const C &a, const C &b, const C &p)
{
    int x1 {static_cast<int>(a.col())};
    int y1 {static_cast<int>(a.row())};
    int x2 {static_cast<int>(b.col())};
    int y2 {static_cast<int>(b.row())};
    int x0 {static_cast<int>(p.col())};
    int y0 {static_cast<int>(p.row())};

    return abs((y2 - y1) * x0 - (x2 - x1) * y0 + x2 * y1 - y2 * x1)
        / sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

template<typename C>
double distance_from_segment(const C &a, const C &b, const C &p)
{
    C ab {b.x() - a.x(), b.y() - a.y()};
    double angle {atan2(ab.y(), ab.x())};
    C ab_t {rotate(ab, -angle)};
    C p_t {rotate(C {p.x() - a.x(), p.y() - a.y()}, -angle)};
    if (p_t.x() < 0) {
        return distance(p, a);
    } else if (p_t.x() > ab_t.x()) {
        return distance(p, b);
    } else {
        return fabs(p_t.y());
    }
}

#endif
