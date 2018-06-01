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

#ifndef GLOBAL_PARAMETERS_H_
#define GLOBAL_PARAMETERS_H_

/**
 * \brief The namespace for global values.
 */
namespace global_parameters {

    /**
     * \brief A parameter to control the output of the program. Bigger value
     * means more output.
     */
    extern int verbosity;

    /**
     * \brief The dry-run mode.
     */
    extern bool dryRun;

}


#endif /* GLOBAL_PARAMETERS_H_ */
