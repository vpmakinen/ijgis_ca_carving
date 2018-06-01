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

#include "AbstractAlgorithm.h"


AbstractAlgorithm::AbstractAlgorithm(
        const std::string &name)
{
    timerTree = std::make_shared<TimerTree>(name);
    timerComm = timerTree->addChild("comm");
    timerExec = timerTree->addChild("exec");
    timerMisc = timerTree->addChild("misc");
}

AbstractAlgorithm::AbstractAlgorithm(AbstractAlgorithm &&other):
    timerTree {nullptr},
    timerComm {nullptr},
    timerExec {nullptr},
    timerMisc {nullptr}
{
    std::swap(timerTree, other.timerTree);
    std::swap(timerComm, other.timerComm);
    std::swap(timerExec, other.timerExec);
    std::swap(timerMisc, other.timerMisc);
}

AbstractAlgorithm::~AbstractAlgorithm() {
}

std::shared_ptr<TimerTree> AbstractAlgorithm::getTimerTree()
{
    return this->timerTree;
}

std::string AbstractAlgorithm::name() const
{
    return timerTree->descr();
}
