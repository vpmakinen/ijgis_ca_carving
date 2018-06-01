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

#ifndef INSERT_CULVERTS_TO_ROAD_STREAM_INTERSECTIONS_H_
#define INSERT_CULVERTS_TO_ROAD_STREAM_INTERSECTIONS_H_

#include "defs.h"
#include "Culvert.h"

void insert_culverts_to_stream_road_intersections(
    DemClass_t & dem_orig,
    FlowDirClass_t & flowdirs,
    DeltaDem_t & delta_dem,
    DemClass_t & dem_wrk,
    CellGrid<acc_type, ct> & accumulated,
    CellGrid<road_id_type, ct> & roads,
    const geo::RasterArea & culvert_insert_area,
    DeltaDemDatatype & next_free_culvert_id,
    std::vector<Culvert<DeltaDemDatatype>> & culverts,
    std::map<DeltaDemDatatype, cprops> & culvert_props,
    const std::set<int> & use_algs,
    std::list<Culvert<DeltaDemDatatype>> & added_this_iter,
    bool & finished,
    unsigned int iter,
    const acc_type & accum_from_threshold,
    const std::pair<double, double> & culvert_len_lims,
    const double & ignore_radius_same_iter,
    const double & ignore_radius);

#endif
