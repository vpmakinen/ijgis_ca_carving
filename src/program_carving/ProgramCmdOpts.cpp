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

#include "ProgramCmdOpts.h"

#include "CmdError.h"

ProgramCmdOpts::ProgramCmdOpts():
    BaseCmdOpts()
{
    namespace po = boost::program_options;

    desc.add_options()
        ("dem",
            po::value<std::string>(&dem_data_str_)->default_value(
                "file:data/DEM")->required(),
            "Specify a string identifying the DEM files")
        ("roads",
            po::value<std::string>(&road_data_str_)->required(),
            "Specify a string identifying the roads raster")
        ("halo",
            po::value<double>(&halo_width_)->default_value(0.0),
            "Specify the width of the halo region (in meters).")
        ("road-buffer",
            po::value<double>(&road_buffer_)->required(),
            "The width of the buffer around the road cells in which the "
            "culverts can be placed.")
        ("ignore-dist-same-iter",
            po::value<double>(&ignore_dist_same_iter_)->required(),
            "The minimum distance of a culvert being placed to the other "
            "culverts placed in the same iteration.")
        ("ignore-dist-other",
            po::value<double>(&ignore_dist_other_)->required(),
            "The minimum distance of a culvert being placed to the other "
            "culverts.")
        ("min-carving-cost",
            po::value<double>(&min_carving_cost_)->required(),
            "The minimum carving cost before the carving is analyzed.")
        ("min-single-carving",
            po::value<double>(&min_carving_single_)->required(),
            "The minimum amount of carving on at least one cell in the "
            "carving path.")
        ("min-flow-accumulation",
            po::value<double>(&min_flow_accum_)->required(),
            "The minimum flow accumulation value of a cell before it is "
            "considered to belong to a stream.")
        ;
}

void ProgramCmdOpts::parse(int argc, char** argv)
{
    namespace po = boost::program_options;

    try {
        BaseCmdOpts::parse(argc, argv);

        if (halo_width_ < 0) {
            throw std::runtime_error("The width of the halo region must be positive.");
        }
        if (road_buffer_ < 0) {
            throw std::runtime_error("The param \"road-buffer\" must be positive.");
        }
        if (ignore_dist_same_iter_ < 0) {
            throw std::runtime_error("The param \"ignore-dist-same-iter\" must be positive.");
        }
        if (ignore_dist_other_ < 0) {
            throw std::runtime_error("The param \"ignore-dist-other\" must be positive.");
        }
        if (min_carving_cost_ < 0) {
            throw std::runtime_error("The param \"min-carving-cost\" must be positive.");
        }
        if (min_carving_single_ < 0) {
            throw std::runtime_error("The param \"min-single-carving\" must be positive.");
        }
        if (min_flow_accum_ < 0) {
            throw std::runtime_error("The param \"min-flow-accumulation\" must be positive.");
        }
    } catch (po::error &e) {
        throw errors::CmdError(e.what());
    }
}
