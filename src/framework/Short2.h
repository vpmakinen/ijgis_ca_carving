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

#ifndef SHORT2_H_
#define SHORT2_H_

#include <iostream>

class short2 {
    public:
        using datatype = short;
        short x;
        short y;
};

class uint2 {
    public:
        using datatype = unsigned int;
        unsigned int x;
        unsigned int y;
};

class int2 {
    public:
        using datatype = int;
        int2(): x {0}, y {0} {}
        int2(int x_, int y_): x {x_}, y {y_} {}
        int2(const int2 &other) = default;

        int2 & operator=(const int2 &other) {
            x = other.x;
            y = other.y;
            return *this;
        }
        int x;
        int y;
};

class schar2 {
    public:
        using datatype = signed char;
        signed char x;
        signed char y;
};

bool operator!= (const short2 &lhs, const short2 &rhs);
bool operator!= (const int2 &lhs, const int2 &rhs);
bool operator!= (const schar2 &lhs, const schar2 &rhs);

std::ostream & operator<<(std::ostream &, const short2 & s);
std::ostream & operator<<(std::ostream &, const int2 & s);
std::ostream & operator<<(std::ostream &, const schar2 & s);

#endif
