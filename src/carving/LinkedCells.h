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

#ifndef LINKED_CELLS_H_
#define LINKED_CELLS_H_

#include <list>
#include <map>

template<typename coord_type>
class LinkedCells {
    public:
        size_t n_groups() const { return groups_.size(); }
        std::list<std::vector<coord_type>> groups_;
        std::map<coord_type, std::vector<coord_type>*> links_;
};

template<typename CG>
LinkedCells<typename CG::coord_type> create_linked_cells(
    const CG & ddem)
{
    using T = typename CG::value_type;
    using coord = typename CG::coord_type;
    using s_t = typename coord::datatype;

    LinkedCells<coord> ret;

    //auto & ddem = *(ddem_cont.get());
    const T * data = ddem.data();
    std::map<T, std::vector<coord>> groups;
    for (s_t j = 0; j < ddem.px_height(); ++j) {
        for (s_t i = 0; i < ddem.px_width(); ++i) {
            T val = data[j * ddem.px_width() + i];
            if (val != 0) {
                if (groups.find(val) == groups.end()) {
                    groups[val] = {};
                }
                groups[val].push_back({i, j});
            }
        }
    }
    for (auto it = groups.begin(); it != groups.end(); ++it) {
        ret.groups_.push_back(it->second);
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt) {
            ret.links_[*jt] = &(ret.groups_.back());
        }
    }

    logging::pLog() << "Number of culverts: " << ret.n_groups();
    return ret;
}

#endif
