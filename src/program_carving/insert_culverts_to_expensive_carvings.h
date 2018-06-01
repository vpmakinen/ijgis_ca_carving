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

#ifndef INSERT_CULVERTS_TO_EXPENSIVE_CARVINGS_H_
#define INSERT_CULVERTS_TO_EXPENSIVE_CARVINGS_H_

#include "defs.h"
#include "Culvert.h"

void insert_culverts_to_expensive_carvings(
    DemClass_t & dem,
    DemClass_t & dem_wrk,
    FlowDirClass_t & flowdirs,
    CellGrid<road_id_type, ct> & roads,
    const geo::RasterArea & culvert_insert_area,
    std::vector<Culvert<DeltaDemDatatype>> & culverts,
    std::map<DeltaDemDatatype, cprops> & culvert_props,
    DeltaDemDatatype & next_free_culvert_id,
    bool & finished,
    unsigned int iter,
    double min_carving_cost,
    std::pair<double, double> culvert_length_limits,
    double min_hdiff,
    double ignore_on_same_iter_radius,
    double ignore_radius);

#endif
