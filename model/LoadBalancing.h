#ifndef LOAD_BALANCING_ALGO_H
#define LOAD_BALANCING_ALGO_H

#include <vector>
#include <functional>

#include "Node.h"

namespace lba {
/**
 * @brief Round-robin load-balancing algorithm
 * 
 * @param nodeList The list of available Service Nodes to choose from
 * @return int The Service Node chosen
 */
int roundrobin(std::vector<ServiceNode> nodeList, Job job);

/**
 * @brief Random load-balancing algorithm
 *
 * @param nodeList the list of available Service Nodes to choose from
 * @return int the chosen service node
 */
int random(std::vector<ServiceNode> NodeList, Job job);

/**
 * @brief Utilization based load-balancing algorithm
 *
 * @param nodeList the list of available service nodes to choose from
 * @return int the chosen service node
 */
int utilizationbased(std::vector<ServiceNode> nodeList, Job job);

/**
 * @brief Least Connections load-balancing algorithms
 *
 * @param nodeList the list of available service nodes to choose from
 * @return int the chosen service node
 */
int leastconnections(std::vector<ServiceNode> nodeList, Job job);

/**
 * @brief Function to test the currenct implementation of the system
 * 
 * @param nodeList 
 * @return int 
 */
int testLBA(std::vector<ServiceNode> nodeList, Job hob);
}  // namespace lba


#endif
