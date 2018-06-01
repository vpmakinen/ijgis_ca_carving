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

#include "GDALRasterDataSource.h"

#include <gdal.h>

#include "GDAL_dataset_ptr.h"
#include "GDAL_help.h"
#include "global_parameters.h"
#include "errors.h"
#include "logging.h"

namespace io {

    class GDAL_file_list {
        public:
            GDAL_file_list(GDALDataset * ds)
            {
                file_list_ = ds->GetFileList();
            }

            ~GDAL_file_list()
            {
                if (file_list_) {
                    CSLDestroy(file_list_);
                    file_list_ = nullptr;
                }
            }

            void add_to_list(std::vector<std::string> & list)
            {
                if (file_list_) {
                    unsigned int i {0};
                    while (true) {
                        const char * p {file_list_[i]};
                        if (!p) break;
                        list.push_back({p});
                        ++i;
                    }
                }
            }

        private:
            char ** file_list_;
    };

    GDALRasterDataSource::GDALRasterDataSource(
            const boost::filesystem::path & path):
        RasterDataSource {},
        name_ {path.string()}
    {
        GDAL::dataset_ptr ds;
        ds.reset(static_cast<GDALDataset*>(
            GDALOpenEx(
                path.string().c_str(),
                GDAL_OF_RASTER | GDAL_OF_READONLY,
                nullptr,
                nullptr,
                nullptr)));
        if (global_parameters::verbosity > 2)
            logging::pLog() << "Opening file '" << path << "' for reading data.";
        if (!ds.get()) {
            std::stringstream ss;
            ss << "no GDAL dataset created.";
            throw errors::DataSourceCreationError(ss.str());
        }

        double geotransform[6];
        if (ds->GetGeoTransform(geotransform) != CE_None)
            throw std::runtime_error("Error reading the geotransformation.");
        if (std::abs(std::abs(geotransform[1]) - std::abs(geotransform[5])) > 1e-5)
            throw std::runtime_error("different pixels sizes in x and y directions.");
        cell_size_ = geotransform[1];
        geo_area_ = geo::RasterArea {
            geo::PixelTopLeftCoordinate {
                geotransform[0], geotransform[3]},
            coordinates::RasterDims {
                static_cast<unsigned int>(ds->GetRasterXSize()),
                static_cast<unsigned int>(ds->GetRasterYSize())},
            cell_size_,
            {ds->GetProjectionRef()}}; // FIXME can this fail?
        auto * band = ds->GetRasterBand(1);
        if (!band)
            throw std::runtime_error("GetRasterBand failed.");
        no_data_value_ = band->GetNoDataValue(reinterpret_cast<int*>(&has_no_data_value_));
        data_type_ = band->GetRasterDataType();
        GDAL_file_list file_list {ds.get()};
        file_list.add_to_list(file_list_);
    }

    GDALRasterDataSource::~GDALRasterDataSource()
    {
    }

    void GDALRasterDataSource::fill_array(
        char * array,
        CellGridFrame & cg,
        GDALDataType value_type) const
    {
        if (!geo::are_overlapping(cg.area(), geo_area_))
            return;
        GDAL::dataset_ptr ds;
        ds.reset(static_cast<GDALDataset*>(
            GDALOpenEx(
                name_.c_str(),
                GDAL_OF_RASTER | GDAL_OF_READONLY,
                nullptr,
                nullptr,
                nullptr)));
        auto * band = ds->GetRasterBand(1);
        GDAL::array_file_rw_(array, cg.area(),
            cg.area(), value_type, band, geo_area_,
            GDAL::RW_MODE::READ);
    }

    bool GDALRasterDataSource::has_no_data_value() const
    {
        return has_no_data_value_;
    }

    double GDALRasterDataSource::no_data_value() const
    {
        return no_data_value_;
    }

    std::string GDALRasterDataSource::no_data_value_str() const
    {
        return no_data_value_str_;
    }

    GDALDataType GDALRasterDataSource::data_type() const
    {
        return data_type_;
    }

    double GDALRasterDataSource::cell_size() const
    {
        return cell_size_;
    }

    geo::Area GDALRasterDataSource::area() const
    {
        return geo_area_;
    }

    geo::RasterArea GDALRasterDataSource::raster_area() const
    {
        return geo_area_;
    }

    std::vector<std::string> GDALRasterDataSource::file_list() const
    {
        return file_list_;
    }

    bool GDALRasterDataSource::has_data_inside_area(const geo::Area & area) const
    {
        return are_overlapping(area, raster_area());
    }

    geo::PixelTopLeftCoordinate GDALRasterDataSource::ul_corner() const
    {
        return {raster_area().left(), raster_area().top()};
    }

    geo::ReferenceSystem GDALRasterDataSource::CRS() const
    {
        return geo_area_.CRS();
    }

}
