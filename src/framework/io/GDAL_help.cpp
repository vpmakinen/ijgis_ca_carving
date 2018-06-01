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

#include "GDAL_help.h"

#include "RasterArea.h"
#include "system_utils.h"

namespace io {

    namespace GDAL {

        class GDAL_blockref_guard;

        class GDAL_blockref_guard {
            public:
                GDAL_blockref_guard(GDALRasterBand * band, int x_block, int y_block, bool just_init):
                    block_ {nullptr}
                {
                    block_ = static_cast<GDALRasterBlock*>(band->GetLockedBlockRef(x_block, y_block, just_init));
                    if (!block_) {
                        std::stringstream ss;
                        ss << "Failed to load the block (" << x_block << ", " << y_block << ") from the "
                            "data source.";
                        throw std::runtime_error(ss.str());
                    }
                }

                ~GDAL_blockref_guard()
                {
                    if (block_) {
                        block_->DropLock();
                        block_ = nullptr;
                    }
                }

                GDALRasterBlock * operator->()
                {
                    return block_;
                }

            private:
                GDALRasterBlock * block_;

        };

        void array_file_rw_(
            char * array,
            const geo::RasterArea & local_padded_area,
            const geo::RasterArea & active_area,
            GDALDataType value_type,
            GDALRasterBand * band,
            const geo::RasterArea & file_raster_area,
            RW_MODE mode)
        {
            geo::RasterArea ovr = file_raster_area.overlap(active_area)
                .overlap(local_padded_area);
            coordinates::RasterCoordinate file_offset =
                file_raster_area.to_raster_coordinate(ovr.ul_corner());
            coordinates::RasterCoordinate array_offset =
                local_padded_area.to_raster_coordinate(ovr.ul_corner());
            int bsize_x, bsize_y;
            band->GetBlockSize(&bsize_x, &bsize_y);
            const auto bsize_x_u = static_cast<unsigned int>(bsize_x);
            const auto bsize_y_u = static_cast<unsigned int>(bsize_y);
            const int n_blocks_x {
                (band->GetXSize() + bsize_x - 1) / bsize_x};
            const int n_blocks_y {
                (band->GetYSize() + bsize_y - 1) / bsize_y};
            const unsigned int x_block_min {
                file_offset.col() / bsize_x_u};
            const unsigned int y_block_min {
                file_offset.row() / bsize_y_u};

            // Maximum file coordinates (exclusive)
            const unsigned int fx_max {
                file_offset.col() + ovr.pixel_width()};
            const unsigned int fy_max {
                file_offset.row() + ovr.pixel_height()};

            const unsigned int x_block_max {std::min(
                static_cast<unsigned int>(n_blocks_x),
                (fx_max + bsize_x_u - 1) / bsize_x_u)};
            const unsigned int y_block_max {std::min(
                static_cast<unsigned int>(n_blocks_y),
                (fy_max + bsize_y_u - 1) / bsize_y_u)};

            for (unsigned int y_block = y_block_min; y_block < y_block_max; ++y_block) {
                for (unsigned int x_block = x_block_min; x_block < x_block_max; ++x_block) {
                    bool overwrite_old_data {
                        mode == RW_MODE::WRITE &&
                        x_block > x_block_min &&
                        x_block < (x_block_max - 1) &&
                        y_block > y_block_min &&
                        y_block < (y_block_max - 1)};

                    GDAL_blockref_guard block {
                        band,
                        static_cast<int>(x_block),
                        static_cast<int>(y_block),
                        overwrite_old_data};

                    unsigned int bx_min {0};
                    unsigned int by_min {0};

                    // Skip the cells from the beginning of the buffer
                    // that are out of the active area.
                    if (x_block == x_block_min) {
                        bx_min = file_offset.col() % bsize_x_u;
                    }
                    if (y_block == y_block_min) {
                        by_min = file_offset.row() % bsize_y_u;
                    }
                    unsigned int bx_max {bsize_x_u};
                    unsigned int by_max {bsize_y_u};

                    // Again, ignore the area of the block that is outside
                    // of the active area.
                    if (fx_max < ((x_block + 1) * bsize_x_u)) {
                        bx_max -= (x_block + 1) * bsize_x_u - fx_max;
                    }
                    if (fy_max < ((y_block + 1) * bsize_y_u)) {
                        by_max -= (y_block + 1) * bsize_y_u - fy_max;
                    }
                    const unsigned int width {bx_max - bx_min};
                    const unsigned int height {by_max - by_min};

                    // file coordinates
                    const unsigned int fx_min {
                        bx_min + x_block * bsize_x_u};
                    const unsigned int fy_min {
                        by_min + y_block * bsize_y_u};

                    // array coordinates
                    const unsigned int ax_min {
                        fx_min - file_offset.col() + array_offset.col()};
                    const unsigned int ay_min {
                        fy_min - file_offset.row() + array_offset.row()};

                    const auto bytesize {static_cast<unsigned int>(
                        #if GDAL_VERSION_MINOR < 2
                        GDALGetDataTypeSize(value_type) / 8
                        #else
                        GDALGetDataTypeSizeBytes(value_type)
                        #endif
                        )};

                    char * block_data = reinterpret_cast<char*>(
                        block->GetDataRef());
                    for (unsigned int row = 0; row < height; ++row) {
                        size_t arr_off {((ay_min + row) * local_padded_area.pixel_width() + ax_min) * bytesize};
                        size_t b_off {(((by_min + row) * bsize_x_u) + bx_min) * bytesize};
                        size_t w {width * bytesize};
                        if (mode == RW_MODE::READ) {
                            memcpy(array + arr_off, block_data + b_off, w);
                        } else if (mode == RW_MODE::WRITE) {
                            memcpy(block_data + b_off, array + arr_off, w);
                        }
                    }
                    if (mode == RW_MODE::WRITE) {
                        block->MarkDirty();
                    }
                }
            }
        }

        template<> GDALDataType toGDALDataType<char>()
        {
            return GDALDataType::GDT_Byte;
        }

        template<> GDALDataType toGDALDataType<unsigned char>()
        {
            return GDALDataType::GDT_Byte;
        }

        template<> GDALDataType toGDALDataType<short>()
        {
            return GDALDataType::GDT_Int16;
        }

        template<> GDALDataType toGDALDataType<unsigned short>()
        {
            return GDALDataType::GDT_UInt16;
        }

        template<> GDALDataType toGDALDataType<unsigned int>()
        {
            return GDALDataType::GDT_Int32;
        }

        template<> GDALDataType toGDALDataType<int>()
        {
            return GDALDataType::GDT_UInt32;
        }

        template<> GDALDataType toGDALDataType<float>()
        {
            return GDALDataType::GDT_Float32;
        }

        template<> int2 get_nodata(const io::DataSource & ds)
        {
            if (!system_utils::compare_exact(ds.no_data_value(), 0.0))
                throw std::runtime_error("The NODATA value for int2 not understood.");
            return {0, 0};
        }

    }

}
