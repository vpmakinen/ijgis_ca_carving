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

#include "Short2.h"


bool operator!= (const schar2 &lhs, const schar2 &rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

bool operator!= (const short2 &lhs, const short2 &rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

bool operator!= (const int2 &lhs, const int2 &rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

std::ostream & operator<<(std::ostream &os, const short2 & s)
{
    return os << s.x << "," << s.y;
}

std::ostream & operator<<(std::ostream &os, const int2 & s)
{
    return os << s.x << "," << s.y;
}

std::ostream & operator<<(std::ostream &os, const schar2 & s)
{
    return os << s.x << "," << s.y;
}
