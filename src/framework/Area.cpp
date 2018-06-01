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

#include "Area.h"

#include <iomanip>
#include <sstream>

namespace geo {

    Area::Area(
            const GeoCoordinate &topleft,
            const GeoDims &dims,
            const ReferenceSystem &ref_sys):
        top_left_ {topleft},
        dims_ {dims},
        ref_system_ {ref_sys}
    {
    }

    Area::Area():
        Area {GeoCoordinate {0, 0},
                 GeoDims {0, 0},
                 ReferenceSystem()}
    {
    }

    double Area::left() const
    {
        return top_left_.x();
    }

    double Area::right() const
    {
        return left() + dims_.width();
    }

    double Area::top() const
    {
        return top_left_.y();
    }

    double Area::bottom() const
    {
        return top() - dims_.height();
    }

    GeoCoordinate Area::top_left() const
    {
        return top_left_;
    }

    ReferenceSystem Area::CRS() const
    {
        return ref_system_;
    }

    void Area::CRS(const ReferenceSystem &rs)
    {
        ref_system_ = rs;
    }

    void Area::add_halo(double width)
    {
        if (width < 0)
        {
            if ((right() - left()) < fabs(2 * width) ||
                (top() - bottom()) < fabs(2 * width))
            {
                std::stringstream ss;
                ss << "Cannot subtract a halo region of width " << width <<
                    " from an area of dimensions " << (right() - left()) <<
                    " x " << (top() - bottom()) << ".";
                throw std::runtime_error(ss.str());
            }
        }
        top_left_ = geo::GeoCoordinate {
            top_left_.x() - width,
            top_left_.y() + width };
        dims_ = GeoDims {dims_.width() + 2 * width, dims_.height() + 2 * width};
    }

    double Area::min_accepted_overlap() const {
        return std::numeric_limits<double>::min();
    }

    bool Area::contains_point(const geo::GeoCoordinate &c) const
    {
        if (c.x() < left() || c.x() > right() ||
            c.y() < bottom() || c.y() > top()) return false;
        return true;
    }

    std::vector<std::string> Area::to_vector_str() const
    {
        std::vector<std::string> ret;
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << left();
            ret.push_back(ss.str());
        }
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << top();
            ret.push_back(ss.str());
        }
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << (right() - left());
            ret.push_back(ss.str());
        }
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << (top() - bottom());
            ret.push_back(ss.str());
        }
        return ret;
    }

    Area create_intersection(const Area &a1, const Area &a2)
    {
        Area ret {a1};
        ret.top_left_ = {std::max(a1.left(), a2.left()),
                         std::min(a1.top(), a2.top())};
        double right {std::min(a1.right(), a2.right())};
        double bottom {std::max(a1.bottom(), a2.bottom())};
        ret.dims_ = {right - ret.left(), ret.top() - bottom};
        return ret;
    }

    Area minimum_cover(const Area & a1, const Area & a2)
    {
        Area ret {a1};
        ret.top_left_ = {std::min(a1.left(), a2.left()),
                         std::max(a1.top(), a2.top())};
        double right {std::max(a1.right(), a2.right())};
        double bottom {std::min(a1.bottom(), a2.bottom())};
        ret.dims_ = {right - ret.left(), ret.top() - bottom};
        return ret;
    }

    bool are_overlapping(const Area &a, const Area &b)
    {
        double limit = std::min(a.min_accepted_overlap(), b.min_accepted_overlap());
        if (a.right() - b.left() < limit) return false;
        if (b.right() - a.left() < limit) return false;
        if (b.top() - a.bottom() < limit) return false;
        if (a.top() - b.bottom() < limit) return false;
        return true;
    }

    std::ostream & operator<<(std::ostream &os, const Area &g)
    {
        os << "Area:" << std::endl;
        os << "  (width, height) = (" << g.right() - g.left() << "m x " <<
            g.top() - g.bottom() << "m)" << std::endl;
        os << "  (xmin, xmax, ymin, ymax) = (" << std::fixed << g.left() << ", " <<
            g.right() << ", " << g.bottom() << ", " << g.top() << ")\n";
        return os;
    }

}
