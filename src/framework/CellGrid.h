/**
 * Copyright
 *   2014-2015 Finnish Geodetic Institute
 *   2015-     Finnish Geospatial Research Institute,
 *             National Land Survey of Finland
 *
 * Programmers: David Eränen and Ville Mäkinen
 *
 * This file is released under the GNU Lesser General Public
 * Licence version 2.1.
 */

#ifndef CELLGRID_H_
#define CELLGRID_H_

#include <string>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "CellGridFrame.h"
#include "global_parameters.h"
#include "system_utils.h"
#include "logging.h"
#include "errors.h"
#include "RasterDataSource.h"
#include "GDAL_help.h"


/**
 * \brief A general class to represent a block of 2D grid data.
 */
template<typename T, typename C>
class CellGrid: public CellGridFrame {
public:
    using value_type = T;
    using coord_type = C;

    CellGrid(
            const CellGridFrame &model,
            const std::string &name);

    /**
     * \brief A constructor to create a new CellGrid from scratch.
     */
    CellGrid(
            const geo::RasterArea & area__,
            const std::string &name);

    virtual ~CellGrid();

    /**
     * \brief Return the pointer to the data array. Allocate the array if it
     * is not.
     */
    T* data();
    T const * data() const;
    T value_at(const geo::PixelCenterCoordinate &) const;
    /**
     * \brief Return true if the array is allocated, otherwise false.
     */
    bool is_allocated() const override;
    bool is_formatted() const;

    /**
     * \brief Set all the data values to \a val.
     *
     * Reset the sent and received boundary indicators to zero and initialize
     * the data array with \a val.
     */
    void format(T);
    void format();

    void copy_data_from(const CellGrid<T, C> &);

    void no_data_value(value_type value);
    void unset_no_data_value();
    bool has_no_data_value() const;
    value_type no_data_value() const;
    bool has_special_value(const std::string &) const;
    value_type special_value(const std::string &) const;
    void special_value(const std::string &, value_type val);
    void remove_special_value(const std::string &);

    GDALDataType data_type() const;

    virtual void pre_data_input(const io::RasterDataSource &);

private:
    std::map<std::string, value_type> special_values_;
    std::vector<T> data_;
    bool is_formatted_;
};







template<typename T, typename coord_type>
CellGrid<T, coord_type>::CellGrid(
        const geo::RasterArea & area__,
        const std::string &name__):
            CellGridFrame(area__, name__),
            data_(px_size()),
            is_formatted_ {false}
{
}

template<typename T, typename coord_type>
CellGrid<T, coord_type>::CellGrid(
        const CellGridFrame & model,
        const std::string &name__):
            CellGridFrame(model, name__),
            data_(px_size()),
            is_formatted_ {false}
{
}

template<typename T, typename coord_type>
CellGrid<T, coord_type>::~CellGrid()
{
}


template<typename T, typename coord_type>
bool CellGrid<T, coord_type>::is_allocated() const
{
    return data_.size() > 0;
}

template<typename T, typename coord_type>
bool CellGrid<T, coord_type>::is_formatted() const
{
    return is_formatted_;
}

template<typename T, typename coord_type>
T* CellGrid<T, coord_type>::data()
{
    return data_.data();
}

template<typename T, typename C>
T const * CellGrid<T, C>::data() const
{
    return data_.data();
}

template<typename T, typename C>
T CellGrid<T, C>::value_at(const geo::PixelCenterCoordinate &gc) const
{
    C c {area().to_raster_coordinate(gc)};
    return data()[c.row() * px_width() + c.col()];
}

template<typename T, typename coord_type>
void CellGrid<T, coord_type>::format(T val)
{
    for (auto it = data_.begin(); it != data_.end(); ++it) {
        *it = val;
    }
    is_formatted_ = true;
}

template<typename T, typename coord_type>
void CellGrid<T, coord_type>::format()
{
    if (!has_no_data_value()) {
        std::stringstream ss;
        ss << "Function format() called but the CellGrid '" << name_
            << "' does not have no_data_value set.";
        throw errors::CellGridCreationError(ss.str());
    }
    format(no_data_value());
}

template<typename T, typename C>
void CellGrid<T, C>::copy_data_from(const CellGrid<T, C> &other)
{
    data_ = std::vector<T>(other.data_);
    is_formatted_ = true;
    special_values_ = std::map<std::string, T>(other.special_values_);
}

template<typename T, typename C>
bool CellGrid<T, C>::has_no_data_value() const
{
    return special_values_.count("nodata");
}

template<typename T, typename C>
auto CellGrid<T, C>::no_data_value() const -> value_type
{
    if (!has_no_data_value()) {
        std::stringstream ss;
        ss << "CellGrid '" << name_ << "' does not have no_data value set.";
        throw std::runtime_error(ss.str());
    }
    return special_values_.at("nodata");
}

template<typename T, typename C>
bool CellGrid<T, C>::has_special_value(const std::string & key) const
{
    return special_values_.count(key);
}

template<typename T, typename C>
T CellGrid<T, C>::special_value(const std::string & key) const
{
    return special_values_.at(key);
}

template<typename T, typename C>
void CellGrid<T, C>::special_value(const std::string & key, value_type val)
{
    special_values_[key] = val;
}

template<typename T, typename C>
void CellGrid<T, C>::remove_special_value(const std::string & key)
{
    auto it = special_values_.find(key);
    if (it != special_values_.end())
        special_values_.erase(it);
}

template<typename T, typename C>
void CellGrid<T, C>::no_data_value(value_type val)
{
    special_values_["nodata"] = val;
}

template<typename T, typename C>
void CellGrid<T, C>::unset_no_data_value()
{
    remove_special_value("nodata");
}

template<typename T, typename C>
GDALDataType CellGrid<T, C>::data_type() const
{
    return io::GDAL::toGDALDataType<T>();
}

template<typename T, typename C>
void CellGrid<T, C>::pre_data_input(
    const io::RasterDataSource & src)
{
    // FIXME If the CellGrid is filled from more than one source,
    // the array is formatted with the no_data_value for every source
    // and only the last data read is not overwritten.
    if (!has_no_data_value()) {
        if (src.has_no_data_value()) {
            no_data_value(io::GDAL::get_nodata<T>(src));
        } else {
            logging::pErr() << "Warning: Neither the CellGrid "
                << "or the data source has nodata value set.";
        }
    } else {
        if (src.has_no_data_value() &&
            !system_utils::compare_exact(
                no_data_value(),
                io::GDAL::get_nodata<T>(src)))
        {
            throw std::runtime_error("The CellGrid and the data source "
                "have different NODATA values.");
        }
    }
    if (!is_formatted()) {
        if (src.has_no_data_value()) {
            format(io::GDAL::get_nodata<T>(src));
        }
    }
}

#endif /* CELLGRID_H_ */
