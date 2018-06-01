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

#include "write_to_file.h"

namespace io {

    boost::filesystem::path add_to_stem(
        const boost::filesystem::path & p,
        const std::string & s)
    {
        auto ret = p.parent_path() / p.stem();
        ret += s;
        ret += p.extension();
        return ret;
    }

    template<>
    OGRFieldType get_field_type<unsigned int>() {
        return OFTInteger;
    }

    template<>
    OGRFieldType get_field_type<int>() {
        return OFTInteger;
    }

    template<>
    OGRFieldType get_field_type<float>() {
        return OFTReal;
    }

    template<>
    OGRFieldType get_field_type<double>() {
        return OFTReal;
    }

    template<>
    OGRFieldType get_field_type<std::string>() {
        return OFTString;
    }

    template<>
    void fill_field(OGRFeature &f, int i, const std::string &s)
    {
        f.SetField(i, s.c_str());
    }

    template<>
    int pixelbits<int>() { return 32; }

    template<>
    int pixelbits<unsigned int>() { return 32; }

    template<>
    int pixelbits<float>() { return 32; }

    template<>
    int pixelbits<double>() { return 64; }

    template<>
    int pixelbits<std::string>() { return 10; }

}
