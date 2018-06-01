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

#ifndef FLOWROUTINGALGORITHM_H_
#define FLOWROUTINGALGORITHM_H_

#include "AbstractAlgorithm.h"
#include "CellGrid.h"

//class FlowRoutingTempDataGuardBase
//{
//    public:
//        virtual ~FlowRoutingTempDataGuardBase();
//};
//
//
//template<class C>
//class FlowRoutingTempDataGuard: public FlowRoutingTempDataGuardBase
//{
//    public:
//        FlowRoutingTempDataGuard(C * flow_routing_algorithm):
//            flow_routing_algorithm_ {flow_routing_algorithm}
//        {
//        }
//
//        virtual ~FlowRoutingTempDataGuard()
//        {
//            flow_routing_algorithm_->release_temp_data();
//        }
//
//    private:
//        C * flow_routing_algorithm_;
//};

template<typename T, typename U, typename C, typename V = int>
class FlowRoutingAlgorithm : public AbstractAlgorithm {
public:
    std::shared_ptr<TimerTree> timerTreeNonFlat;
    std::shared_ptr<TimerTree> timerFlatDirAssign;
    std::shared_ptr<TimerTree> timerTreeFlat;

public:
    FlowRoutingAlgorithm();
    virtual ~FlowRoutingAlgorithm();

    /**
     * \brief Perform flow routing for all the cells expect the fixed ones.
     *
     * \note It is assumed that the non-fixed cells are not flat.
     */
    void execute_D8(
            const CellGrid<U, C> & dem,
            const CellGrid<char, C> & fixed_cells,
            CellGrid<T, C> & flowdirs);


    virtual void assign_border_flowdirs_out(
            CellGrid<T, C> & flowdirs) = 0;

    virtual std::pair<V, T> flow_dir_D8(
        const U * data,
        const C & c,
        const typename C::datatype & nx,
        const typename C::datatype & ny) = 0;

    //virtual void performFlowRoutingD8(
    //        CellGrid<T, C> * flowDir,
    //        CellGrid<V, C> * flatDist,
    //        CellGrid<U, C> * dem,
    //        U noDataValueDem) = 0;

    virtual void perform_flow_routing_non_flat_D8(
            const CellGrid<U, C> & dem,
            const CellGrid<char, C> & fixed,
            CellGrid<T, C> & flowdirs) = 0;
};




/* implementations */





template<typename T, typename U, typename C, typename V>
FlowRoutingAlgorithm<T, U, C, V>::FlowRoutingAlgorithm():
    AbstractAlgorithm {"Flow routing"}
{
    timerTreeNonFlat = std::make_shared<TimerTree>("non-flat");
    timerFlatDirAssign = std::make_shared<TimerTree>("flat dir assign");
    timerTreeFlat = std::make_shared<TimerTree>("flat");
    timerTree->addChild(timerTreeNonFlat);
    timerTree->addChild(timerTreeFlat);
    timerTree->addChild(timerFlatDirAssign);
}

template<typename T, typename U, typename C, typename V>
FlowRoutingAlgorithm<T, U, C, V>::~FlowRoutingAlgorithm() {
}

template<typename T, typename U, typename C, typename V>
void FlowRoutingAlgorithm<T, U, C, V>::execute_D8(
    const CellGrid<U, C> & dem,
    const CellGrid<char, C> & fixed_cells,
    CellGrid<T, C> & flowdirs)
{
    perform_flow_routing_non_flat_D8(
        dem, fixed_cells, flowdirs);
}

#endif /* FLOWROUTINGALGORITHM_H_ */
