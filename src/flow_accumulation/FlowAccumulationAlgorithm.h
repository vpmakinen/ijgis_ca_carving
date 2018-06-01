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

#ifndef FLOW_ACCUMULATION_ALGORITHM_H_
#define FLOW_ACCUMULATION_ALGORITHM_H_

#include "AbstractAlgorithm.h"
#include "CellGrid.h"


template<typename T, typename U, typename C>
class FlowAccumulationAlgorithm: public AbstractAlgorithm
{
    public:

        FlowAccumulationAlgorithm();
        virtual ~FlowAccumulationAlgorithm();

        void execute(
            const CellGrid<T, C> & flowdir,
            CellGrid<U, C> & accum);

    protected:
        void perform_flow_accumulation(
            const CellGrid<T, C> & flowdir,
            CellGrid<U, C> & accum,
            CellGrid<char, C> & n_neighs);
};







/* implementation */





template<typename T, typename U, typename C>
FlowAccumulationAlgorithm<T, U, C>::FlowAccumulationAlgorithm():
    AbstractAlgorithm("Flow accumulation algorithm")
{
}

template<typename T, typename U, typename C>
FlowAccumulationAlgorithm<T, U, C>::~FlowAccumulationAlgorithm()
{
}


template<typename T, typename U, typename C>
void FlowAccumulationAlgorithm<T, U, C>::execute(
        const CellGrid<T, C> & flowdirs,
        CellGrid<U, C> & accumulated)
{
    TimerController tc(timerTree);
    logging::pLog() << "[" << timerTree->descr() << "]";
    logging::LogIndent logIndent;

    accumulated.format(1);

    CellGrid<char, C> n_neighbours {
        flowdirs,
        "flow_accum_n_neighs" };

    perform_flow_accumulation(
        flowdirs,
        accumulated,
        n_neighbours);
}

template<typename T, typename U, typename C>
void FlowAccumulationAlgorithm<T, U, C>::perform_flow_accumulation(
        const CellGrid<T, C> & flowdir,
        CellGrid<U, C> & accumulated,
        CellGrid<char, C> & n_neighs)
{
    using ct = typename C::datatype;
    n_neighs.format(0);

    auto nx = flowdir.px_width();
    auto ny = flowdir.px_height();

    const T * flowdir_data = flowdir.data();
    U * acc_data = accumulated.data();
    char * n_neighs_data = n_neighs.data();

    // Count for each cell, how many neighbors flow into that cell
    for (ct j = 0; j < ny; ++j) {
        for (ct i = 0; i < nx; ++i) {
            C c {i, j};
            const T &fd {flowdir_data[coordinates::to_raster_index(c, nx)]};
            C cn {coordinates::move_coord(c, {fd.x, fd.y}, nx, ny)};
            if (c == cn) continue;
            size_t ind {coordinates::to_raster_index(cn, nx)};
            n_neighs_data[ind] = static_cast<char>(static_cast<int>(n_neighs_data[ind]) + 1);
        }
    }
    std::list<C> process_next;
    for (ct j = 0; j < ny; ++j) {
        for (ct i = 0; i < nx; ++i) {
            C c {i, j};
            if (n_neighs_data[coordinates::to_raster_index(c, nx)] == 0) {
                process_next.push_back(c);
            }
        }
    }
    size_t n_cells {flowdir.px_size()};
    size_t counter {0};
    size_t print_counter {0};
    while (process_next.size() > 0) {
        C c {process_next.front()};
        process_next.pop_front();
        ++counter;
        const T &fd {flowdir_data[coordinates::to_raster_index(c, nx)]};
        C cn {coordinates::move_coord(c, {fd.x, fd.y}, nx, ny)};
        if (c == cn) continue;
        auto ind = coordinates::to_raster_index(cn, nx);
        acc_data[ind] += acc_data[coordinates::to_raster_index(c, nx)];
        n_neighs_data[ind] = static_cast<char>(static_cast<int>(n_neighs_data[ind] - 1));
        if (n_neighs_data[ind] == 0) {
            process_next.push_back(cn);
        }
        if (static_cast<size_t>(std::floor((counter * 10)/ n_cells)) > print_counter)
        {
            logging::pLog() << "processed " << (10 * ++print_counter) << " %";
        }
    }
    logging::pLog() << "processed 100 %";
}

#endif
