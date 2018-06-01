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

#include <ogrsf_frmts.h>

#include "ProgramCmdOpts.h"
#include "program.h"
#include "errors.h"
#include "logging.h"

int main(int argc, char** argv)
{
    try {
        GDALAllRegister();

        ProgramCmdOpts opts;

        opts.parse(argc, argv);

        program(opts);

        return 0;
    }
    catch (errors::CmdError &e)
    {
        logging::pErr() << "Could not parse the command line options: "
            << e.what();

        return 0;
    }
    catch (errors::SignalError &e)
    {
        logging::pErr() << e.what();
        return 0;
    }
    catch (std::runtime_error &e)
    {
        logging::pErr() << "std::runtime_error: " << e.what();
        return 0;
    }
}
