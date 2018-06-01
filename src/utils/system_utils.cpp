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

#include "system_utils.h"

#include <assert.h>
#include <unistd.h> //gethostname
#include <sstream>

#include "global_parameters.h"
#include "logging.h"

namespace system_utils {

    bool compare_float_exact(const float &f1, const float &f2);
    bool compare_double_exact(const double &f1, const double &f2);



    short short2_to_short(short2);

    bool compare_float_exact(const float &f1, const float &f2)
    {
        static_assert(sizeof(float) == sizeof(int),
            "The function 'compare_float_exact' requires two types of same size.");
        union U {
            float f;
            int i;
        };
        U u1, u2;
        u1.f = f1;
        u2.f = f2;
        return u1.i == u2.i;
    }

    bool compare_double_exact(const double &f1, const double &f2)
    {
        static_assert(sizeof(double) == sizeof(long int),
            "The function 'compare_double_exact' requires two types of same size.");
        union U {
            double f;
            long int i;
        };
        U u1, u2;
        u1.f = f1;
        u2.f = f2;
        return u1.i == u2.i;
    }

    template<>
    bool compare_exact(const int2 &t1, const int2 &t2)
    {
        return t1.x == t2.x && t1.y == t2.y;
    }

    template<>
    bool compare_exact(const float &f1, const float &f2)
    {
        return compare_float_exact(f1, f2);
    }

    template<>
    bool compare_exact(const double &f1, const double &f2)
    {
        return compare_double_exact(f1, f2);
    }

    template<> std::string toString(const short2 &value)
    {
        std::stringstream ss;
        ss << value.x << "," << value.y;
        return ss.str();
    }

    template<> std::string toString(const unsigned char &value)
    {
        std::stringstream ss;
        ss << static_cast<int>(value);
        return ss.str();
    }

    template<> std::string toString(const char &value)
    {
        std::stringstream ss;
        ss << static_cast<int>(value);
        return ss.str();
    }

    template<> int toInt(const schar2 &value)
    {
        int ret = 0;
        ret = value.x + 255 * value.y;
        return ret;
    }

    template<> int toInt(const short2 &value)
    {
        int ret = 0;
        ret = value.x + 65536 * value.y;
        return ret;
    }

    template<> int toInt(const int2 &value)
    {
        logging::pErr() << "int2 -> int conversion not properly implemented.";
        int ret = 0;
        ret = value.x + 65536 * value.y;
        return ret;
    }

    template<> int2 zero_value<int2>()
    {
        return {0, 0};
    }

    std::string constructFullFilePath(
            const std::string& fileName,
            const std::string& filePath,
            const std::string& fileExtension)
    {
        return constructFullFilePath(fileName, filePath,
            fileExtension, false, 0, 0);
    }

    std::string constructFullFilePath(
            const std::string& fileName,
            const std::string& filePath,
            const std::string& fileExtension,
            bool add_part, size_t partX, size_t partY)
    {
        std::stringstream ss;
        if (filePath.length() > 0) {
            ss << filePath << "/";
        }
        ss << fileName;

        if (add_part) {
            ss << "_" << partX << "_" << partY;
        }

        ss << fileExtension;

        return ss.str();
    }

    void openFileForReading(
        std::ifstream* file,
        const std::string& fullFilePath)
    {
        file->open(fullFilePath.c_str(), std::ios_base::in|std::ios_base::binary|std::ios_base::ate);

        if (!file->is_open()) {
            logging::pErr() << "Unable to open file \"" << fullFilePath
                << "\" for reading.";
            throw std::runtime_error("File not found");
        } else {
            #ifdef DEBUG
            if (global_parameters::verbosity >= 3)
                logging::pLog() << "file " << fullFilePath << " opened.";
            #endif
        }
        file->seekg(0, std::ios::beg);
    }

    // TODO: Change so that this function can create directories which don't exist
    void openFileForWriting(
            std::fstream* file,
            const std::string& fullFilePath)
    {
        std::ios_base::openmode mode = std::ios::binary|std::ios::out|std::ios::in|std::ios::trunc;

        file->open(fullFilePath.c_str(), mode);
        if (!file->is_open()) {
            logging::pErr() << "Unable to open file \"" << fullFilePath <<
                "\" for writing.";
            throw std::runtime_error("Could not open file");
        }
    }


    std::string getHostName()
    {
        char hostname[20];
        gethostname(hostname, 20);
        return std::string(hostname);
    }

    bool fileExists(const std::string &filename)
    {
        std::ifstream file(filename);
        return file.good();
    }

    //short short2_to_short(short2 s) {
    //    if (s.x ==  0 && s.y ==  0) return 0;
    //    if (s.x ==  0 && s.y == -1) return 1;
    //    if (s.x == -1 && s.y ==  0) return 2;
    //    if (s.x ==  1 && s.y ==  0) return 3;
    //    if (s.x ==  0 && s.y ==  1) return 4;
    //    if (s.x == -1 && s.y == -1) return 5;
    //    if (s.x ==  1 && s.y == -1) return 6;
    //    if (s.x == -1 && s.y ==  1) return 7;
    //    if (s.x ==  1 && s.y ==  1) return 8;
    //    return 9;
    //}

    //template<>
    //void writeToFile<short2>(
    //        short2* data,
    //        size_t dataSize,
    //        const std::string& fullFilePath)
    //{
    //    std::fstream file;
    //    openFileForWriting(&file, fullFilePath);

    //    auto saip = array_manager::createTrackedArrayInfoSafe(
    //        dataSize, array_manager::StorageType::HOST, "temp write array(short2)");

    //    for (size_t i = 0; i < dataSize; ++i) {
    //        dat[i] = short2_to_short(data[i]);
    //    }
    //    file.seekp(0, std::ios::beg);
    //    file.write(
    //        reinterpret_cast<char*>(dat),
    //        static_cast<std::streamsize>(dataSize * sizeof(short)));
    //    delete[] dat; // FIXME

    //    file.close();
    //}

    //template<>
    //void writeToFile<short2>(
    //        short2* data,
    //        size_t width,
    //        size_t height,
    //        size_t widthPad,
    //        size_t initialOffset,
    //        const std::string& fullFilePath)
    //{
    //    std::fstream file;
    //    openFileForWriting(&file, fullFilePath);

    //    file.seekp(0, std::ios::beg);
    //    short *dat = new short[static_cast<size_t>(width)];
    //    for (size_t row = 0; row < height; ++row) {
    //        for (size_t i = 0; i < width; ++i) {
    //            dat[i] = short2_to_short(data[row * widthPad + initialOffset]);
    //        }
    //        file.write(
    //            reinterpret_cast<char*>(data),
    //            static_cast<std::streamsize>(width * sizeof(short)));
    //    }
    //    delete[] dat;
    //    file.close();
    //}

}
