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

#ifndef ABSTRACTALGORITHM_H_
#define ABSTRACTALGORITHM_H_

#include <stdexcept>
#include <string>
#include <memory>

#include "TimerTree.h"


/**
 * \brief A class to represent general algorithm that works on 2D uniform grid
 * data that is possibly divided into blocks.
 */
class AbstractAlgorithm {
public:
    AbstractAlgorithm(const std::string &name);
    AbstractAlgorithm(AbstractAlgorithm &&);
    virtual ~AbstractAlgorithm();

    /**
     * \brief Return the algorithm main timer.
     */
    std::shared_ptr<TimerTree> getTimerTree();

    virtual std::string name() const;

protected:
    std::shared_ptr<TimerTree> timerTree;
    std::shared_ptr<TimerTree> timerComm;
    std::shared_ptr<TimerTree> timerExec;
    std::shared_ptr<TimerTree> timerMisc;
};

#endif /* ABSTRACTALGORITHM_H_ */
