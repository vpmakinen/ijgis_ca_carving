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

#ifndef CARVING_ALGORITHM_H_
#define CARVING_ALGORITHM_H_

#include "AbstractAlgorithm.h"
#include "CellGrid.h"
#include "Culvert.h"


template<typename T, typename U, typename V, typename C>
class CarvingAlgorithm: public AbstractAlgorithm
{
    public:
        CarvingAlgorithm();
        virtual ~CarvingAlgorithm() {}

        void execute(
            CellGrid<T, C> & dem,
            CellGrid<U, C> & delta_dem,
            CellGrid<V, C> & flowdir,
            CellGrid<char, C> &,
            std::vector<Culvert<U>> &,
            bool fix_flow_directions = true);
};

#endif
