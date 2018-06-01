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

#ifndef INSERT_CULVERT_ALGORITHM_H_
#define INSERT_CULVERT_ALGORITHM_H_

#include <tuple>
#include <list>
#include <map>

#include "AbstractAlgorithm.h"
#include "CellGrid.h"
#include "Culvert.h"
#include "geometrics.h"
#include "system_utils.h"

/**
 * \brief Return to raster coordinates that define a rectangle
 *
 * Return the raster coordinate limits for a rectangle defined by the
 * points p1 and p2, to which a halo region of width radius is added. The
 * returned rectangle points are clipped by the nx and ny.
 *
 *  +------------------+
 *  |                  |
 *  |     +-----------+|               +-----------+
 *  |     |           ||               |           |
 *  |     |  p1----+  ||     =>        |           |
 *  |     |  |     |  ||               |           |
 *  |     |  +----p2  ||               |           |
 *  +-----+-----------++               +-----------+
 *        +-----------+
 */
template<typename C>
std::pair<C, C> create_window_limits(
    unsigned int nx,
    unsigned int ny,
    const C & p1,
    const C & p2,
    unsigned int radius)
{
    int irad {static_cast<int>(radius)};
    unsigned int min_i {static_cast<unsigned int>(std::max(
        0, static_cast<int>(std::min(p1.col(), p2.col())) - irad))};
    unsigned int max_i {static_cast<unsigned int>(std::min(
        std::max(p1.col(), p2.col()) + radius,
        static_cast<unsigned int>(nx - 1)))};
    unsigned int min_j {static_cast<unsigned int>(std::max(
        static_cast<int>(0),
        static_cast<int>(std::min(p1.row(), p2.row())) - irad))};
    unsigned int max_j {static_cast<unsigned int>(std::min(
        std::max(p1.row(), p2.row()) + radius,
        static_cast<unsigned int>(ny - 1)))};
    return {{min_i, min_j}, {max_i, max_j}};
}

template<typename T, typename C>
void create_window(
    const CellGrid<T, C> & cg,
    const C & p1,
    const C & p2,
    unsigned int radius,
    std::unique_ptr<T> & uptr,
    C & min_corner,
    C & max_corner,
    bool fill = true)
{
    unsigned int nx {static_cast<unsigned int>(cg.px_width())};
    unsigned int ny {static_cast<unsigned int>(cg.px_height())};
    auto ret = create_window_limits(nx, ny, p1, p2, radius);
    std::tie(min_corner, max_corner) = ret;
    unsigned int nx_ {max_corner.col() - min_corner.col() + 1};
    unsigned int ny_ {max_corner.row() - min_corner.row() + 1};
    unsigned int n_ {nx_ * ny_};
    uptr.reset(new T[n_]);
    if (fill) {
        const T * data {cg.data()};
        T * window {uptr.get()};
        for (unsigned int j = min_corner.row(); j <= max_corner.row(); ++j) {
            for (unsigned int i = min_corner.col(); i <= max_corner.col(); ++i) {
                unsigned int j_ {j - min_corner.row()};
                unsigned int i_ {i - min_corner.col()};
                window[to_raster_index(C {i_, j_}, nx_)] =
                    data[to_raster_index(C {i, j}, nx)];
            }
        }
    }
}

class InsertCulvertAlgorithm: public AbstractAlgorithm
{
    public:
        InsertCulvertAlgorithm();
        virtual ~InsertCulvertAlgorithm();

        template<typename T, typename U, typename V,
                 typename X, typename C>
        std::pair<Culvert<X>, bool> insert_culvert_pit_fill_upstream(
            CellGrid<T, C> & dem,
            CellGrid<U, C> & roads,
            CellGrid<V, C> & flowdir,
            CellGrid<T, C> & filled_dem,
            const geo::RasterArea & insert_area,
            const std::pair<double, double> & culvert_len_lims,
            X id,
            const C &);

        template<typename T, typename V, typename X, typename Y, typename C>
        std::pair<Culvert<X>, bool> insert_culvert_along_flow_route(
            const CellGrid<T, C> & dem,
            const CellGrid<V, C> & flowdirs,
            const CellGrid<X, C> & delta_dem,
            const CellGrid<Y, C> & roads,
            const std::pair<double, double> & culvert_len_lims,
            const C & start,
            X id);

        template<typename V, typename C>
        bool crosses_road(
            const CellGrid<V, C> &,
            C,
            C);

        template<typename V, typename C>
        void extend_roads_with_buffer_region(
            CellGrid<V, C> &,
            V road_value,
            V buffer_value,
            double width);

        template<typename V, typename C>
        void extend_roads_with_buffer_region(
            V * road_data,
            unsigned int nx,
            unsigned int ny,
            V road_value,
            V buffer_value,
            double width);

        template<typename T, typename C>
        void fill_area_with_id(
            T * data,
            typename C::datatype nx,
            typename C::datatype ny,
            const C & start,
            T to_be_replaced,
            T replacing_value);

        template<typename T, typename C>
        void fill_area_with_id(
            CellGrid<T, C> & cg,
            const C &c,
            T no_data_value,
            T id);

        template<typename T, typename C>
        void fill_areas_with_unique_id(
            CellGrid<T, C> & cg,
            T to_be_replaced,
            T first_free_id);

        template<typename T, typename C>
        void burn_culverts(
            CellGrid<T, C> & delta_dem,
            std::vector<Culvert<T>> &);

        template<typename T, typename C>
        void burn_culvert(
            CellGrid<T, C> & cg,
            Culvert<T> & c);

        template<typename T, typename U, typename C>
        std::multimap<T, std::tuple<C, C, double>> find_expensive_carvings(
            const CellGrid<T, C> & dem_orig,
            const CellGrid<T, C> & dem_carved,
            const CellGrid<U, C> & flowdirs,
            double min_cost,
            double min_hdiff,
            double min_culvert_length);

        template<typename T, typename U, typename C>
        std::multimap<T, std::tuple<C, C, double>> find_expensive_carvings_(
            const CellGrid<T, C> & dem_orig,
            const CellGrid<T, C> & dem_carved,
            const CellGrid<U, C> & flowdirs,
            double min_cost,
            double min_hdiff,
            double min_culvert_length);

        template<typename T, typename U, typename C>
        std::multimap<T, C> find_expensive_carvings_upstream_points(
            const CellGrid<T, C> & dem_carved,
            const T * diff,
            const CellGrid<U, C> & flowdirs);

        template<typename T, typename U, typename C>
        std::pair<C, bool> find_alternative_carving_near_roads(
            const CellGrid<T, C> & dem,
            const CellGrid<U, C> & roads,
            const geo::RasterArea & insert_area,
            const C & start,
            T cost,
            std::pair<double, double> culvert_length_limits,
            std::unique_ptr<T> &,
            C &,
            C &);
};






/* implementation */

/**
 * \brief Return true if there is a road cell along the line (roughy)
 * between a and b.
 */
template<typename V, typename C>
bool InsertCulvertAlgorithm::crosses_road(
        const CellGrid<V, C> & roads,
        C a,
        C b)
{
    if (a.col() > b.col()) std::swap(a, b);
    int dj {a.row() < b.row() ? 1 : -1};

    unsigned int nx = roads.px_width();
    unsigned int ny = roads.px_height();
    C c {a};
    while (true) {
        if (roads.data()[to_raster_index(c, roads.px_width())] == 1)
            return true;
        if (c == b) break;
        if (c.col() == b.col()) {
            c = move_coord(c, {0, dj}, nx, ny);
        } else if (c.row() == b.row()) {
            c = move_coord(c, {1, 0}, nx, ny);
        } else {
            // move either to the right or up/down, depending on which of
            // the cells is closer to the line from a to b.
            double dist_i = distance_from_line(a, b, coordinates::move_coord(
                c, {1, 0}, nx, ny));
            double dist_j = distance_from_line(a, b, coordinates::move_coord(
                c, {0, dj}, nx, ny));
            if (dist_i < dist_j) {
                c = move_coord(c, {1, 0}, nx, ny);
            } else {
                c = move_coord(c, {0, dj}, nx, ny);
            }
        }
    }
    return false;
}

template<typename T>
std::vector<T> d8_neighbors(
        const T &coord,
        typename T::datatype nx,
        typename T::datatype ny)
{
    std::vector<T> neighs(8, coord);
    auto i = neighs.begin();
    for (int n = 0; n < 8; ++n) {
        auto fd = coordinates::get_neig_circular(n);
        auto cn = coordinates::move_coord(coord, fd, nx, ny);
        if (cn == coord) continue;
        *i = cn;
        ++i;
    }
    return std::vector<T>(neighs.begin(), i);
}

template<typename T, typename U, typename V,
         typename X, typename C>
std::pair<Culvert<X>, bool> InsertCulvertAlgorithm::insert_culvert_pit_fill_upstream(
    CellGrid<T, C> & dem,
    CellGrid<U, C> & roads,
    CellGrid<V, C> & flowdirs,
    CellGrid<T, C> & filled_dem,
    const geo::RasterArea & insert_area,
    const std::pair<double, double> & culvert_len_lims,
    X id,
    const C & start)
{
    const std::pair<Culvert<X>, bool> return_fail {{{0, 0}, {0, 0}, 0}, false};

    logging::LogIndent li;
    T * dem_data {dem.data()};
    U * road_data {roads.data()};

    unsigned int nx {dem.px_width()};
    unsigned int ny {dem.px_height()};

    // follow upstream and downstream to the first non-road cells
    //
    // Assumption is that the road cells have higher elevation than the
    // near non-road cells. If the road data does not accurately match
    // the DEM, it is possible that the start cell is lower than the first
    // non-road upstream cell and furthermore all it's neighbours can be
    // even higher. In this case the simulated pit filling stops after the
    // first step.
    //
    // One solution: force the search on the upstream side with a radius
    // of few cells until lower cells are found.
    C c_up {start};
    T h_start {dem_data[to_raster_index(start, nx)]};
    {
        std::list<C> queue;
        queue.push_back(start);
        while (queue.size() > 0) {
            C c_t {queue.front()};
            queue.pop_front();
            size_t ind {to_raster_index(c_t, nx)};
            if (road_data[ind] > 1) {
                if (dem_data[ind] < h_start) {
                    c_up = c_t;
                    break;
                }
            }
            for (const C & cn: d8_neighbors(c_t, nx, ny)) {
                size_t nind {to_raster_index(cn, nx)};
                V fd {flowdirs.data()[nind]};
                if (move_coord(cn, {fd.x, fd.y}, nx, ny) != c_t) continue;
                if (static_cast<double>((cn - start).norm_squared()) > pow(culvert_len_lims.second / 2, 2)) continue;
                if (road_data[nind] == 0) continue;
                queue.push_back(cn);
            }
        }
    }
    if (c_up == start) {
        return return_fail;
    }
    T h_up {dem_data[to_raster_index(c_up, nx)]};

    // perform "pit filling"
    std::pair<C, T> lowest {c_up, h_up};
    {
        std::set<C> flooded_cells;
        std::list<C> queue;
        queue.push_back(c_up);
        while (queue.size() > 0) {
            C c = queue.front();
            size_t ind {to_raster_index(c, nx)};
            queue.pop_front();
            if (flooded_cells.find(c) == flooded_cells.end() &&
                dem_data[ind] < h_start &&
                road_data[ind] > static_cast<U>(1))
            {
                flooded_cells.insert(c);
                if (flooded_cells.size() == 1) {
                    filled_dem.data()[ind] = 99999; // for debugging
                } else {
                    filled_dem.data()[ind] = h_start; // for debugging
                }
                T h {dem_data[ind]};
                if (h < lowest.second) {
                    lowest = std::make_pair(c, h);
                }
                for (const C & cn: d8_neighbors(c, nx, ny)) {
                    if (flooded_cells.find(cn) == flooded_cells.end()) {
                        queue.push_back(cn);
                    }
                }
            }
        }
        auto l = lowest.first;
        filled_dem.data()[to_raster_index(l, nx)] = 99998;
    }
    c_up = lowest.first;

    std::unique_ptr<T> cost_window;
    C c_min {0, 0};
    C c_max {0, 0};
    auto ret = find_alternative_carving_near_roads(
        dem,
        roads,
        insert_area,
        c_up,
        std::numeric_limits<T>::max(),
        culvert_len_lims,
        cost_window, c_min, c_max);

    geo::PixelCenterCoordinate g_up {dem.to_geocoordinate(c_up)};
    geo::PixelCenterCoordinate g_down {dem.to_geocoordinate(ret.first)};
    return {{g_up, g_down, id}, ret.second};
}

template<typename T, typename U, typename C>
U assign_flowdir_slope(
    const T * dem,
    size_t nx,
    size_t ny,
    const C &c)
{
    U ret {0, 0};
    T h {dem[to_raster_index(c, nx)]};
    double steepest {0};
    for (int dj = -1; dj <= 1; ++dj) {
        for (int di = -1; di <= 1; ++di) {
            C cn {move_coord(c, di, dj, static_cast<unsigned int>(nx), static_cast<unsigned int>(ny))};
            if (cn == c) continue;
            double dist {sqrt(di * di + dj * dj)};
            double slope = static_cast<double>(
                dem[to_raster_index(cn, nx)] - h) / dist;
            if (slope < steepest) {
                steepest = slope;
                ret = {di, dj};
            }
        }
    }
    return ret;
}

template<typename T, typename C>
void InsertCulvertAlgorithm::fill_area_with_id(
    T * data,
    typename C::datatype nx,
    typename C::datatype ny,
    const C & start,
    T to_be_replaced,
    T replacing_value)
{
    std::list<C> queue;
    queue.push_back(start);
    while (queue.size() > 0) {
        C c {queue.front()};
        queue.pop_front();
        if (data[to_raster_index(c, nx)] == to_be_replaced) {
            data[to_raster_index(c, nx)] = replacing_value;
            auto cn = move_coord(c, {-1, 0}, nx, ny);
            if (c != cn) queue.push_back(cn);
            cn = move_coord(c, {1, 0}, nx, ny);
            if (c != cn) queue.push_back(cn);
            cn = move_coord(c, {0, -1}, nx, ny);
            if (c != cn) queue.push_back(cn);
            cn = move_coord(c, {0, 1}, nx, ny);
            if (c != cn) queue.push_back(cn);
        }
    }
}

template<typename T, typename C>
void InsertCulvertAlgorithm::fill_area_with_id(
    CellGrid<T, C> & cg,
    const C &start,
    T no_data_value,
    T id)
{
    fill_area_with_id(cg.data(),
        static_cast<typename C::datatype>(cg.px_width()),
        static_cast<typename C::datatype>(cg.px_height()),
        start, no_data_value, id);
}

template<typename T, typename C>
void InsertCulvertAlgorithm::fill_areas_with_unique_id(
    CellGrid<T, C> & cg,
    T to_be_replaced,
    T first_free_id)
{
    using ct = typename C::datatype;
    T * data {cg.data()};
    for (ct j = 0; j < cg.px_height(); ++j) {
        for (ct i = 0; i < cg.px_width(); ++i) {
            if (data[to_raster_index(C {i, j}, cg.px_width())] == to_be_replaced) {
                fill_area_with_id(
                    cg, {i, j}, to_be_replaced,
                    first_free_id++);
            }
        }
    }
}

template<typename T, typename C>
void InsertCulvertAlgorithm::burn_culverts(
    CellGrid<T, C> & delta_dem,
    std::vector<Culvert<T>> &culverts)
{
    for (auto &c: culverts)
    {
        burn_culvert(delta_dem, c);
    }
}

template<typename T, typename C>
void InsertCulvertAlgorithm::burn_culvert(
    CellGrid<T, C> & cg,
    Culvert<T> & c)
{
    T * data {cg.data()};
    unsigned int nx {static_cast<unsigned int>(cg.px_width())};
    auto c_sink = cg.area().to_raster_coordinate(c.sink());
    auto c_source = cg.area().to_raster_coordinate(c.source());
    data[to_raster_index(c_sink, nx)] = c.id();
    data[to_raster_index(c_source, nx)] = c.id();
}

template<typename T, typename V, typename X, typename Y, typename C>
std::pair<Culvert<X>, bool> InsertCulvertAlgorithm::insert_culvert_along_flow_route(
    const CellGrid<T, C> & dem,
    const CellGrid<V, C> & flowdirs,
    const CellGrid<X, C> & delta_dem,
    const CellGrid<Y, C> & roads,
    const std::pair<double, double> & culvert_len_lims,
    const C & start,
    X id)
{
    std::pair<Culvert<X>, bool> return_fail {{{0, 0}, {0, 0}, 0}, false};

    try {
        //logging::pLog() << "insert_culvert_along_flow_route " << start;
        logging::LogIndent li;
        const T * dem_data {dem.data()};
        const Y * road_data {roads.data()};
        auto nx = static_cast<typename C::datatype>(dem.px_width());
        auto ny = static_cast<typename C::datatype>(dem.px_height());
        std::set<X> encountered_ids;
        C c_src {start};
        C c_sink {start};
        T h_start {dem_data[to_raster_index(start, nx)]};
        {
            std::list<C> queue;
            queue.push_back(start);
            std::set<C> upstream_cells;
            // Find all the cells that flow to the start cell and that are
            // further than min_culvert_length / 2 but closer than
            // max_culvert_length / 2 to the start.
            while (queue.size() > 0) {
                C c_t {queue.front()};
                queue.pop_front();
                size_t ind {to_raster_index(c_t, nx)};
                if (static_cast<double>((c_t - start).norm_squared()) >= pow(culvert_len_lims.first, 2) &&
                        road_data[ind] > 1) {
                    upstream_cells.insert(c_t);
                }
                for (const C & cn: d8_neighbors(c_t, nx, ny)) {
                    size_t nind {to_raster_index(cn, nx)};
                    V fd {flowdirs.data()[nind]};
                    // check if the cell flows out of the area
                    if (move_coord(cn, {fd.x, fd.y}, nx, ny) != c_t) continue;
                    if (static_cast<double>((cn - start).norm_squared()) > pow(culvert_len_lims.second / 2, 2)) continue;
                    if (delta_dem.data()[nind] != 0) {
                        continue;
                        // FIXME at least continue if the cell is sink.
                        // If the cell is source, perhaps something else
                        // could be done.
                    }
                    queue.push_back(cn);
                }
            }
            // From the found cells choose the lowest.
            if (upstream_cells.size() == 0) {
                return return_fail;
            }
            c_sink = [&]() {
                T hmin {std::numeric_limits<T>::max()};
                C c_ret {*(upstream_cells.begin())};
                for (const C & c: upstream_cells) {
                    size_t ind {to_raster_index(c, nx)};
                    T h {dem_data[ind]};
                    if (h < hmin) {
                        hmin = h;
                        c_ret = c;
                    }
                }
                return c_ret;
            }();
        }

        // Follow the flow directions to the first non-road cell from
        // the intersection
        C c_tmp {start};
        while (true) {
            V fd {flowdirs.data()[to_raster_index(c_tmp, nx)]};
            C c_next {move_coord(c_tmp, {fd.x, fd.y}, nx, ny)};
            if (c_next == c_tmp) break;
            c_tmp = c_next;
            if (road_data[to_raster_index(c_tmp, nx)] > 1)
                break;
        }
        c_src = c_tmp;
        {
            // Find all the cells on the downstream side that are lower
            // than the found c_src and that are connected to the c_src
            // (within the max_culvert_length).
            std::set<C> visited_cells;
            std::list<C> queue;
            std::multimap<double, C> connected_cells;
            visited_cells.insert(c_src);
            connected_cells.insert({static_cast<double>((c_src - c_sink).norm_squared()), c_src});
            queue.push_back(c_src);
            while (queue.size() > 0) {
                C c {queue.front()};
                queue.pop_front();
                for (const C & cn: d8_neighbors(c, nx, ny)) {
                    if (visited_cells.find(cn) != visited_cells.end()) continue;
                    visited_cells.insert(cn);
                    double dist2 {static_cast<double>((cn - c_sink).norm_squared())};
                    if (dist2 > pow(culvert_len_lims.second, 2)) continue;
                    size_t ind {to_raster_index(cn, nx)};
                    if (road_data[ind] < 2) continue;
                    T h {dem_data[ind]};
                    if (h <= h_start) {
                        connected_cells.insert({dist2, cn});
                        queue.push_back(cn);
                    }
                }
            }
            T h_sink {dem_data[to_raster_index(c_sink, nx)]};
            bool found {false};
            for (const auto &c: connected_cells) {
                T h {dem_data[to_raster_index(c.second, nx)]};
                if (h < h_sink) {
                    c_src = c.second;
                    found = true;
                    break;
                }
            }
            if (!found) {
                c_src = connected_cells.begin()->second;
            }
        }
        if (c_src.col() > nx || c_src.row() > ny) {
            logging::pErr() << "c_src out of the area.";
            return return_fail;
        }

        if ((c_sink == c_src) ||
            (encountered_ids.size() > 1))
            return return_fail;
        if (encountered_ids.size() == 1) {
            id = *(encountered_ids.begin());
        }
        return {{delta_dem.to_geocoordinate(c_sink),
                 delta_dem.to_geocoordinate(c_src),
                 id},
                true};
    } catch (std::runtime_error & e) {
        logging::pErr() << e.what();
        return return_fail;
    }
}

template<typename T, typename U, typename C>
std::multimap<T, std::tuple<C, C, double>> InsertCulvertAlgorithm::find_expensive_carvings(
    const CellGrid<T, C> & dem_orig,
    const CellGrid<T, C> & dem_carved,
    const CellGrid<U, C> & flowdirs,
    double min_cost,
    double min_hdiff,
    double min_culvert_length)
{
    std::multimap<T, std::tuple<C, C, double>> ret;
    auto tmp = find_expensive_carvings_(
        dem_orig,
        dem_carved,
        flowdirs,
        min_cost,
        min_hdiff,
        min_culvert_length);
    ret.insert(tmp.begin(), tmp.end());
    return ret;
}

template<typename T, typename U, typename C>
std::multimap<T, C> InsertCulvertAlgorithm::find_expensive_carvings_upstream_points(
    const CellGrid<T, C> & dem_carved,
    const T * diff,
    const CellGrid<U, C> & flowdirs)
{
    // FIXME it would be easier to store the backtrack calls from the
    // carving and check which pits to include/exclude.
    using ct = typename C::datatype;
    ct nx {static_cast<ct>(dem_carved.px_width())};
    ct ny {static_cast<ct>(dem_carved.px_height())};

    // The points where the carving starts (i.e. upstream end of the carved path)
    std::multimap<T, C> upstream_points;
    for (ct j = 0; j < ny; ++j)
    {
        for (ct i = 0; i < nx; ++i)
        {
            C c_tmp {i, j};
            if (system_utils::compare_exact(
                diff[to_raster_index(c_tmp, nx)], static_cast<T>(0)))
                continue;
            C upstream {i, j};

            bool skip {false};
            // check if the cell is a starting point of a carving (i.e. if
            // the cell was previously a spurious pit). The cell must be
            // carved and it must not have a carved neighbour that flows
            // back to this cell.
            for (const C & cn: d8_neighbors(upstream, nx, ny)) {
                if (diff[to_raster_index(cn, nx)] > 0) {
                    U fd {flowdirs.data()[to_raster_index(cn, nx)]};
                    if (move_coord(cn, {fd.x, fd.y}, nx, ny) == upstream) {
                        skip = true;
                        break;
                    }
                }
                if (skip) break;
            }
            if (skip) continue;

            // do not add the found cells, but one cell upstream so that
            // the actual pit cell is added.
            bool added {false};
            for (const C & cn: d8_neighbors(upstream, nx, ny)) {
                if (added) break;
                auto i_n = to_raster_index(cn, nx);
                U fd {flowdirs.data()[i_n]};
                if (system_utils::compare_exact(diff[i_n], static_cast<T>(0)) &&
                    system_utils::compare_exact(
                            dem_carved.data()[i_n],
                            dem_carved.data()[to_raster_index(upstream, nx)]) &&
                    move_coord(cn, {fd.x, fd.y}, nx, ny) == upstream)
                {
                    upstream_points.insert(
                        {dem_carved.data()[i_n], cn});
                    added = true;
                    break;
                }
            }
        }
    }
    return upstream_points;
}

template<typename T, typename U, typename C>
std::multimap<T, std::tuple<C, C, double>> InsertCulvertAlgorithm::find_expensive_carvings_(
    const CellGrid<T, C> & dem_orig,
    const CellGrid<T, C> & dem_carved,
    const CellGrid<U, C> & flowdirs,
    double min_cost,
    double min_hdiff,
    double min_culvert_length)
{
    using ct = typename C::datatype;
    std::multimap<T, std::tuple<C, C, double>> ret;

    ct nx {static_cast<ct>(dem_orig.px_width())};
    ct ny {static_cast<ct>(dem_orig.px_height())};
    std::unique_ptr<T> diff_ptr {new T[dem_orig.px_size()]};
    T * diff {diff_ptr.get()};
    for (size_t i = 0; i < dem_orig.px_size(); ++i)
    {
        diff[i] = dem_orig.data()[i] - dem_carved.data()[i];
    }

    // The points where the carving starts (i.e. upstream end of the
    // carved path)
    std::multimap<T, C> upstream_points = find_expensive_carvings_upstream_points(
        dem_carved, diff, flowdirs);

    for (auto it = upstream_points.begin(); it != upstream_points.end(); ++it)
    {
        // The cell upstream is a starting point of a carving. Follow it
        // downstream until a non-carved cell is found to determine the
        // cost of the carving.
        C upstream {it->second};
        C downstream {upstream};
        T cost {diff[to_raster_index(upstream, nx)]};
        T hmax {dem_orig.data()[to_raster_index(upstream, nx)]};
        double dist {0};
        while (true) {
            U fd {flowdirs.data()[to_raster_index(downstream, nx)]};
            auto tmp = move_coord(downstream, {fd.x, fd.y}, nx, ny);
            if (tmp == downstream) break;
            T cost_ {diff[dem_orig.to_raster_index(tmp)]};
            if (system_utils::compare_exact(cost_, static_cast<T>(0))) break;
            cost += cost_;
            dist += (downstream - tmp).norm();
            downstream = tmp;
            hmax = std::max(hmax, dem_orig.data()[
                to_raster_index(downstream, nx)]);
        }
        size_t ind = to_raster_index(downstream, nx);
        T h {dem_orig.data()[ind]};
        double max_hdiff {static_cast<double>(hmax - h)};
        if (static_cast<double>(hmax - h) >= min_hdiff &&
            dist >= min_culvert_length &&
            static_cast<double>(cost) >= min_cost) {
            ret.insert({cost, std::make_tuple(upstream, downstream, max_hdiff)});
        }
    }
    return ret;
}

/**
 * \brief The A* search to find a cheaper carving through the roads.
 */
template<typename T, typename U, typename C>
std::pair<C, bool> InsertCulvertAlgorithm::find_alternative_carving_near_roads(
    const CellGrid<T, C> & dem,
    const CellGrid<U, C> & roads,
    const geo::RasterArea & insert_area,
    const C & start, // the upstream end of the carving
    T orig_cost,
    std::pair<double, double> culvert_length_limits,
    std::unique_ptr<T> & cost_ptr_,
    C & min_c,
    C & max_c)
{
    unsigned int max_radius {static_cast<unsigned int>(
        std::floor(culvert_length_limits.second / dem.area().cell_size()))};

    unsigned int nx {static_cast<unsigned int>(dem.px_width())};
    unsigned int ny {static_cast<unsigned int>(dem.px_height())};

    unsigned int r2 {max_radius * max_radius};
    if (roads.data()[to_raster_index(start, nx)] == 1) {
        logging::pErr() << "find_alternative_carving_near_roads " << start << " " << dem.to_geocoordinate(start);
        return {start, false};
    }
    if (!insert_area.contains_point(dem.to_geocoordinate(start))) {
        return {start, false};
    }

    std::unique_ptr<U> roads_window;
    create_window(dem, start, start, max_radius + 1, cost_ptr_, min_c, max_c, false);
    create_window(roads, start, start, max_radius + 1, roads_window, min_c, max_c, true);
    unsigned int nx_ {max_c.col() - min_c.col() + 1};
    unsigned int ny_ {max_c.row() - min_c.row() + 1};
    auto to_window_c = [&min_c](const C &c) {
        return C {c.col() - min_c.col(), c.row() - min_c.row()};};
    auto to_global_c = [&min_c](const C &c) {
        return C {min_c.col() + c.col(), min_c.row() + c.row()};};

    T * cost_ {cost_ptr_.get()};
    const U * roads_ {roads_window.get()};
    for (size_t i = 0; i < nx_ * ny_; ++i) {
        cost_[i] = std::numeric_limits<T>::max();
    }
    C start_ {max_radius + 1, max_radius + 1};

    if (start_ != to_window_c(start)) throw std::runtime_error(
        "Coordinate conversion error 1.");
    if (start != to_global_c(to_window_c(start))) throw std::runtime_error(
        "Coordinate conversion error 2.");

    const T * dem_data {dem.data()};
    T h {dem_data[to_raster_index(start, nx)]};
    std::multimap<T, C> queue;
    queue.insert({0.0, start});
    cost_[to_raster_index(start_, nx_)] = 0.0;
    std::set<C> potential_cells;

    const T out_val {static_cast<T>(99999.0)};
    const T real_edge_val {static_cast<T>(99997.0)};

    // Starting from the center cell, use A* to determine the cost to the
    // neighbouring cells. If the cost to reach the cell is lower than the
    // original cost and if the cell is lower than the center cell, add
    // id to the list of potential cells.
    while (queue.size() > 0)
    {
        auto it = queue.begin();
        C c {it->second};
        T co {it->first};
        queue.erase(it);
        for (const C & cn: d8_neighbors(c, nx, ny)) {
            if (cn.col() < min_c.col() || cn.col() > max_c.col() ||
                cn.row() < min_c.row() || cn.row() > max_c.row())
                continue;
            C cn_ {to_window_c(cn)};
            size_t indn_ {to_raster_index(cn_, nx_)};
            T hn {dem_data[to_raster_index(cn, nx)]};
            T new_cost {co + std::max(static_cast<T>(0), hn - h)};
            if (roads_[indn_] == static_cast<U>(0)) {
                cost_[indn_] = out_val;
            } else if (new_cost > orig_cost) {
                cost_[indn_] = out_val;
            } else if (new_cost < cost_[indn_])
            {
                if ((start - cn).norm_squared() > r2) {
                    cost_[indn_] = out_val;
                } else {
                    cost_[indn_] = new_cost;
                    queue.insert({new_cost, cn});
                    if (hn < h) {
                        potential_cells.insert(cn);
                    }
                }
            }
        }
    }
    // Pick all the sources s_i for which the line (s_i, start) crosses a
    // road.
    // FIXME the source is selected even if the line simply touches a road
    // without crossing it
    std::vector<std::pair<C, double>> proper_sources;
    for (const auto &c: potential_cells) {
        if (crosses_road(roads, start, c)) {
            C c_ {to_window_c(c)};
            proper_sources.push_back({c, (start - c).norm_squared()});
            cost_[to_raster_index(c_, nx_)] = real_edge_val;
        }
    }
    if (proper_sources.size() == 0) {
        return {start, false};
    }

    // sort the sources s_i by the line length (s_i, start), in ascendind
    // order
    std::sort(proper_sources.begin(), proper_sources.end(),
        [](const auto &a, const auto &b) { return a.second < b.second; });

    // choose the first source s_i for which the line (s_i, start) is long
    // enough
    auto min_len_2 = pow(culvert_length_limits.first, 2);
    for (const auto &p: proper_sources) {
        auto & s_i = p.first;
        if (static_cast<double>((s_i - start).norm_squared()) >= min_len_2) {
            return {s_i, true};
        }
    }
    //if (db) logging::pLogDB() << "All the upstream points are too far way from te original starting point";
    return {start, false};
}

template<typename V, typename C>
void InsertCulvertAlgorithm::extend_roads_with_buffer_region(
    CellGrid<V, C> & cg,
    V road_value,
    V buffer_value,
    double width)
{
    extend_roads_with_buffer_region<V, C>(
        cg.data(),
        static_cast<unsigned int>(cg.px_width()),
        static_cast<unsigned int>(cg.px_height()),
        road_value, buffer_value,
        width);
}

template<typename V, typename C>
void InsertCulvertAlgorithm::extend_roads_with_buffer_region(
    V * road_data,
    unsigned int nx,
    unsigned int ny,
    V road_value,
    V buffer_value,
    double width)
{
    if (width <= 0) return;
    unsigned int uwidth {static_cast<unsigned int>(std::ceil(width))};
    int iwidth {static_cast<int>(uwidth)};
    for (unsigned int j = 0; j < ny; ++j)
    {
        unsigned int jj_min {static_cast<unsigned int>(
            std::max(0, static_cast<int>(j) - iwidth))};
        unsigned int jj_max {static_cast<unsigned int>(
            std::min(static_cast<int>(ny), static_cast<int>(j) + iwidth + 1))};
        for (unsigned int i = 0; i < nx; ++i)
        {
            unsigned int ii_min {static_cast<unsigned int>(
                std::max(0, static_cast<int>(i) - iwidth))};
            unsigned int ii_max {static_cast<unsigned int>(
                std::min(static_cast<int>(nx), static_cast<int>(i) + iwidth + 1))};
            if (road_data[to_raster_index(C {i, j}, nx)] == road_value)
            {
                for (unsigned int jj = jj_min; jj < jj_max; ++jj) {
                    for (unsigned int ii = ii_min; ii < ii_max; ++ii) {
                        if ((pow(static_cast<int>(ii) - static_cast<int>(i), 2) + pow(static_cast<int>(jj) - static_cast<int>(j), 2)) <= (width * width))
                        {
                            auto ind = to_raster_index(C {ii, jj}, nx);
                            if (road_data[ind] != road_value)
                            {
                                road_data[ind] = buffer_value;
                            }
                        }
                    }
                }
            }
        }
    }
}

#endif
