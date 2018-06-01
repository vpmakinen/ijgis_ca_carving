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

#include "insert_culverts_to_road_stream_intersections.h"

#include "InsertCulvertAlgorithm.h"

#include "global_parameters.h"

void insert_culverts_to_stream_road_intersections(
    DemClass_t & dem_orig,
    FlowDirClass_t & flowdirs,
    DeltaDem_t & delta_dem,
    DemClass_t & dem_wrk,
    CellGrid<acc_type, ct> & acc, // accumulated
    CellGrid<road_id_type, ct> & roads,
    const geo::RasterArea & culvert_insert_area,
    DeltaDemDatatype & next_free_culvert_id,
    std::vector<Culvert<DeltaDemDatatype>> & culverts,
    std::map<DeltaDemDatatype, cprops> & culvert_props,
    const std::set<int> & use_algs,
    std::list<Culvert<DeltaDemDatatype>> & added_this_iter,
    bool & finished,
    unsigned int /*iter*/,
    const acc_type & accum_flow_threshold,
    const std::pair<double, double> & culvert_len_lims,
    const double & ignore_radius_same_iter,
    const double & ignore_radius_any)
{
    InsertCulvertAlgorithm ICA;
    if (!finished)
    {
        finished = true;

        // locate where important streams cross roads
        std::vector<ct> intersections;
        for (unsigned short j = 0; j < acc.px_height(); ++j)
        {
            for (unsigned short i = 0; i < acc.px_width(); ++i)
            {
                if (acc.data()[j * acc.px_width() + i] >= accum_flow_threshold &&
                    roads.data()[j * roads.px_width() + i] == 1)
                {
                    ct rc {i, j};
                    bool skip {false};
                    auto cn = acc.to_geocoordinate(rc);
                    for (auto &c: added_this_iter) {
                        if (distance(cn, c.center()) < ignore_radius_same_iter) {
                            skip = true;
                            break;
                        }
                    }
                    if (skip) continue;
                    intersections.push_back(rc);
                }
            }
        }
        //unsigned int n {0};
        for (auto & rc: intersections)
        {
            //++n;
            //std::string descr("pit_filling");
            std::pair<Culvert<DeltaDemDatatype>, bool> ret {
                {{0, 0}, {0, 0}, 0}, false};
            if (use_algs.find(1) != use_algs.end()) {
                ret = ICA.insert_culvert_pit_fill_upstream(
                    dem_orig,
                    roads,
                    flowdirs,
                    dem_wrk,
                    culvert_insert_area,
                    culvert_len_lims,
                    next_free_culvert_id,
                    rc);
            }

            if (!ret.second && use_algs.find(2) != use_algs.end()) {
                ret = ICA.insert_culvert_along_flow_route(
                    dem_orig,
                    flowdirs,
                    delta_dem,
                    roads,
                    culvert_len_lims,
                    rc,
                    next_free_culvert_id);
                //std::cout << "DEBUG: program done" << std::endl;
                //descr = "along_flow_route";
            }
            if (ret.second) {
                bool too_close {false};
                for (auto &c: culverts) {
                    //if (std::get<5>(culvert_props.at(c.id())) > 1) continue;
                    if (distance(ret.first.center(), c.center()) < ignore_radius_any) {
                        too_close = true;
                        break;
                    }
                }
                if (too_close) continue;

                culverts.push_back(ret.first);
                culvert_props[next_free_culvert_id] = std::make_tuple(
                    next_free_culvert_id,
                    //0.0,
                    //0.0,
                    std::numeric_limits<acc_type>::max()
                    //iter,
                    //0,
                    //"road/stream intersection(" + descr + ")"
                    );
                ++next_free_culvert_id;
                std::vector<Culvert<DeltaDemDatatype>> tmp;
                tmp.push_back(ret.first);
                ICA.burn_culverts(delta_dem, tmp);
                added_this_iter.push_back(ret.first);
                finished = false;
            }
        }
    }
}
