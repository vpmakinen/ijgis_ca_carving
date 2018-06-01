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

#ifndef CARVING_ALGORITHM_IMPL_H_
#define CARVING_ALGORITHM_IMPL_H_

#include "CarvingAlgorithm.h"
#include "LinkedCells.h"
#include "CarvingEngineCPU.h"
#include "FlowRoutingAlgorithmCPU.h"

template<typename T, typename U, typename V, typename C>
CarvingAlgorithm<T, U, V, C>::CarvingAlgorithm():
    AbstractAlgorithm {"Carving"}
{
}

template<typename T, typename U, typename V, typename C>
void CarvingAlgorithm<T, U, V, C>::execute(
        CellGrid<T, C> & dem,
        CellGrid<U, C> & delta_dem,
        CellGrid<V, C> & flowdirs,
        CellGrid<char, C> & carved_cells,
        std::vector<Culvert<U>> & culverts,
        bool fix_flow_directions)
{
    TimerController tc(timerTree);
    logging::pLog() << "[" << timerTree->descr() << "]";
    logging::LogIndent logIndent;

    // create a mapping from the culvert sources to the sinks
    std::map<C, C> raster_culverts;
    const auto & area = dem.area();
    for (const auto & c: culverts) {
        C sink {area.to_raster_coordinate(c.sink())};
        C source {area.to_raster_coordinate(c.source())};
        raster_culverts.insert({source, sink});
        if (c.two_way()) {
            raster_culverts.insert({sink, source});
        }
    }
    auto lc = create_linked_cells(delta_dem);

    CarvingEngineCPU<T, U, V, C> engine;

    engine.perform_carving(
        dem,
        flowdirs,
        carved_cells,
        lc,
        raster_culverts);

    if (fix_flow_directions)
    {
        FlowRoutingAlgorithm_CPU<V, T, C> flow_routing_algorithm;

        flow_routing_algorithm.execute_D8(
            dem,
            carved_cells,
            flowdirs);

        flow_routing_algorithm.assign_border_flowdirs_out(
            flowdirs);
    }
}

#endif
