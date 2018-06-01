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

#ifndef SYSTEM_UTILS_H_
#define SYSTEM_UTILS_H_

#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ios>

#include "Short2.h"

namespace system_utils {

    /**
     * \brief Convert the value into string representing the value.
     */
    template<typename T>
    std::string toString(const T &value);

    template<typename T>
    bool compare_exact(const T &t1, const T &t2)
    {
        return t1 == t2;
    }

    template<>
    bool compare_exact(const int2 &t1, const int2 &t2);

    template<>
    bool compare_exact(const float &f1, const float &f2);

    template<>
    bool compare_exact(const double &f1, const double &f2);

    template<> std::string toString<short2>(const short2 &);

    template<typename T>
    int toInt(const T &value)
    {
        return static_cast<int>(value);
    }
    template<> int toInt(const schar2 &);
    template<> int toInt(const short2 &);
    template<> int toInt(const int2 &);

    template<typename T>
    T zero_value() {
        return static_cast<T>(0);
    }

    template<> int2 zero_value<int2>();

    std::string constructFullFilePath(
            const std::string& fileName,
            const std::string& filePath,
            const std::string& fileExtension,
            bool, size_t partX, size_t partY);

    std::string constructFullFilePath(
            const std::string& fileName,
            const std::string& filePath,
            const std::string& fileExtension);

    std::string getHostName();

    bool fileExists(const std::string &);

    void openFileForReading(
        std::ifstream* file,
        const std::string & fullFilePath);

    void openFileForWriting(
        std::fstream* file,
        const std::string & fullFilePath);


    template<typename T>
    void writeToFile(
            const T* data,
            size_t dataSize,
            const std::string& fullFilePath);

    template<>
    void writeToFile<short2>(
            const short2* data,
            size_t dataSize,
            const std::string& fullFilePath);

    template<typename T>
    void writeToFile(
            const T* data,
            size_t width,
            size_t height,
            size_t widthPad,
            size_t initialOffset,
            const std::string& fullFilePath);

    template<>
    void writeToFile<short2>(
            const short2* data,
            size_t width,
            size_t height,
            size_t widthPad,
            size_t initialOffset,
            const std::string& fullFilePath);

    template<typename T>
    void readFromTempFile(
            T* data,
            const std::string& fullFilePath);






    /* Implementations */






    template<typename T>
    std::string toString(const T &value)
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template<typename T>
    void writeToFile(
            const T* data,
            size_t dataSize,
            const std::string& fullFilePath)
    {
        std::fstream file;
        openFileForWriting(&file, fullFilePath);

        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<const char*>(data),
                   static_cast<std::streamsize>(dataSize * sizeof(T)));

        file.close();
    }

    template<typename T>
    void readFromFile(
        T* data,
        size_t dataSize,
        const std::string & fullFilePath)
    {
        std::ifstream file;
        openFileForReading(&file, fullFilePath);

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(data),
                   static_cast<std::streamsize>(dataSize * sizeof(T)));

        file.close();
    }

    template<typename T>
    void readFromFile(
        T * output,
        size_t output_width,
        size_t output_height,
        size_t file_width,
        size_t /*file_height*/,
        size_t offset_x,
        size_t offset_y,
        const std::string & full_file_path)
    {
        std::ifstream file;
        openFileForReading(&file, full_file_path);

        size_t base_offset = offset_y * file_width + offset_x;
        for (size_t row = 0; row < output_height; ++row)
        {
            file.seekg(
                static_cast<std::streamoff>((base_offset + row * file_width) * sizeof(T)),
                std::ios::beg);
            file.read(
                reinterpret_cast<char*>(output + row * output_width),
                static_cast<std::streamsize>(output_width * sizeof(T)));
        }
     }

    template<typename T>
    void writeToFile(
            const T* data,
            size_t width,
            size_t height,
            size_t widthPad,
            size_t initialOffset,
            const std::string& fullFilePath)
    {
        std::fstream file;
        openFileForWriting(&file, fullFilePath);

        file.seekp(0, std::ios::beg);
        for (size_t row = 0; row < height; ++row) {
            file.write(
                reinterpret_cast<const char*>(data + row * widthPad + initialOffset),
                static_cast<std::streamsize>(width * sizeof(T)));
        }
        file.close();
    }

    template<typename T>
    void readFromTempFile(
            T* data,
            const std::string& fullFilePath)
    {
        std::ifstream file;
        openFileForReading(&file, fullFilePath);

        file.seekg(0, std::ios::end);
        auto fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        file.read(reinterpret_cast<char*>(data), fileSize);

        file.close();
    }

}

#endif
