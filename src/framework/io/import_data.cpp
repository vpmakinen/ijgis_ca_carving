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

#include "import_data.h"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include "GDALRasterDataSource.h"
#include "GDALRasterCollectionDataSource.h"

#include "errors.h"

namespace io {

    std::unique_ptr<RasterDataSource> create_raster_collection_data_source(
        const boost::filesystem::path & path,
        const std::string & regex_expr);

    DataSourceType DataSource::source_type() const
    {
        return source_type_;
    }

    std::unique_ptr<RasterDataSource> create_raster_collection_data_source(
        const boost::filesystem::path & path,
        const std::string & regex_expr)
    {
        std::unique_ptr<RasterDataSource> ret_ptr;
        boost::regex expr {regex_expr};
        std::set<std::string> associated_files;
        ret_ptr = std::make_unique<GDALRasterCollectionDataSource>();
        auto * src = dynamic_cast<GDALRasterCollectionDataSource*>(ret_ptr.get());
        using DI = boost::filesystem::directory_iterator;
        DI end_it;
        for (DI it(path); it != end_it; ++it)
        {
            boost::match_results<std::string::const_iterator> m;
            if (!boost::regex_match(it->path().string(), m, expr)) {
                continue;
            }
            if (associated_files.count(it->path().string()))
                continue;

            try {
                std::unique_ptr<RasterDataSource> ds =
                    std::make_unique<GDALRasterDataSource>(*it);
                auto * p = dynamic_cast<GDALRasterDataSource*>(ds.get());
                auto file_list = p->file_list();
                src->add_data_source(ds.release());
                for (const auto & s: file_list)
                {
                    associated_files.insert(s);
                }
            } catch (std::runtime_error & e) {
                logging::pLog() << "Could not form a dataset from '"
                    << it->path().string() << "': " << e.what();
            }
        }
        if (src->data_set_count() == 0) {
            std::stringstream ss;
            ss << "No raster data sets found from '" << path.string()
                << "'";
            throw std::runtime_error(ss.str());
        }
        return ret_ptr;
    }

    std::unique_ptr<RasterDataSource> create_raster_data_source(
        const boost::filesystem::path & path)
    {
        std::unique_ptr<RasterDataSource> ret_ptr;
        if (path.empty()) return ret_ptr;
        if (boost::filesystem::is_regular_file(path))
        {
            ret_ptr = std::make_unique<GDALRasterDataSource>(path);
            return ret_ptr;
        } else if (boost::starts_with(path.string(), "/vsicurl/")) {
            ret_ptr = std::make_unique<GDALRasterDataSource>(path);
        } else {
            auto par = path.parent_path();
            std::string expr {".*"};
            if (boost::contains(path.filename().string(), "*")) {
                expr = path.filename().string();
            }
            ret_ptr = create_raster_collection_data_source(par, expr);
        }
        if (!ret_ptr) {
            std::stringstream ss;
            ss << "Cannot create a data source from '" << path.string() << "'";
            throw std::runtime_error(ss.str());
        }
        return ret_ptr;
    }

}
