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

#ifndef CARVING_ENGINE_CPU_H
#define CARVING_ENGINE_CPU_H

#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <chrono>

#include "CarvingEngine.h"
#include "carving_help_CPU.h"

#include "LinkedCells.h"
#include "geo.h"
#include "system_utils.h"
#include "coordinates.h"


template<typename T, typename U, typename V, typename C>
class CarvingEngineCPU:
    public CarvingEngine<T, U, V, C>
{
    public:
        void perform_carving(
            CellGrid<T, C> &,
            CellGrid<V, C> &,
            CellGrid<char, C> &,
            const LinkedCells<C> &,
            const std::map<C, C> &);

    protected:
        void level_linked_cells(
            CellGrid<T, C> &,
            const LinkedCells<C> &);

};


/* implementations */


template<typename T, typename U, typename V, typename C>
void CarvingEngineCPU<T, U, V, C>::
level_linked_cells(
        CellGrid<T, C> & dem,
        const LinkedCells<C> & lc)
{
    T * dem_data {dem.data()};

    for (const auto &group: lc.groups_) {
        T h_min {std::numeric_limits<T>::max()};
        for (C c: group) {
            h_min = std::min(h_min, dem_data[dem.to_raster_index(c)]);
        }
        for (C c: group) {
            dem_data[dem.to_raster_index(c)] = h_min;
            // FIXME should we bevel the neighboring cells?
        }
    }
}

template<typename T, typename U, typename V, typename C>
void CarvingEngineCPU<T, U, V, C>::perform_carving(
        CellGrid<T, C> & dem,
        CellGrid<V, C> & flowdirs,
        CellGrid<char, C> & carved,
        const LinkedCells<C> & lc,
        const std::map<C, C> &raster_culverts)
{
    level_linked_cells(dem, lc);

    auto wpad = dem.px_width();
    auto hpad = dem.px_height();

    size_t n_inserted {0};
    size_t n_pr {0};
    size_t n_limit {std::max(static_cast<unsigned long>(1),
                             dem.px_size() / 50)};

    auto minima = find_minima(dem);
    std::vector<bool> inserted_(dem.px_size(), false);

    T* dem_data = dem.data();
    V* fd_data = flowdirs.data();

    auto t0 = std::chrono::high_resolution_clock::now();

    // multimap
    //std::multimap<T, CT> queue;
    // pq
    using P = std::pair<T, C>;
    std::priority_queue<P, std::vector<P>, compare_pq<P>> queue;

    std::set<C> s_minima;
    T max_val {0};
    for (const auto &p: minima) {
        const C &c = p.first;
        if (c.col() == 0 || c.col() == wpad - 1 ||
            c.row() == 0 || c.row() == hpad - 1)
        {
            // for multimap
            //queue.insert({p.second, c});
            // for qp
            queue.push({p.second, c});

            max_val = std::max(max_val, p.second);
            ++n_inserted;
            inserted_[dem.to_raster_index(c)] = true;
        } else {
            s_minima.insert(c);
        }
    }

    const int len {static_cast<int>(std::floor(std::log10(wpad * hpad) + 1))};
    while (queue.size() > 0) {
        // for multimap
        //auto it = queue.begin();
        //CT c {it->second};
        //T h_cur {it->first};
        //queue.erase(it);
        // for pq
        C c {queue.top().second};
        T h_cur {queue.top().first};
        queue.pop();

        for (const auto &nc: get_neighbors(c, raster_culverts, wpad, hpad))
        {
            auto indn = coordinates::to_raster_index(nc, wpad);
            if (!inserted_[indn]) {
                fd_data[indn] =
                    {static_cast<short>(c.col() - nc.col()),
                     static_cast<short>(c.row() - nc.row())};
                if (s_minima.count(nc)) {
                    backtrack(nc, dem_data, fd_data, carved.data(),
                        dem.px_width(), dem.px_height());
                }
                T h {dem_data[indn]};
                max_val = std::max(max_val, h);
                // for multimap
                //queue.insert({dem_data[nc.row() * wpad + nc.col()], nc});
                // for pq
                queue.push({h, nc});

                ++n_inserted;
                inserted_[indn] = true;
            }
        }
        if (n_inserted / n_limit > n_pr) {
            logging::pLog() << std::setw(len) << std::setfill(' ') << n_inserted
                << " / " << (wpad * hpad) << ",  q: " << std::setw(len - 2)
                << queue.size() << " [" << h_cur << " ... " << max_val << "]";
            ++n_pr;
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    logging::pLog() << "Carving(PQ) performed in " << std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() << " seconds.";
}

#endif
