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

#ifndef CELLGRIDFRAME_H_
#define CELLGRIDFRAME_H_

#include <string>
#include <string.h> //memset
#include <assert.h>
#include <cstdlib>

#include "directions.h"
#include "coordinates.h"
#include "RasterArea.h"


class CellGridFrame {

    public:
        using raster_coord_type = coordinates::raster_coord_type;

        CellGridFrame() = delete;

        CellGridFrame(
                const CellGridFrame & model,
                const std::string &name);

        CellGridFrame(
                const geo::RasterArea & area,
                const std::string &name);

        virtual ~CellGridFrame();

        /**
         * \brief The width of the grid.
         */
        raster_coord_type px_width() const;
        /**
         * \bried The height of the grid.
         */
        raster_coord_type px_height() const;
        /**
         * \brief The number of cells in the local grid.
         */
        size_t px_size() const;

        geo::PixelCenterCoordinate to_geocoordinate(
            const coordinates::RasterCoordinate &) const;

        size_t to_raster_index(const coordinates::RasterCoordinate &) const;

        geo::RasterArea area() const;
        std::string name() const;

        virtual bool is_allocated() const = 0;

    protected:
        const geo::RasterArea & area_;
        /**
         * \brief An identifies string for the data.
         */
        std::string name_;

};

#endif
