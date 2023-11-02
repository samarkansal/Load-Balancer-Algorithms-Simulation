#ifndef MY_NODE_H
#define MY_NODE_H

#include <iostream>
#include <queue>

#include "Job.h"

// A service node is both a server and a queue.
// If the service node doesn't have a queue (a la single-queue, multi-server),
// then a service node has a max queue size of 0.
class ServiceNode {
 public:
  /**
   * @brief Construct a new Service Node object with given ID
   *
   * This constructor assumes that the queue size will be nothing (that is no
   * queue). In this case, the model might be single-queue, multi-server.
   *
   * @param id The ID of the Service Node
   */
  ServiceNode(int id);

  /**
   * @brief Construct a new Service Node object with given ID and queue size
   *
   * The Service Node will have a maximum queue size. No elements can be added
   * once the queue reaches that size.
   *
   * @param id The Service Node's ID
   * @param maxQueueSz The maximum size of the Service Node's queue
   */
  ServiceNode(int id, size_t maxQueueSz);

  // ~ServiceNode();

  /**
   * @brief Update the Service Node server's utilization
   *
   * This will update the servers utilization. This will find the current
   * utilization given the number of jobs, the departure time of the last job,
   * and the current running average service time. The formula for this is
   * defined in slide deck 1.2, slide 22.
   *
   * Equation: \Bar{x} = (n/c_n)*\Bar{s}
   * 
   * @param mostRecentDep The most recent entering job's departure time
   */
  void updateUtil(double mostRecentDep);

  /**
   * @brief Add the job to the Service Node's queue, if there's space
   *
   * @param job The job stats
   * @return true The job was successfully added to the queue
   * @return false The queue is full and the job could not be added to the queue
   */
  bool enterQueue(Job& job);

  /**
   * @brief Determine if a job can enter this ServiceNode.
   * 
   * Given a job, determine if this job can actually enter the ServiceNode. If
   * the node is busy, then check if there's space to wait in the queue.
   * 
   * @param job The Job attempting to enter the ServiceNode
   * @return true If a job is able to be worked on here.
   * @return false If this node is full/busy
   */
  bool enterNode(Job job);

  /**
   * @brief Get the Service Node's ID
   *
   * @return int The Service Node's ID
   */
  int getId() const;

  /**
   * @brief Get the Service Node server's utilization
   *
   * @return double The utilization of this service node
   */
  double getUtil() const;

  /**
   * @brief Get the Service Node server's job queue length
   *
   * NOTE: This will have to be updated based on how jobs are processed
   *
   * @return int the number of jobs in the service node's queue
   */
  int getQueueLength() const;

  /**
   * @brief Process the Jobs in the ServiceNode.
   * 
   * This will process the Jobs in the ServiceNode, effectively updating the
   * lastDeparture and jobQueue, as well as other relevant statistics.
   * 
   * @param currArrival The job's arrival time trying to enter the queue
   */
  void processQueue(double currArrival);

  /**
   * @brief Get the number of jobs processed by this node.
   * 
   * @return int The number of jobs processed
   */
  int getNumProcJobs() const;
  
  /**
   * @brief Calculate the average service time.
   * 
   * Calculate the average service time for this node at the "point in time" for
   * when this function is called.
   * 
   * @return double 
   */
  double calcAvgSt() const;

  /**
   * @brief Calculate the average queue length for the Service Node
   * 
   * This uses the following equation: \Bar{q}=(n/c_n)*\Bar{d}
   * This can be simplified to: \Bar{q}=(\Sum{di}/c_n). c_n == the last
   * departure time, i.e. the departure time for the most recently added job in
   * a queue.
   * 
   * @return double The average queue length
   */
  double calcAvgQueue() const;

  /**
   * @brief Calculate the utilization for an arriving job without updating it.
   * 
   * @param job The job to use for the calculation
   * @return double The utilization
   */
  double calcUtil(Job job);

  /**
   * @brief Calculate the average delay for jobs in the node
   * 
   * @return double The average delay
   */
  double calcAvgDelay() const;

  /**
   * @brief Get the maximum queue size for a node
   * 
   * @return int The maxmium queue length
   */
  int getMaxQueueLen() const;

 private:
  /**
   * @brief Calculate the average service time
   * 
   * Calculate the average service time, each time there's a new job added
   * 
   * @param lastST The service time of the most recently pushed Job.
   * 
   * @return double 
   */
  double updateTotST(double lastST);

  // A service nodes indentifying number
  int id;

  // The servers current utilization
  // (could also indicate a Server's "weight")
  double util;

  // The actual queue object
  // FIXME: This will have to change depending on how jobs are actually
  // represented
  std::queue<Job> jobQueue;

  // The maximum number of jobs that can wait in the queue.
  // q.size() <= maxQueueSz
  size_t maxQueueSz;

  // The running average of the service time
  mutable double totST; 

  // The total number of jobs processed over the life time of this ServiceNode
  int numJobsProcessed;

  // The departure time of the last job to enter the server part of this
  // ServiceNode;
  double lastDeparture;

  // departure time of the job being serviced
  double serviceDeparture;

  // the total delay for all the jobs processed
  mutable double totDelay;
};

// overload the << operator
std::ostream& operator<<(std::ostream& out, const ServiceNode& node);

#endif
