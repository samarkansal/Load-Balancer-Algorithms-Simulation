#include "LoadBalancing.h"

#include <iostream>

#include "rvgs.h"

int lba::roundrobin(std::vector<ServiceNode> nodeList, Job job) {
  // static counter of jobs
  static int index{0};

  int server = index;

  // update index accounting for node_size
  index++;
  index %= nodeList.size();

  return server;
}

int lba::random(std::vector<ServiceNode> nodeList, Job job) {
  // return a random server index
  return Equilikely(0, nodeList.size() - 1);
}

/**
 * These two algorithms have an issue
 * These algorithms assume that reading the queue length or
 * Utilization retrieves the Nodes "current" status
 * but since it is possible for jobs to have not entered these
 * nodes for a long time, (maybe they are already full) they
 * don'd update their utilization or queue length so they might
 * never be picked again
 */

int lba::utilizationbased(std::vector<ServiceNode> nodeList, Job job) {
  // NOTE: this should still work for both sqms and mqms
  int least_utilized{0};

  // find the index with the least utilization
  for (size_t ii = 0; ii < nodeList.size(); ii++) {
    // process queue
    nodeList[ii].processQueue(job.getArrival());

    // check if this is less utilized
    // NOTE: using calc util seems to make things better i.e. more balanced
    if (nodeList[least_utilized].calcUtil(job) > nodeList[ii].calcUtil(job)) {
      least_utilized = ii;
    }
  }
  return least_utilized;
}

int lba::leastconnections(std::vector<ServiceNode> nodeList, Job job) {
  // NOTE: Not sure how to rework for sqms. Perhaps a condition on the model
  // type could work. Perhaps number of jobs processed by this node?
  int least_connections{0};

  // get the maximum queue lengh for the nodes in the list
  int max_queue_size{nodeList[0].getMaxQueueLen()};

  // pick a node from nodes with a queue
  if (max_queue_size > 0) {
    // find the index with the least number of jobs
    for (size_t ii = 0; ii < nodeList.size(); ii++) {
      nodeList[ii].processQueue(job.getArrival());

      // use the average queue lengths of nodes to determine best node
      // for a job
      if (nodeList[least_connections].calcAvgQueue() >
          nodeList[ii].calcAvgQueue()) {
        least_connections = ii;
      }
    }
  } else {  // pick a node from those without queues
    for (size_t ii = 0; ii < nodeList.size(); ii++) {
      if (nodeList[least_connections].getNumProcJobs() >
          nodeList[ii].getNumProcJobs()) {
        least_connections = ii;
      }
    }
  }

  return least_connections;
}

int lba::testLBA(std::vector<ServiceNode> nodeList, Job job) {
  if (nodeList.size() > 0) {
    for (ServiceNode node : nodeList) {
      std::cout << node << std::endl;
    }
    return nodeList[0].getId();
  }

  return -1;
}
