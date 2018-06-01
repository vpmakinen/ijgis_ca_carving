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

#ifndef VECTORIZE_H_
#define VECTORIZE_H_

#include <vector>

#include "geo.h"
#include "geometrics.h"

namespace vectorize {

    template<typename T, typename U, typename X, typename C>
    std::vector<std::pair<std::vector<geo::GeoCoordinate>, T>>
    vectorize_stream_like_raster(
        const CellGrid<T, C> & raster,
        const CellGrid<U, C> & flowdirs,
        const CellGrid<X, C> & delta_dem,
        const T & threshold);

    template<typename T, typename U, typename X, typename C>
    std::vector<std::pair<std::vector<geo::GeoCoordinate>, T>>
    vectorize_stream_like_raster(
        const CellGrid<T, C> & raster,
        const U * flowdirs,
        const X * delta_dem,
        const T & threshold);

    template<typename C>
    std::vector<C> simplify_line(
        const std::vector<C> & orig_line,
        double err_limit);






    /* template definitions */

    template<typename T, typename U, typename X, typename C>
    std::vector<std::pair<std::vector<geo::GeoCoordinate>, T>>
    vectorize_stream_like_raster(
        const CellGrid<T, C> & raster,
        const CellGrid<U, C> & flowdirs,
        const CellGrid<X, C> & delta_dem,
        const T & threshold)
    {
        using Line = std::pair<std::vector<geo::GeoCoordinate>, T>;
        std::vector<Line> ret;

        std::vector<Line> ret_ {vectorize_stream_like_raster(
            raster,
            flowdirs.data(),
            delta_dem.data(),
            threshold)};
        ret.insert(ret.end(), ret_.begin(), ret_.end());

        return ret;
    }

    template<typename T, typename U, typename X, typename C>
    std::vector<std::pair<std::vector<geo::GeoCoordinate>, T>>
    vectorize_stream_like_raster(
        const CellGrid<T, C> & raster,
        const U * flowdir_data,
        const X * delta_dem_data,
        const T & threshold)
    {
        using ct = typename C::datatype;
        std::vector<std::pair<std::vector<geo::GeoCoordinate>, T>> lines;

        ct nx {raster.px_width()};
        ct ny {raster.px_height()};

        const T * raster_data {raster.data()};
        std::set<C> starting_points;

        unsigned int p {0};
        for (ct j = 0; j < ny; ++j)
        {
            unsigned int np {static_cast<unsigned int>(j * 100 / ny)};
            if (np > p) {
                p = np;
            }
            for (ct i = 0; i < nx; ++i)
            {
                C c {i, j};
                size_t ind {coordinates::to_raster_index(i, j, nx)};
                if (raster_data[ind] < threshold) continue;
                unsigned int add {0};
                if (delta_dem_data && delta_dem_data[ind] != 0) add = 2;
                for (int dj = -1; dj <= 1; ++dj)
                {
                    for (int di = -1; di <= 1; ++di)
                    {
                        auto cn = coordinates::move_coord(c, {di, dj}, nx, ny);
                        if (cn == c) continue;
                        size_t ind_n {coordinates::to_raster_index(cn, nx)};
                        U fd {flowdir_data[ind_n]};
                        if (di == -fd.x && dj == -fd.y &&
                            raster_data[ind_n] >= threshold)
                        {
                            ++add;
                        }
                    }
                }
                // add == 0: a start point of a new stream
                // add > 1: two or more stream join at the given cell
                if (add != 1) {
                    starting_points.insert(c);
                }
            }
        }

        p = 0;
        unsigned int n {0};
        for (auto c: starting_points)
        {
            unsigned int np {static_cast<unsigned int>(++n * 100 / starting_points.size())};
            if (np > p) {
                p = np;
            }
            T cat {raster_data[c.row() * nx + c.col()]};
            std::vector<C> line;
            while (true) {
                line.push_back(c);
                if (line.size() > 1 && starting_points.count(c)) break;
                U fd {flowdir_data[coordinates::to_raster_index(c, nx)]};
                if (fd.x == 0 && fd.y == 0) {
                    break;
                }
                if (std::abs(fd.x) > 1 || std::abs(fd.y) > 1) {
                    // only follow primitive flow directions
                    break;
                }
                auto cn = coordinates::move_coord(c, {fd.x, fd.y}, nx, ny);
                if (cn == c) break;
                c = cn;
            }
            std::vector<C> rline {simplify_line(line, 2.0)};
            std::vector<geo::GeoCoordinate> gline;
            for (const auto &cc: rline) {
                gline.push_back(raster.to_geocoordinate(cc));
            }
            lines.push_back({gline, cat});
        }
        return lines;
    }

    /**
     * \brief Simplify the given line using Ramer–Douglas–Peucker algorithm.
     */
    template<typename C>
    std::vector<C> simplify_line(
        const std::vector<C> &orig_line,
        double err_limit)
    {
        if (orig_line.size() > std::numeric_limits<int>::max()) {
            throw std::runtime_error("Too many points on the line.");
        }
        if (orig_line.size()< 3) return orig_line;
        double max_err {0.0};
        int max_ind {-1};
        for (unsigned int i = 1; i < orig_line.size() - 1; ++i) {
            double d {distance_from_line(orig_line.front(), orig_line.back(), orig_line[i])};
            if (d > max_err) {
                max_err = d;
                max_ind = static_cast<int>(i);
            }
        }
        std::vector<C> simp_line;
        if (max_err< err_limit) {
            simp_line.push_back(orig_line.front());
            simp_line.push_back(orig_line.back());
        } else {
            std::vector<C> a {simplify_line(std::vector<C>(
                orig_line.begin(), orig_line.begin() + max_ind + 1), err_limit)};
            std::vector<C> b {simplify_line(std::vector<C>(
                orig_line.begin() + max_ind, orig_line.end()), err_limit)};
            simp_line.insert(simp_line.end(), a.begin(), a.end());
            simp_line.insert(simp_line.end(), b.begin(), b.end());
        }
        return simp_line;
    }

}

#endif
