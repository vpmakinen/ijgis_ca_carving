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

#ifndef CARVING_HELP_CPU_H
#define CARVING_HELP_CPU_H

#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <chrono>

#include "geo.h"
#include "system_utils.h"
#include "coordinates.h"


//template<typename C>
//std::vector<C> get_neighbors(
//    const C &c,
//    typename C::datatype nx,
//    typename C::datatype ny);

template<typename C>
std::vector<C> get_neighbors(
    const C &c,
    const std::map<C, C> &culvs,
    typename C::datatype nx,
    typename C::datatype ny);

template<typename C>
std::vector<C> get_neighbors(
    const C &c,
    const std::map<C, C> &culvs,
    typename C::datatype nx,
    typename C::datatype ny)
{
    std::vector<C> ret;
    for (int n = 0; n < 8; ++n) {
        auto d = coordinates::get_neig_circular(n);

        C cn {coordinates::move_coord(c, d, nx, ny)};
        if (c == cn) continue;
        ret.push_back(cn);
    }
    if (culvs.find(c) != culvs.end())
    {
        ret.push_back(culvs.at(c));
    }
    return ret;
}

template<typename T, typename C>
std::list<std::pair<C, T>> find_minima(
        const CellGrid<T, C> & dem)
{
    const T* dem_data = dem.data();
    auto nx = dem.px_width();
    auto ny = dem.px_height();
    std::list<std::pair<C, T>> minima;
    for (typename C::datatype j = 0; j < ny; ++j) {
        for (typename C::datatype i = 0; i < nx; ++i) {
            C c {i, j};
            T h {dem_data[dem.to_raster_index(c)]};
            T steepest {0};

            for (int n = 0; n < 8; ++n) {
                auto d = coordinates::get_neig_circular(n);

                C cn {coordinates::move_coord(c, d, nx, ny)};
                if (cn == c) continue;
                auto ind = dem.to_raster_index(cn);

                T h_ {dem_data[ind]};
                if (h_ < h) {
                    T dist {static_cast<T>(d.norm())};
                    T st {(h_ - h) / dist};
                    if (st < steepest) {
                        steepest = st;
                    }
                }
            }
            if (system_utils::compare_exact(steepest, static_cast<T>(0)))
                minima.push_back({c, h});
        }
    }
    return minima;
}

template<typename T, typename U, typename V, typename C>
void backtrack(
    C c,
    T* dem_data,
    const U* fd_data,
    V* carved_data,
    typename C::datatype wpad,
    typename C::datatype hpad)
{
    auto ind = coordinates::to_raster_index(c, wpad);
    T h {dem_data[ind]};
    while (true) {
        const U &f = fd_data[ind];
        C cn = coordinates::move_coord(c, {f.x, f.y}, wpad, hpad);
        if (cn == c) break;
        c = cn;
        ind = coordinates::to_raster_index(c, wpad);
        if (dem_data[ind] <= h) break;
        dem_data[ind] = h;
        carved_data[ind] = true;
    }
}

template<typename T>
struct compare_pq
{
    bool operator()(const T &a, const T &b)
    {
        return a.first > b.first;
    }
};

#endif
