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

#ifndef DEFS_H_
#define DEFS_H_

#include <tuple>

#include "Short2.h"

#include "CarvingAlgorithm_impl.h"
#include "FlowAccumulationAlgorithm.h"

using ct = coordinates::RasterCoordinate;

using DemDataType = float;
using StreamDataType = unsigned short;
using DeltaDemDatatype = unsigned int;
using FlowDirDataType = int2;
using acc_type = unsigned int;
using road_id_type = unsigned short;

//using cprops = std::tuple<DeltaDemDatatype, double, double, acc_type, unsigned int, unsigned int, std::string>;
using cprops = std::tuple<DeltaDemDatatype, acc_type>;
using DemClass_t = CellGrid<DemDataType, ct>;
using StreamClass_t = CellGrid<StreamDataType, ct>;
using DeltaDem_t = CellGrid<DeltaDemDatatype, ct>;
using FlowDirClass_t = CellGrid<FlowDirDataType, ct>;
using CarvedCells_t = CellGrid<char, ct>;

using CarvingAlgorithm_t = CarvingAlgorithm<DemDataType, DeltaDemDatatype, FlowDirDataType, ct>;

using FlowAccumulationAlgorithm_t =
    FlowAccumulationAlgorithm<FlowDirDataType, acc_type, ct>;

#endif
