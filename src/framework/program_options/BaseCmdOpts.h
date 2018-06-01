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

#ifndef BASE_CMD_OPTS_H_
#define BASE_CMD_OPTS_H_

#include <string>
#include <boost/program_options.hpp>

#include "CmdError.h"


class BaseCmdOpts
{
    private:
        bool log_timestamps_;
        std::string logging_destination;

    protected:
        boost::program_options::variables_map vm;
        boost::program_options::options_description desc;

    public:
        BaseCmdOpts();
        virtual ~BaseCmdOpts();

        virtual void parse(int args, char** argv);
};

#endif
