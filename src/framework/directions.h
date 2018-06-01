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

#ifndef DIRECTIONS_H_
#define DIRECTIONS_H_

#include <vector>
#include <set>
#include <iostream>

namespace directions {

    enum class Direction {
        TOP_LEFT = 0,
        TOP = 1,
        TOP_RIGHT = 2,
        LEFT = 3,
        RIGHT = 4,
        BOTTOM_LEFT = 5,
        BOTTOM = 6,
        BOTTOM_RIGHT = 7
    };

    Direction get_direction(int);

    std::ostream & operator<<(std::ostream &, const Direction &);

}

#endif
