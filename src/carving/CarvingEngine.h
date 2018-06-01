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

#ifndef CARVING_ENGINE_H_
#define CARVING_ENGINE_H_

template<typename T, typename U, typename V, typename C>
class CarvingEngine
{
    public:
        virtual ~CarvingEngine() {}

        virtual void perform_carving(
            CellGrid<T, C> & dem,
            CellGrid<V, C> & flowdir,
            CellGrid<char, C> &,
            const LinkedCells<C> &,
            const std::map<C, C> &) = 0;
};

#endif
