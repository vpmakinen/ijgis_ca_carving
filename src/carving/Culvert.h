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

#ifndef CULVERT_H_
#define CULVERT_H_

#include <iostream>

template<typename T>
class Culvert
{
    public:
        Culvert(
            const geo::PixelCenterCoordinate &sink,
            const geo::PixelCenterCoordinate &source,
            T id);//,
            //bool two_way = false);

        geo::GeoCoordinate center() const {
            return average(sink_, source_);
        }

        geo::PixelCenterCoordinate sink() const { return sink_; }

        geo::PixelCenterCoordinate source() const { return source_; }

        T id() const { return id_; }

        bool two_way() const { return true; }

    private:
        geo::PixelCenterCoordinate sink_;
        geo::PixelCenterCoordinate source_;
        T id_;
        //bool two_way_;
};

template<typename T>
std::ostream & operator<<(std::ostream &os, const Culvert<T> &c)
{
    os << "Culvert(" << static_cast<int>(c.id()) << ")\n";
    os << "  center: " << c.center() << std::endl;
    os << "  sink-source: " << c.sink() << " " << c.source() << std::endl;
    return os;
}


template<typename T>
Culvert<T>::Culvert(
        const geo::PixelCenterCoordinate &sink,
        const geo::PixelCenterCoordinate &source,
        T id)://,
        //bool two_way):
    sink_ {sink},
    source_ {source},
    id_ {id}//,
    //two_way_ {two_way}
{
}

#endif
