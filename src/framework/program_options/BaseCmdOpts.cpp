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

#include "BaseCmdOpts.h"

#include <boost/filesystem.hpp>

#include "global_parameters.h"
#include "logging.h"


BaseCmdOpts::BaseCmdOpts():
    desc("Allowed options")
{
    namespace po = boost::program_options;

    desc.add_options()
        ("help,h", "Produce help message")
        ("verbosity,v",
                po::value<int>(&global_parameters::verbosity)->default_value(2),
                "Verbosity level\n"
                    "1 = normal\n"
                    "2... more")
        ("log",
                po::value<std::string>(&logging_destination)->default_value("-"),
                "Where to output log messages\n"
                    "\"-\" = stdout (default)\n"
                    "\"null\" = no output\n"
                    "any other string is taken as a filename for the log file.")
        ("log-timestamps",
                po::value<bool>(&log_timestamps_)->default_value(false)->implicit_value(true),
                "Prefix log entries with time")
        ;
}

BaseCmdOpts::~BaseCmdOpts()
{
}

void BaseCmdOpts::parse(int argc, char** argv)
{
    namespace po = boost::program_options;
    // po::store/notify can throw, so set the output to stdout before the
    // wanted location can be read from the comamnd line
    logging::init("-", false);

    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << "Carving with culvert search enabled." << std::endl;
        std::cout << desc << "\n";
        exit(EXIT_SUCCESS);
    }

    if (vm.count("supported-calc-modes")) {
        std::stringstream ss;
        ss << "cpu";
        std::cout << ss.str() << std::endl;
        exit(EXIT_SUCCESS);
    }

    po::notify(vm);

    // Set the logging system
    logging::init(logging_destination, log_timestamps_);
}
