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

#include "CmdError.h"

namespace errors
{
    CmdError::CmdError(const std::string &s):
        std::runtime_error(s)
    {
    }

    CmdError::~CmdError()
    {
    }
}
