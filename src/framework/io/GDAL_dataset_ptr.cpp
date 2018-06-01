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

#include "GDAL_dataset_ptr.h"


namespace io {

    namespace GDAL {

        dataset_ptr::dataset_ptr():
            ds_ {nullptr}
        {
        }

        dataset_ptr::~dataset_ptr()
        {
            if (ds_) {
                GDALClose(ds_);
                ds_ = nullptr;
            }
        }

        dataset_ptr::dataset_ptr(dataset_ptr && other):
            ds_ {nullptr}
        {
            std::swap(ds_, other.ds_);
        }

        dataset_ptr & dataset_ptr::operator=(dataset_ptr && other)
        {
            std::swap(ds_, other.ds_);
            return *this;
        }

        void dataset_ptr::reset(GDALDataset * ds)
        {
            if (ds_) {
                GDALClose(ds_);
            }
            ds_ = ds;
        }

        GDALDataset * dataset_ptr::get() {
            return ds_;
        }

        GDALDataset * dataset_ptr::operator->()
        {
            return ds_;
        }

    }

}
