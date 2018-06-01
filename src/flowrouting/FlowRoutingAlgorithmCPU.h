/**
 * Copyright
 *   2014-2015 Finnish Geodetic Institute
 *   2015-     Finnish Geospatial Research Institute,
 *             National Land Survey of Finland
 *
 * Programmers: David Eränen and Ville Mäkinen
 *
 * This file is released under the GNU Lesser General Public
 * Licence version 2.1.
 */

#ifndef FLOWROUTINGALGORITHMCPU_H_
#define FLOWROUTINGALGORITHMCPU_H_

#include "FlowRoutingAlgorithm.h"
#include "FlowRoutingCommon.h"

namespace flowrouting_help {

    template<typename T>
    void get_neig(int n, T &dx, T &dy) {
        switch(n) {
            case 0: dx =  0; dy = -1; break;
            case 1: dx =  1; dy = -1; break;
            case 2: dx =  1; dy =  0; break;
            case 3: dx =  1; dy =  1; break;
            case 4: dx =  0; dy =  1; break;
            case 5: dx = -1; dy =  1; break;
            case 6: dx = -1; dy =  0; break;
            case 7: dx = -1; dy = -1; break;
            default: throw std::runtime_error("Impossible error.");
        }
    }

    template<typename T>
    void get_neig(int n, T &dx, T &dy, double &d) {
        switch(n) {
            case 0: dx =  0; dy = -1; d = 1.0; break;
            case 1: dx =  1; dy = -1; d = sqrt(2.0); break;
            case 2: dx =  1; dy =  0; d = 1.0; break;
            case 3: dx =  1; dy =  1; d = sqrt(2.0); break;
            case 4: dx =  0; dy =  1; d = 1.0; break;
            case 5: dx = -1; dy =  1; d = sqrt(2.0); break;
            case 6: dx = -1; dy =  0; d = 1.0; break;
            case 7: dx = -1; dy = -1; d = sqrt(2.0); break;
            default: throw std::runtime_error("Impossible error.");
        }
    }

    template<typename T>
    void get_neig_closest_first(int n, T & dx, T & dy) {
        switch (n) {
            case 0: dx =  0; dy = -1; break;
            case 1: dx = -1; dy =  0; break;
            case 2: dx =  1; dy =  0; break;
            case 3: dx =  0; dy =  1; break;
            case 4: dx = -1; dy = -1; break;
            case 5: dx =  1; dy = -1; break;
            case 6: dx = -1; dy =  1; break;
            case 7: dx =  1; dy =  1; break;
            default: throw std::runtime_error("Impossible error.");
        }
    }
}





template<typename T, typename U, typename C, typename V = int>
class FlowRoutingAlgorithm_CPU:
    public FlowRoutingAlgorithm<T, U, C, V>
{
    public:
        //using temp_guard = FlowRoutingTempDataGuard<
        //    FlowRoutingAlgorithm_CPU<T, U, C, V>>;
        //friend temp_guard;

        FlowRoutingAlgorithm_CPU():
            FlowRoutingAlgorithm<T, U, C, V> { }
        {
        }

        std::string name() const override {
            return FlowRoutingAlgorithm<T, U, C, V>::name() + "(serial-CPU)";
        }

        void assign_border_flowdirs_out(
            CellGrid<T, C> & flowdirs) override;

        //void performFlowRoutingD8(
        //        CellGrid<T, C>* flowDir,
        //        CellGrid<V, C>*      flatDist,
        //        CellGrid<U, C>*     dem,
        //        U noDataValueDem) override;

        void perform_flow_routing_non_flat_D8(
                const CellGrid<U, C> & dem,
                const CellGrid<char, C> & fixed,
                CellGrid<T, C> & flow_dirs) override;

        std::pair<V, T> flow_dir_D8(
            const U * data,
            const C & c,
            const typename C::datatype & nx,
            const typename C::datatype & ny) override;
};


template<typename T, typename U, typename C, typename V>
std::pair<V, T> FlowRoutingAlgorithm_CPU<T, U, C, V>::flow_dir_D8(
    const U * dem_data,
    const C & coord,
    const typename C::datatype & nx,
    const typename C::datatype & ny)
{
    size_t index {coordinates::to_raster_index(coord, nx)};
    U h {dem_data[index]};
    U steepest {0};
    T steepest_T {0, 0};
    for (int i = 0; i < 8; i++) {
        T dr;
        double d;
        flowrouting_help::get_neig(i, dr.x, dr.y, d);

        C cn = coordinates::move_coord(coord, {dr.x, dr.y}, nx, ny);
        if (cn == coord) continue;

        size_t nIndex {coordinates::to_raster_index(cn, nx)};

        U steepness {static_cast<U>(
            static_cast<double>(h - dem_data[nIndex]) / d)};

        if (steepness > 0.0f) {
            if (steepness > steepest) {
                steepest = steepness;
                steepest_T = dr;
            }
        }
    }
    if (steepest > 0.0f) {
        return {FD_HAS_FLOW_DIR, steepest_T};
    } else {
        return {0, {0, 0}};
    }
}

template<typename T, typename U, typename C, typename V>
void FlowRoutingAlgorithm_CPU<T, U, C, V>::assign_border_flowdirs_out(
    CellGrid<T, C> & flowdirs)
{
    auto nx = flowdirs.px_width();
    auto ny = flowdirs.px_height();
    T * data = flowdirs.data();
    for (unsigned int j = 0; j < ny; ++j)
    {
        for (unsigned int i = 0; i < nx; ++i)
        {
            auto ind = coordinates::to_raster_index(i, j, nx);
            if (i == 0) data[ind] = {-1, 0};
            else if (i == nx - 1) data[ind] = {1, 0};
            else if (j == 0) data[ind] = {0, -1};
            else if (j == ny - 1) data[ind] = {0, 1};
        }
    }
    data[0] = {-1, -1};
    data[nx - 1] = {1, -1};
    data[coordinates::to_raster_index(0, ny - 1, nx)] = {-1, 1};
    data[coordinates::to_raster_index(nx - 1, ny - 1, nx)] = {1, 1};
}

//template<typename T, typename U, typename C, typename V>
//void FlowRoutingAlgorithm_CPU<T, U, C, V>::performFlowRoutingD8(
//        CellGrid<T, C> * flowDir,
//        CellGrid<V, C> * flatDist,
//        CellGrid<U, C> * dem,
//        U /*noDataValueDem*/)
//{
//    U * dem_data = dem->data();
//    T * flowDir_data = flowDir->data();
//    V * flatDist_data = flatDist->data();
//
//    if (global_parameters::dryRun) return;
//
//    for (typename C::datatype y = 0; y < dem->px_height(); ++y) {
//        for (typename C::datatype x = 0; x < dem->px_width(); ++x) {
//            // FIXME no need to initialize to zero, does it matter speedwise?
//            //size_t index = y * dem->local_width() + x;
//            size_t index {coordinates::to_raster_index(x, y, dem->px_width())};
//            if (dem_data[index] <= 0) {
//                flowDir_data[index] = {0, 0};
//                //if (dem_data[index] == 0) {
//                if (system_utils::compare_exact(dem_data[index],
//                                                system_utils::zero_value<U>()))
//                {
//                    flatDist_data[index] = FD_STREAM;
//                } else {
//                    flatDist_data[index] = FD_OUTSIDE;
//                }
//                continue;
//            }
//            auto ret = flow_dir_D8(dem_data, {x, y}, dem->px_width(),
//                dem->px_height());
//            if (ret.first == FD_HAS_FLOW_DIR) {
//                flowDir_data[index] = ret.second;
//                flatDist_data[index] = ret.first;
//            }
//        }
//    }
//}

template<typename T, typename U, typename C, typename V>
void FlowRoutingAlgorithm_CPU<T, U, C, V>::perform_flow_routing_non_flat_D8(
        const CellGrid<U, C> & dem,
        const CellGrid<char, C> & fixed,
        CellGrid<T, C> & flow_dirs)
{
    using ct = coordinates::raster_coord_type;

    const char * fixed_data {fixed.data()};
    T * flowdir_data {flow_dirs.data()};

    if (global_parameters::dryRun) return;

    ct nx {dem.px_width()};
    ct ny {dem.px_height()};
    // FIXME check that these are separate for each openMP-thread
    for (ct y = 0; y < ny; ++y) {
        for (ct x = 0; x < nx; ++x) {
            C c {x, y};
            //size_t index = y * nx + x;
            size_t index {coordinates::to_raster_index(c.col(), c.row(), nx)};
            if (fixed_data[index]) continue;

            auto ret = flow_dir_D8(dem.data(), {x, y}, dem.px_width(),
                dem.px_height());
            if (ret.first == FD_HAS_FLOW_DIR) {
                flowdir_data[index] = ret.second;
            }
        }
    }
}

#endif /* FLOWROUTINGALGORITHMCPU_H_ */
