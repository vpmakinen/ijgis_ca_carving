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

#include "GDALRasterCollectionDataSource.h"


namespace io {

    GDALRasterCollectionDataSource::GDALRasterCollectionDataSource():
        RasterDataSource {}
    {
    }

    GDALRasterCollectionDataSource::~GDALRasterCollectionDataSource()
    {
    }

    void GDALRasterCollectionDataSource::add_data_source(RasterDataSource * src)
    {
        // FIXME check here that the data sets are compatible
        sources_.push_back(std::unique_ptr<RasterDataSource>{src});
        if (sources_.size() == 1) {
            geo_area_ = geo::RasterArea {src->raster_area(), src->cell_size()};
        } else {
            geo_area_ = geo::RasterArea{
                minimum_cover(geo_area_, src->raster_area()),
                cell_size()};
        }
    }

    unsigned int GDALRasterCollectionDataSource::data_set_count() const
    {
        return static_cast<unsigned int>(sources_.size());
    }

    void GDALRasterCollectionDataSource::fill_array(
        char * array,
        CellGridFrame & cg,
        GDALDataType value_type) const
    {
        for (const auto & source: sources_)
        {
            source->fill_array(array, cg, value_type);
        }
    }

    bool GDALRasterCollectionDataSource::has_no_data_value() const
    {
        if (sources_.size() == 0) return false;
        return sources_.front()->has_no_data_value();
    }

    double GDALRasterCollectionDataSource::no_data_value() const
    {
        if (!has_no_data_value())
            throw std::runtime_error("The data source does not have "
                "NODATA value.");
        return sources_.front()->no_data_value();
    }

    std::string GDALRasterCollectionDataSource::no_data_value_str() const
    {
        return sources_.front()->no_data_value_str();
    }

    GDALDataType GDALRasterCollectionDataSource::data_type() const
    {
        if (sources_.size() == 0)
            throw std::runtime_error("The GDALRasterCollectionDataSource "
                "has no data sources.");
        return sources_.front()->data_type();
    }

    double GDALRasterCollectionDataSource::cell_size() const
    {
        if (sources_.size() == 0)
            throw std::runtime_error("The GDALRasterCollectionDataSource "
                "has no data sources.");
        return sources_.front()->cell_size();
    }

    geo::Area GDALRasterCollectionDataSource::area() const
    {
        return geo_area_;
    }

    geo::RasterArea GDALRasterCollectionDataSource::raster_area() const
    {
        return geo_area_;
    }

    bool GDALRasterCollectionDataSource::has_data_inside_area(const geo::Area & area) const
    {
        return are_overlapping(area, raster_area());
    }

    geo::PixelTopLeftCoordinate GDALRasterCollectionDataSource::ul_corner() const
    {
        return {raster_area().left(), raster_area().top()};
    }

    geo::ReferenceSystem GDALRasterCollectionDataSource::CRS() const
    {
        if (sources_.size() == 0) {
            throw std::runtime_error("The GDALRasterCollectionDataSource "
                "has no data sets.");
        }
        return sources_.front()->CRS();
    }

}
