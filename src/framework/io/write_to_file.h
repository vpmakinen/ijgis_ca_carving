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

#ifndef WRITE_TO_FILE_H_
#define WRITE_TO_FILE_H_

#include "ogrsf_frmts.h"
#include <boost/filesystem.hpp>

#include "GDALRasterPrinter.h"

namespace io
{

    boost::filesystem::path add_to_stem(
        const boost::filesystem::path & p,
        const std::string & s);


    template<typename T = void>
    int pixelbits() {
        static_assert(std::is_void<T>::value == false,
            "Unknown variable to convert to shapefile (pixelbits).");
        return 0;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, int>::type
    get_field_precision() {
        return 6;
    }

    template<typename T>
    typename std::enable_if<!std::is_floating_point<T>::value, int>::type
    get_field_precision() {
        return 0;
    }

    template<typename T, typename = void>
    struct cast_type {
        using value_type = int;
    };

    template<typename T>
    struct cast_type<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
    {
        using value_type = double;
    };

    template<>
    struct cast_type<std::string> {
        using value_type = std::string;
    };

    template<>
    int pixelbits<int>();

    template<>
    int pixelbits<unsigned int>();

    template<>
    int pixelbits<float>();

    template<>
    int pixelbits<double>();

    template<typename value_type = void>
    OGRFieldType get_field_type() {
        static_assert(std::is_void<value_type>::value == false,
            "Unknown variable to convert to shapefile (get_field_type).");
        return OFTReal;
    }

    template<>
    OGRFieldType get_field_type<unsigned int>();

    template<>
    OGRFieldType get_field_type<int>();

    template<>
    OGRFieldType get_field_type<float>();

    template<>
    OGRFieldType get_field_type<double>();

    template<>
    OGRFieldType get_field_type<std::string>();

    class ESRI_Shapefile_printer
    {
        public:
            template<typename T>
            void write(
                const std::vector<std::pair<std::vector<geo::GeoCoordinate>, T>> & lines,
                const boost::filesystem::path & file);

            template<typename T>
            void write(
                const std::vector<std::pair<geo::GeoCoordinate, T>> & points,
                const boost::filesystem::path & file);

            template<typename ...Args>
            void write_lines(
                const std::vector<std::pair<
                    std::vector<geo::GeoCoordinate>,
                    std::tuple<Args...> > > & lines,
                const std::vector<std::string> & field_names,
                const boost::filesystem::path & file);

            template<size_t I = 0, typename ...Args>
            typename std::enable_if<I == sizeof...(Args), void>::type
            create_fields(
                OGRLayer * /*poLayer*/, const std::vector<std::string> &, const std::tuple<Args...> &);

            template<size_t I = 0, typename ...Args>
            typename std::enable_if<I < sizeof...(Args), void>::type
            create_fields(
                OGRLayer * poLayer,
                const std::vector<std::string> &names,
                const std::tuple<Args...> &t);

            template<size_t I = 0, typename ...Args>
            typename std::enable_if<I == sizeof...(Args), void>::type
            set_feature_fields(
                OGRFeature * /*poFeature*/,
                const std::vector<std::string> & /*names*/,
                const std::tuple<Args...> & /*t*/);

            template<size_t I = 0, typename ...Args>
            typename std::enable_if<I < sizeof...(Args), void>::type
            set_feature_fields(
                OGRFeature *poFeature,
                const std::vector<std::string> &names,
                const std::tuple<Args...> &t);
    };

    template<typename T>
    void ESRI_Shapefile_printer::write(
            const std::vector<std::pair<std::vector<geo::GeoCoordinate>, T>> & lines,
            const boost::filesystem::path & file)
    {
        const char *pszDriverName = "ESRI Shapefile";
        GDALDriver * poDriver {nullptr};
        poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        if (!poDriver) {
            throw std::runtime_error("Could not create the driver for writing ESRI Shapefile;");
        }
        //std::string full = file.path_ + "/" + file.name_ + file.ext_;
        GDALDataset *poDS {poDriver->Create(file.string().c_str(), 0, 0, 0,
            GDT_Unknown, 0)};
        if (!poDS) {
            throw std::runtime_error("Could not create an ESRI Shapefile dataset");
        }
        OGRLayer *poLayer {poDS->CreateLayer(file.stem().string().c_str(), 0, wkbLineString, 0)};
        if (!poLayer) {
            throw std::runtime_error("Could not create an ESRI Shapefile layer.");
        }
        OGRFieldDefn oField("Category", get_field_type<T>());
        oField.SetWidth(pixelbits<T>());
        oField.SetPrecision(get_field_precision<T>());
        if (poLayer->CreateField(&oField) != OGRERR_NONE) {
            throw std::runtime_error("Could not create a field.");
        }

        for (const auto &line: lines)
        {
            const std::vector<geo::GeoCoordinate> & points {line.first};
            OGRFeature *poFeature {OGRFeature::CreateFeature(poLayer->GetLayerDefn())};
            poFeature->SetField("Category",
                static_cast<typename cast_type<T>::value_type>(line.second));
            OGRLineString ogr_line;
            for (const auto &c: points)
            {
                OGRPoint pt;
                pt.setX(c.x());
                pt.setY(c.y());
                ogr_line.addPoint(&pt);
            }
            poFeature->SetGeometry(&ogr_line);
            if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
                logging::pErr() << "Failed to create a shapefile feature.";
            }
            OGRFeature::DestroyFeature(poFeature);
        }
        GDALClose(poDS);
    }

    template<typename T>
    void ESRI_Shapefile_printer::write(
            const std::vector<std::pair<geo::GeoCoordinate, T>> & points,
            const boost::filesystem::path & file)
    {
        const char *pszDriverName = "ESRI Shapefile";
        GDALDriver * poDriver {nullptr};
        poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        if (!poDriver) {
            throw std::runtime_error("Could not create the driver for writing ESRI Shapefile;");
        }
        GDALDataset *poDS {poDriver->Create(file.string().c_str(), 0, 0, 0,
            GDT_Unknown, 0)};
        if (!poDS) {
            throw std::runtime_error("Could not create an ESRI Shapefile dataset");
        }
        OGRLayer *poLayer {poDS->CreateLayer(file.stem().string().c_str(), 0, wkbPoint, 0)};
        if (!poLayer) {
            throw std::runtime_error("Could not create an ESRI Shapefile layer.");
        }
        OGRFieldDefn oField("Category", get_field_type<T>());
        oField.SetWidth(pixelbits<T>());
        oField.SetPrecision(get_field_precision<T>());
        if (poLayer->CreateField(&oField) != OGRERR_NONE) {
            throw std::runtime_error("Could not create a field.");
        }

        for (const auto &item: points)
        {
            const geo::GeoCoordinate & point {item.first};
            OGRFeature *poFeature {OGRFeature::CreateFeature(poLayer->GetLayerDefn())};
            poFeature->SetField("Category",
                static_cast<typename cast_type<T>::value_type>(item.second));
            OGRPoint ogr_point;
            ogr_point.setX(point.x());
            ogr_point.setY(point.y());
            poFeature->SetGeometry(&ogr_point);
            if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
                logging::pErr() << "Failed to create a shapefile feature.";
            }
            OGRFeature::DestroyFeature(poFeature);
        }
        GDALClose(poDS);
    }

    template<size_t I, typename ...Args>
    typename std::enable_if<I == sizeof...(Args), void>::type
    ESRI_Shapefile_printer::create_fields(
        OGRLayer * /*poLayer*/, const std::vector<std::string> &, const std::tuple<Args...> &)
    {
    }

    template<size_t I, typename ...Args>
    typename std::enable_if<I < sizeof...(Args), void>::type
    ESRI_Shapefile_printer::create_fields(
        OGRLayer * poLayer,
        const std::vector<std::string> &names,
        const std::tuple<Args...> &t)
    {
        using T = typename std::tuple_element<I, std::tuple<Args...>>::type;
        OGRFieldDefn oField(names[I].c_str(), get_field_type<T>());
        oField.SetWidth(pixelbits<T>());
        oField.SetPrecision(get_field_precision<T>());
        if (poLayer->CreateField(&oField) != OGRERR_NONE) {
            throw std::runtime_error("Could not create a field.");
        }
        create_fields<I + 1, Args...>(poLayer, names, t);
    }

    template<typename T>
    void fill_field(OGRFeature &f, int i, const T &t)
    {
        f.SetField(i, static_cast<typename cast_type<T>::value_type>(t));
    }

    template<>
    void fill_field(OGRFeature &f, int i, const std::string &);

    template<size_t I, typename ...Args>
    typename std::enable_if<I == sizeof...(Args), void>::type
    ESRI_Shapefile_printer::set_feature_fields(
        OGRFeature * /*poFeature*/,
        const std::vector<std::string> & /*names*/,
        const std::tuple<Args...> & /*t*/)
    {
    }

    template<size_t I, typename ...Args>
    typename std::enable_if<I < sizeof...(Args), void>::type
    ESRI_Shapefile_printer::set_feature_fields(
        OGRFeature *poFeature,
        const std::vector<std::string> &names,
        const std::tuple<Args...> &t)
    {
        fill_field(*poFeature, static_cast<int>(I), std::get<I>(t));
        set_feature_fields<I + 1, Args...>(poFeature, names, t);
    }

    template<typename ...Args>
    void ESRI_Shapefile_printer::write_lines(
        const std::vector<std::pair<
            std::vector<geo::GeoCoordinate>,
            std::tuple<Args...> > > & lines,
        const std::vector<std::string> & field_names,
        const boost::filesystem::path & file)
    {
        const char *pszDriverName = "ESRI Shapefile";
        GDALDriver * poDriver {nullptr};
        poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        if (!poDriver) {
            throw std::runtime_error("Could not create the driver for writing ESRI Shapefile;");
        }
        GDALDataset *poDS {poDriver->Create(file.string().c_str(), 0, 0, 0,
            GDT_Unknown, 0)};
        if (!poDS) {
            throw std::runtime_error("Could not create an ESRI Shapefile dataset");
        }
        OGRLayer *poLayer {poDS->CreateLayer(file.stem().string().c_str(), 0, wkbLineString, 0)};
        if (!poLayer) {
            throw std::runtime_error("Could not create an ESRI Shapefile layer.");
        }
        create_fields(poLayer, field_names, lines[0].second);

        for (const auto &line: lines)
        {
            const std::vector<geo::GeoCoordinate> & points {line.first};
            OGRFeature *poFeature {OGRFeature::CreateFeature(poLayer->GetLayerDefn())};
            set_feature_fields(poFeature, field_names, line.second);
            OGRLineString ogr_line;
            for (const auto &c: points)
            {
                OGRPoint pt;
                pt.setX(c.x());
                pt.setY(c.y());
                ogr_line.addPoint(&pt);
            }
            poFeature->SetGeometry(&ogr_line);
            if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
                logging::pErr() << "Failed to create a shapefile feature.";
            }
            OGRFeature::DestroyFeature(poFeature);
        }
        GDALClose(poDS);
    }

    template<class CG>
    void write_to_file(
        CG & cg,
        const boost::filesystem::path & file,
        const std::string & format,
        const geo::Area & area_to_write)
    {
        GDAL::write(cg, file, format, area_to_write);
    }

    template<class CG>
    void write_to_file(
        CG & cg,
        const boost::filesystem::path & file,
        const std::string & format)
    {
        write_to_file(cg, file, format, cg.area());
    }

    template<typename T, typename U>
    void write_to_file(
        const std::vector<std::pair<U, T>> & dataset,
        const boost::filesystem::path & file)
    {
        if (file.extension().string() == ".shp") {
            io::ESRI_Shapefile_printer pr;
            pr.write(dataset, file);
        } else {
            throw std::runtime_error("Unknown vector file format.");
        }
    }

}

#endif
