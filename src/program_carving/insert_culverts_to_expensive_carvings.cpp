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

#include "insert_culverts_to_expensive_carvings.h"

#include "InsertCulvertAlgorithm.h"
//#include "write_to_file.h"

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
    unsigned int /*iter*/,
    double min_carving_cost,
    std::pair<double, double> culvert_length_limits,
    double min_hdiff,
    double ignore_in_same_iter_inside_radius,
    double ignore_radius)
{
    finished = true;
    InsertCulvertAlgorithm ICA;
    auto exp_carvs = ICA.find_expensive_carvings(
        dem,
        dem_wrk,
        flowdirs,
        min_carving_cost,
        min_hdiff,
        culvert_length_limits.first);

    logging::pLog() << "expensive carvings: " << exp_carvs.size();

    // write the carvings into shape file
    //{
    //    std::vector<std::pair<std::vector<geo::GeoCoordinate>, std::tuple<double, double>>> lines;
    //    for (auto it = exp_carvs.rbegin(); it != exp_carvs.rend(); ++it)
    //    {
    //        std::vector<geo::GeoCoordinate> line;
    //        auto * cg = dem_orig_container.get();
    //        line.push_back(geo::GeoCoordinate(cg->to_geocoordinate(std::get<0>(it->second))));
    //        line.push_back(geo::GeoCoordinate(cg->to_geocoordinate(std::get<1>(it->second))));
    //        auto tup = std::make_tuple(it->first, std::get<2>(it->second));
    //        lines.push_back({line, tup});
    //    }
    //    std::stringstream ss;
    //    ss << "expensive_carvings_" << iter << ".shp";
    //    io::ESRI_Shapefile_printer pr;
    //    std::vector<std::string> field_names;
    //    field_names.push_back("cost");
    //    field_names.push_back("max_hdiff");
    //    pr.write_lines(lines, field_names, ss.str());
    //}

    // upstream points to which a culvert has been tried to insert already
    std::set<ct> tested_cells;
    // The endpoints of the inserted culverts
    std::vector<ct> inserted_endpoints;
    // The endpoints of the inserted culverts in this iteration
    //std::set<ct> inserted_downstreams_this_iter;
    // endpoints of the expensive carvings that end up on the border of the grid
    std::set<ct> used_border_endpoints;
    unsigned int n_skipped {0};
    unsigned int n_inserted {0};
    double ignore_r2 {pow(ignore_in_same_iter_inside_radius, 2)};
    unsigned int n {0};
    unsigned int progress {0};
    for (auto it = exp_carvs.rbegin(); it != exp_carvs.rend(); ++it)
    {
        const DemDataType * dem_data {dem.data()};
        const DemDataType * carved_data {dem_wrk.data()};
        const auto full_cost = it->first;
        double cost {static_cast<double>(full_cost)};
        auto upstream = std::get<0>(it->second);
        const auto downstream = std::get<1>(it->second);
        geo::PixelCenterCoordinate gupstream {dem.to_geocoordinate(upstream)};
        ++n;
        if ((n * 10) / exp_carvs.size() > progress) {
            logging::pLog() << (progress * 10) << " % searched.";
            progress = static_cast<unsigned int>((n * 10) / exp_carvs.size());
        }
        //logging::pLog();
        //logging::pLog() << "expensive carving " << n << "/" <<
        //    exp_carvs.size() << "  " << gupstream << " -> " << gdownstream;

        // Follow the expensive carving from the upstream point
        // downhill and try to find better carving route for each
        // point that is close enough to a road.
        //while (upstream != downstream)
        while (true)
        {
            if (upstream == downstream) {
                break;
            }
            size_t ind_upstream {coordinates::to_raster_index(
                upstream.col(), upstream.row(), roads.px_width())};
            auto fd = flowdirs.data()[ind_upstream];
            bool ignore_point {false};
            bool follow_to_next {false};
            {
                auto c_next = coordinates::move_coord(upstream, {fd.x, fd.y},
                    dem.px_width(), dem.px_height());
                size_t ind_next = coordinates::to_raster_index(
                    c_next.col(), c_next.row(), dem.px_width());
                if (dem_data[ind_next] < dem_data[ind_upstream] &&
                    (cost - static_cast<double>(dem_data[ind_next] - carved_data[ind_next])) >= min_carving_cost) {
                    follow_to_next = true;
                }
            }
            if (tested_cells.count(upstream)) {
                break;
            }
            tested_cells.insert(upstream);
            if (cost < min_carving_cost) {
                break;
            }
            auto road_id = roads.data()[ind_upstream];
            if (road_id < static_cast<road_id_type>(2)) {
                follow_to_next = true;
            }
            // This carving matches the criteria and should be checked.
            // However, there was already a culvert inserted close, so we
            // set finished to false and see if the carving is still
            // present in the next iteration.
            for (const auto &c: inserted_endpoints) {
                if (static_cast<double>((c - upstream).norm_squared()) <= ignore_r2) {
                    ignore_point = true;
                    finished = false;
                    ++n_skipped;
                    break;
                }
            }

            if (ignore_point) break;
            if (!follow_to_next)
            {
                auto clims = culvert_length_limits;
                std::unique_ptr<DemDataType> cost_ptr;
                ct min_c {0, 0};
                ct max_c {0, 0};
                // try to insert a culvert with the sink at the upstream
                auto ret = ICA.find_alternative_carving_near_roads(
                    dem, roads,
                    culvert_insert_area,
                    upstream,
                    full_cost, clims,
                    cost_ptr, min_c, max_c);

                if (ret.second) {
                    // The placing algorithm returned a valid location, now
                    // check that there are no culverts too close to the
                    // given location.
                    bool ok {true};
                    auto a = dem.to_geocoordinate(ret.first);
                    auto b = dem.to_geocoordinate(upstream);
                    for (const auto &cul: culverts) {
                        if (distance_from_segment(cul.sink(), cul.source(), a) < ignore_radius ||
                            distance_from_segment(cul.sink(), cul.source(), b) < ignore_radius) {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok) {
                        continue;
                    }
                    Culvert<DeltaDemDatatype> cul {
                        dem.to_geocoordinate(upstream),
                        dem.to_geocoordinate(ret.first),
                        next_free_culvert_id };
                    culverts.push_back(cul);
                    culvert_props[next_free_culvert_id] = std::make_tuple(
                        next_free_culvert_id,
                        //full_cost,
                        //std::get<2>(it->second),
                        std::numeric_limits<acc_type>::max()
                        //iter,
                        //false,
                        //"expensive carving"
                        );
                    ++n_inserted;
                    //    unsigned int nx_ {max_c.col() - min_c.col() + 1};
                    //    unsigned int ny_ {max_c.row() - min_c.row() + 1};
                    //    std::fstream file;
                    //    std::stringstream ss;
                    //    ss << "window_" << std::setw(3) << std::setfill('_') << next_free_culvert_id;
                    //    file.open(ss.str() + ".bin", std::ios::out|std::ios::binary|std::ios::trunc);
                    //    file.write(reinterpret_cast<char*>(cost_ptr.get()), sizeof(DemDataType) * nx_ * ny_);
                    //    file.close();
                    //    file.open(ss.str() + ".hdr", std::ios::trunc|std::ios::out);
                    //    file << std::fixed << std::setprecision(7);
                    //    auto g = dem->to_geocoordinate(min_c);
                    //    file << "ulxmap " << g.x() << std::endl;
                    //    file << "ulymap " << g.y() << std::endl;
                    //    file << "cellsize 1.0" << std::endl;
                    //    file << "ncols " << nx_ << std::endl;
                    //    file << "nrows " << ny_ << std::endl;
                    //    file << "pixeltype floating" << std::endl;
                    //    file << "nbits " << (sizeof(DemDataType) * 8) << std::endl;
                    //    file << "byteorder lsbfirst" << std::endl;
                    //}
                    ++next_free_culvert_id;
                    finished = false;
                    inserted_endpoints.push_back(upstream);
                    inserted_endpoints.push_back(ret.first);
                    break;
                }
            }
            // the culvert placing failed, follow the flow directions to the
            // next cell, subtract from the cost the difference and try again.

            upstream = coordinates::move_coord(upstream, {fd.x, fd.y},
                dem.px_width(), dem.px_height());
            ind_upstream = coordinates::to_raster_index(
                upstream.col(), upstream.row(), dem.px_width());
            cost -= static_cast<double>(
                dem.data()[ind_upstream] - dem_wrk.data()[ind_upstream]);
            gupstream = dem.to_geocoordinate(upstream);
        }
    }
    logging::pLog() << "100 % searched (inserted " << n_inserted << " culverts, "
        "skipped " << n_skipped << " possible carvings).";
}
