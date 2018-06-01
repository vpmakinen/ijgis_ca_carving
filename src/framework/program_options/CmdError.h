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

#ifndef CMD_ERROR_H_
#define CMD_ERROR_H_

#include <stdexcept>

namespace errors {

    class CmdError: public std::runtime_error {
        public:
            CmdError(const CmdError &) = default;
            CmdError(const std::string &);
            ~CmdError();
    };

}

#endif
