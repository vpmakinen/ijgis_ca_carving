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

#ifndef CMD_OPTS_H_
#define CMD_OPTS_H_

#include <boost/filesystem.hpp>

#include "BaseCmdOpts.h"

#include "geo.h"
#include "coordinates.h"


class ProgramCmdOpts: BaseCmdOpts {
    public:
        ProgramCmdOpts();

        std::string dem_data_str() const { return dem_data_str_; }
        std::string road_data_str() const { return road_data_str_; }

        double halo_width() const {
            return halo_width_; }
        double road_buffer_width() const {
            return road_buffer_; }
        double ignore_dist_same_iter() const {
            return ignore_dist_same_iter_; }
        double ignore_dist() const {
            return ignore_dist_other_; }
        double min_carving_cost_path() const {
            return min_carving_cost_; }
        double min_carving_single() const {
            return min_carving_single_; }
        double min_flow_accum() const {
            return min_flow_accum_; }

        void parse(int argc, char** argv);

    private:
        std::string dem_data_str_;
        std::string road_data_str_;
        double halo_width_;
        double road_buffer_;
        double ignore_dist_same_iter_;
        double ignore_dist_other_;
        double min_carving_cost_;
        double min_carving_single_;
        double min_flow_accum_;
};

#endif
