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

#include "program.h"

#include "ProgramCmdOpts.h"

#include "defs.h"

#include "InsertCulvertAlgorithm.h"
#include "insert_culverts_to_road_stream_intersections.h"
#include "insert_culverts_to_expensive_carvings.h"
#include "global_parameters.h"

#include "import_data.h"
#include "write_to_file.h"
#include "vectorize.h"

int program(
    const ProgramCmdOpts & opts)
{
    const std::pair<double, double> culvert_len_lims {
        3.0, 2 * opts.road_buffer_width()};
    try
    {
        auto dem_data_source = io::create_raster_data_source(
            {opts.dem_data_str()});

        auto roads_data_source = io::create_raster_data_source(
            {opts.road_data_str()});

        geo::RasterArea calc_area {dem_data_source->raster_area()};

        geo::RasterArea culvert_insert_area {calc_area};
        culvert_insert_area.add_halo(-opts.halo_width());

        DemClass_t dem_orig {
            calc_area, "DEM"};
        dem_orig.no_data_value(0.0);
        io::fill_array(dem_orig, *dem_data_source);
        // place artifical "dam" at the border where a lake is cut off
        {
            auto pa_ = dem_orig.area();
            // Velskolan pitkäjärvi
            auto c_start = pa_.to_raster_coordinate({369143.5, 6686200.5});
            auto c_stop = pa_.to_raster_coordinate({369374.5, 6686200.5});
            for (auto i = c_start.col(); i < c_stop.col(); ++i) {
                auto ind = coordinates::to_raster_index(i, c_start.row(), dem_orig.px_width());
                dem_orig.data()[ind] = std::max(
                    dem_orig.data()[ind],
                    static_cast<DemDataType>(60.0));
            }
            // Saarijärvi
            c_start = pa_.to_raster_coordinate({367400.5, 6690760.5});
            c_stop = pa_.to_raster_coordinate({367400.5, 6689070.5});
            for (auto i = c_start.row(); i < c_stop.row(); ++i) {
                auto ind = coordinates::to_raster_index(c_start.col(), i, dem_orig.px_width());
                dem_orig.data()[ind] = std::max(
                    dem_orig.data()[ind],
                    static_cast<DemDataType>(80.0));
            }
            // Lepsämänjoki wrong direction
            c_start = pa_.to_raster_coordinate({369161.5, 6694299.5});
            c_stop = pa_.to_raster_coordinate({369242.5, 6694299.5});
            for (auto i = c_start.col(); i < c_stop.col(); ++i) {
                auto ind = coordinates::to_raster_index(i, c_start.row(), dem_orig.px_width());
                dem_orig.data()[ind] = std::max(
                    dem_orig.data()[ind],
                    static_cast<DemDataType>(41.0));
            }
            // Lepsämänjoki wrong direction
            c_start = pa_.to_raster_coordinate({371212.5, 6694299.5});
            c_stop = pa_.to_raster_coordinate({371270.5, 6694299.5});
            for (auto i = c_start.col(); i < c_stop.col(); ++i) {
                auto ind = coordinates::to_raster_index(i, c_start.row(), dem_orig.px_width());
                dem_orig.data()[ind] = std::max(
                    dem_orig.data()[ind],
                    static_cast<DemDataType>(35.0));
            }
            // Luukinjärvi
            c_start = pa_.to_raster_coordinate({372799.5, 6688850.5});
            c_stop = pa_.to_raster_coordinate({372799.5, 6688450.5});
            for (auto i = c_start.row(); i < c_stop.row(); ++i) {
                auto ind = coordinates::to_raster_index(c_start.col(), i, dem_orig.px_width());
                dem_orig.data()[ind] = std::max(
                    dem_orig.data()[ind],
                    static_cast<DemDataType>(47.0));
            }
            // Urja
            c_start = pa_.to_raster_coordinate({367400.5, 6688430.5});
            c_stop = pa_.to_raster_coordinate({367400.5, 6687220.5});
            for (auto i = c_start.row(); i < c_stop.row(); ++i) {
                auto ind = coordinates::to_raster_index(c_start.col(), i, dem_orig.px_width());
                dem_orig.data()[ind] = std::max(
                    dem_orig.data()[ind],
                    static_cast<DemDataType>(65));
            }
        }

        DemClass_t dem_wrk {
            dem_orig, "dem_wrk"};
        dem_wrk.copy_data_from(dem_orig);

        DeltaDem_t delta_dem {
            dem_orig, "d-DEM"};
        delta_dem.no_data_value(0);
        delta_dem.format(0);

        FlowDirClass_t flowdirs {
            dem_orig, "flowdirs"};

        CarvedCells_t carved_cells {
            dem_orig,
            "carved_cells"};
        carved_cells.no_data_value(0);
        carved_cells.format();

        CellGrid<road_id_type, ct> roads {
            dem_orig, "roads"};
        roads.no_data_value(0);

        io::fill_array(roads, *roads_data_source);

        CellGrid<acc_type, ct> accumulated {
            dem_orig,
            "flow_accum"};
        accumulated.no_data_value(0);


        std::vector<Culvert<DeltaDemDatatype>> culverts;
        DeltaDemDatatype next_free_culvert_id {1};
        std::map<DeltaDemDatatype, cprops> culvert_props;
        std::vector<std::string> field_names;
        field_names.push_back("id");
        //field_names.push_back("cost");
        //field_names.push_back("max hdiff");
        field_names.push_back("flow");
        //field_names.push_back("in iter");
        //field_names.push_back("two_way");
        //field_names.push_back("insertmode");

        FlowRoutingAlgorithm_CPU<FlowDirDataType, DemDataType, ct> flow_routing_algorithm;

        InsertCulvertAlgorithm ICA;

        // A function to execute the needed algorithms to generate the flow
        // accumulation and vectorize it
        auto generate_flow_accumulation = [&](const std::string & str)
        {
            delta_dem.format(0);
            ICA.burn_culverts(delta_dem, culverts);

            dem_wrk.copy_data_from(dem_orig);

            carved_cells.format(0);

            CarvingAlgorithm_t carving_algorithm;

            carving_algorithm.execute(
                dem_wrk,
                delta_dem,
                flowdirs,
                carved_cells,
                culverts);

            if (str.size() > 0) {
                io::write_to_file(dem_wrk,
                    std::string("dem_carved_") + str + ".gtiff", "gtiff");
            }

            FlowAccumulationAlgorithm_t flow_accum_algorithm;

            accumulated.format(0);

            flow_accum_algorithm.execute(
                flowdirs,
                accumulated);
        };

        // a function to write the culverts into shapefile
        auto write_culverts = [&](const std::string &filename)
        {
            std::vector<std::pair<
                std::vector<geo::GeoCoordinate>, cprops>> lines;

            for (const auto &c: culverts)
            {
                auto co = accumulated.area().to_raster_coordinate(
                    c.sink());
                std::vector<geo::GeoCoordinate> line;
                line.push_back(c.sink());
                line.push_back(c.source());
                auto & props = culvert_props.at(c.id());
                std::get<1>(props) =
                    accumulated.data()[co.row() * accumulated.px_width() + co.col()];
                lines.push_back({line, culvert_props.at(c.id())});
            }
            io::ESRI_Shapefile_printer pr;
            pr.write_lines(lines, field_names, filename);
        };

        // A function to write the vectorized flow accumulation into a shapefile
        auto write_flow_accum = [&](
            const std::string &filename,
            unsigned int threshold = 1000)
        {
            delta_dem.format(0);
            ICA.burn_culverts(delta_dem, culverts);

            auto lines = vectorize::vectorize_stream_like_raster(
                accumulated,
                flowdirs,
                delta_dem,
                threshold);
            io::write_to_file(lines, filename);
        };

        ICA.extend_roads_with_buffer_region(
            roads,
            static_cast<road_id_type>(1),
            static_cast<road_id_type>(2),
            opts.road_buffer_width());

        ICA.fill_areas_with_unique_id(
            roads,
            static_cast<road_id_type>(2),
            static_cast<road_id_type>(3));

        unsigned int iter {0};
        while (true)
        {
            logging::pLog() << "Starting iteration " << iter;

            generate_flow_accumulation("");

            //{
            //std::stringstream ss;
            //ss << "flow_accum_" << iter << ".shp";
            //write_flow_accum(ss.str());
            //}

            // start the culvert placing procedure
            std::list<Culvert<DeltaDemDatatype>> added_this_iter;
            bool algorithm_exp_carvs_done {false};
            bool algorithm_intersect_done {false};

            {
                insert_culverts_to_expensive_carvings(
                    dem_orig,
                    dem_wrk,
                    flowdirs,
                    roads,
                    culvert_insert_area,
                    culverts,
                    culvert_props,
                    next_free_culvert_id,
                    algorithm_exp_carvs_done,
                    iter,
                    opts.min_carving_cost_path(),
                    culvert_len_lims,
                    opts.min_carving_single(),
                    opts.ignore_dist_same_iter(),
                    opts.ignore_dist());
            }
            if (algorithm_exp_carvs_done)
            {
                insert_culverts_to_stream_road_intersections(
                    dem_orig,
                    flowdirs,
                    delta_dem,
                    dem_wrk,
                    accumulated,
                    roads,
                    culvert_insert_area,
                    next_free_culvert_id,
                    culverts,
                    culvert_props,
                    {1, 2},
                    added_this_iter,
                    algorithm_intersect_done,
                    iter,
                    static_cast<acc_type>(opts.min_flow_accum()),
                    culvert_len_lims,
                    opts.ignore_dist_same_iter(),
                    opts.ignore_dist());
            }

            if (added_this_iter.size() == 0 &&
                algorithm_intersect_done &&
                algorithm_exp_carvs_done)
            {
                break;
            }

            // write the culverts from this iteration into shapefile
            //{
            //std::stringstream ss;
            //ss << "culverts_" << iter << ".shp";
            //write_culverts(ss.str());
            //}

            ++iter;
        }

        // Remove the culverts through which no water is flowing.
        {
            logging::pLog() << "Removing unused culverts...";
            std::vector<Culvert<DeltaDemDatatype>> proper_culverts;
            auto ar = flowdirs.area();
            for (const auto &c: culverts)
            {
                auto gsink = c.sink();
                auto gsource = c.source();
                auto sink = ar.to_raster_coordinate(gsink);
                auto source = ar.to_raster_coordinate(gsource);

                auto fd = flowdirs.value_at(gsink);
                auto source_ = move_coord(sink, {fd.x, fd.y},
                    flowdirs.px_width(),
                    flowdirs.px_height());
                if (source_ != sink && source_ == source) {
                    if (accumulated.value_at(gsink) > 0) {
                        proper_culverts.push_back(c);
                    }
                } else if (c.two_way()) {
                    fd = flowdirs.value_at(gsource);
                    source_ = move_coord(source, {fd.x, fd.y},
                        flowdirs.px_width(),
                        flowdirs.px_height());
                    if (source_ != source && source_ == sink) {
                        if (accumulated.value_at(gsource) > 0)
                        {
                            proper_culverts.push_back(c);
                        }
                    }
                }
            }
            auto n_rejected = culverts.size() - proper_culverts.size();
            if (n_rejected > 0) {
                logging::pLog() << " " << n_rejected
                    << " culverts removed.";
                std::swap(proper_culverts, culverts);
                generate_flow_accumulation("");
            } else {
                logging::pLog() << "  no unused culverts found.";
            }
        }

        logging::pLog() << "The final culvert placing iteration.";
        std::list<Culvert<DeltaDemDatatype>> added_this_iter_;
        bool done_ {false};
        insert_culverts_to_stream_road_intersections(
            dem_orig,
            flowdirs,
            delta_dem,
            dem_wrk,
            accumulated,
            roads,
            culvert_insert_area,
            next_free_culvert_id,
            culverts,
            culvert_props,
            {2},
            added_this_iter_,
            done_,
            iter + 1,
            static_cast<acc_type>(opts.min_flow_accum()),
            culvert_len_lims,
            opts.ignore_dist_same_iter(),
            opts.ignore_dist());

        logging::pLog() << "Added " << added_this_iter_.size() << " culverts.";
        generate_flow_accumulation("final");

        // write final stream network
        write_flow_accum("flow_accum.shp");

        // write final culverts
        write_culverts("culverts.shp");

        return 0;
    } catch (...)
    {
        throw;
    }
}
