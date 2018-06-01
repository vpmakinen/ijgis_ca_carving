add_library(FlowRoutingAlgorithm INTERFACE)
target_link_libraries(FlowRoutingAlgorithm INTERFACE
    AbstractAlgorithm CellGrid)

add_library(FlowRoutingAlgorithmCPU INTERFACE)
target_link_libraries(FlowRoutingAlgorithmCPU INTERFACE
    FlowRoutingAlgorithm)

add_library(FlowRouting INTERFACE)
target_link_libraries(FlowRouting INTERFACE
    FlowRoutingAlgorithmCPU)
