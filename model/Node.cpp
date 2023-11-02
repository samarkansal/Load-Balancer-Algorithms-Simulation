#include "Node.h"

#include <iomanip>

#include "Job.h"

ServiceNode::ServiceNode(int id)
    : id{id},
      util{0},
      maxQueueSz{0},
      totST{0},
      numJobsProcessed{0},
      lastDeparture{0.0},
      totDelay{0.0} {}

ServiceNode::ServiceNode(int id, size_t maxQueueSz)
    : id{id},
      util{0},
      maxQueueSz{maxQueueSz},
      totST{0},
      numJobsProcessed{0},
      lastDeparture{0.0},
      totDelay{0.0} {}

void ServiceNode::updateUtil(double mostRecentDep) {
  util = (totST / mostRecentDep);
}

bool ServiceNode::enterQueue(Job& job) {
  if (jobQueue.size() < maxQueueSz) {
    job.setDelay(lastDeparture);
    jobQueue.push(job);

    // process the existing queue now that a new arrival has come
    // problem, how to handle case where queue is full
    // - can't add new job to queue so the processQueue() function won't
    //   be able to see the "current" time

    return true;
  }

  return false;
}

bool ServiceNode::enterNode(Job job) {
  double currArrival{job.getArrival()};
  if (maxQueueSz > 0) {
    processQueue(currArrival);
  }

  bool isEnter{false};

  if (job.getArrival() < serviceDeparture) {
    // the server is busy and there's no queue, so job can't wait here
    if (maxQueueSz == 0) {
      return isEnter;
    }

    bool isInQueue{enterQueue(job)};
    isEnter = isInQueue;  // if true, the job was able to enter the queue
  }

  // empty queue, job can queue (for both cases)
  // bitwise OR to ensure that both cases are covered
  // (w/ and w/o queueueus)
  isEnter |= (jobQueue.size() == 0);
  // job can enter, so update stats
  if (isEnter) {
    ++numJobsProcessed;                 // this Job can be processed
    updateTotST(job.getServiceTime());  // increase the total ST
    updateUtil(job.calcDeparture());    // update utilization
    totDelay += job.getDelay();         // update the delay.
    // update the last Job's departure time
    lastDeparture = job.calcDeparture();

    // if the queue is empty, this job will depart first
    if (jobQueue.size() == 0) {
      serviceDeparture = job.calcDeparture();
    }
  }

  // return the result of this expression
  // If work-conserve, then the server can start work immidiately.
  return (isEnter);
}

void ServiceNode::processQueue(double currArrival) {
  if (jobQueue.size() > 0) {
    // get the most recent arrival time (back of queue)
    // double currentTime{jobQueue.back().getArrival()};
    // if the job in the server is still being processed
    if (currArrival > serviceDeparture) {
      while (!jobQueue.empty()) {
        // if front of queue has already departed
        if (jobQueue.front().calcDeparture() < currArrival) {
          // insert calculations here maybe?

          // remove job from queue
          jobQueue.pop();
        } else {
          break;
        }
      }
      // update job in servers departure time
      if (!jobQueue.empty()) {
        // first job in what's left is being processed in server
        serviceDeparture = jobQueue.front().calcDeparture();
        jobQueue.pop();
      }
    }
  }
}

int ServiceNode::getId() const { return id; }

double ServiceNode::getUtil() const { return util; }

int ServiceNode::getQueueLength() const { return jobQueue.size(); }

double ServiceNode::calcAvgSt() const { return totST / numJobsProcessed; }

double ServiceNode::updateTotST(double lastST) {
  totST = totST + lastST;
  return totST;
}

int ServiceNode::getNumProcJobs() const { return numJobsProcessed; }

double ServiceNode::calcAvgQueue() const {
  double avgQ{0};
  if (numJobsProcessed > 0) {
    avgQ = totDelay / lastDeparture;
  }
  return avgQ;
}

double ServiceNode::calcUtil(Job job) {
  double departure{job.calcDeparture()};  // the jobs departure time
  double st{job.getServiceTime()};        // the jobs service time

  // calculate a temporary average service time
  double tempSt{(totST + st)};

  // calculate a temporary utilization
  double tempUtil{tempSt / departure};

  return tempUtil;
}

double ServiceNode::calcAvgDelay() const { return totDelay / numJobsProcessed; }

int ServiceNode::getMaxQueueLen() const { return maxQueueSz; }

std::ostream& operator<<(std::ostream& out, const ServiceNode& node) {
  // choose to print the delay and queue length.
  if (node.getMaxQueueLen() > 0) {
    out << "ID: " << std::setw(2) << node.getId() << ", util: " << std::setw(7)
        << node.getUtil() << ", njobs: " << std::setw(6)
        << node.getNumProcJobs() << ", avg_s: " << std::setw(6)
        << node.calcAvgSt() << ", avg_q: " << std::setw(4)
        << node.calcAvgQueue() << ", avg_d: " << std::setw(6)
        << node.calcAvgDelay();
  } else { // don't print delay and queue len if there's no queue
    out << "ID: " << std::setw(2) << node.getId() << ", util: " << std::setw(7)
        << node.getUtil() << ", njobs: " << std::setw(6)
        << node.getNumProcJobs() << ", avg_s: " << std::setw(6)
        << node.calcAvgSt();
  }

  return out;
}