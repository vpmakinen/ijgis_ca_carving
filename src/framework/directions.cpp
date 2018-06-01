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

#include <stdexcept>
#include <numeric>

#include "directions.h"

namespace directions {

    Direction get_direction(int i)
    {
        switch (i) {
            case 0: return Direction::TOP_LEFT;
            case 1: return Direction::TOP;
            case 2: return Direction::TOP_RIGHT;
            case 3: return Direction::LEFT;
            case 4: return Direction::RIGHT;
            case 5: return Direction::BOTTOM_LEFT;
            case 6: return Direction::BOTTOM;
            case 7: return Direction::BOTTOM_RIGHT;
            default: throw std::runtime_error("Invalid direction index.");
        }
    }

    std::ostream & operator<<(std::ostream &o, const Direction &d)
    {
        switch (d) {
            case Direction::TOP_LEFT: return o << "TOP_LEFT";
            case Direction::TOP: return o << "TOP";
            case Direction::TOP_RIGHT: return o << "TOP_RIGHT";
            case Direction::LEFT: return o << "LEFT";
            case Direction::RIGHT: return o << "RIGHT";
            case Direction::BOTTOM_LEFT: return o << "BOTTOM_LEFT";
            case Direction::BOTTOM: return o << "BOTTOM";
            case Direction::BOTTOM_RIGHT: return o << "BOTTOM_RIGHT";
            default: return o << "UNKNOWN_DIRECTION";
        }
    }

}
