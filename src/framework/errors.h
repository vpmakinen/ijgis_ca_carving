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

#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>

namespace errors {

    class NotImplementedError: public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    class AlgorithmNotFinishedError: public std::runtime_error {
        public:
            AlgorithmNotFinishedError(const AlgorithmNotFinishedError &) = default;
            AlgorithmNotFinishedError(const std::string &msg):
                std::runtime_error(msg) {}
            ~AlgorithmNotFinishedError();
    };

    class DataSourceCreationError: public std::runtime_error {
        public:
            DataSourceCreationError(const DataSourceCreationError &) = default;
            DataSourceCreationError(const std::string &s):
                std::runtime_error {s}
            {
            }
            ~DataSourceCreationError();
    };

    class CellGridCreationError: public std::runtime_error {
        public:
            CellGridCreationError(const CellGridCreationError &) = default;
            CellGridCreationError(const std::string &s):
                std::runtime_error(s) {}
            ~CellGridCreationError();
    };

    class SignalError: public std::runtime_error
    {
    public:
        SignalError(const SignalError &) = default;
        SignalError(const std::string &s): std::runtime_error(s) {}
        ~SignalError();
    };

}

#endif
