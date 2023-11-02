#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>

#include "Job.h"
#include "LoadBalancing.h"
#include "Node.h"
#include "rngs.h"
#include "rvgs.h"

// Type definition aliases
typedef int node_idx;
typedef std::function<node_idx(std::vector<ServiceNode>, double)> lba_func;
typedef std::vector<ServiceNode> node_list;
typedef int lba_alg;

// GLOBAL VARIABLES
const int DAY_SEC{24 * 60 * 60};   // seconds in a day
const int NOON_TIME{DAY_SEC / 2};  // time of day for noon
const int HOUR_SEC{DAY_SEC / 24};
const double START{0.0};                 // start time for the simulation
const double END{(double)DAY_SEC * 30};  // end time for the simulation
enum class Model { mqms, sqms };         // model enums
std::string alg{""};

// NOTE: surely there must be a better way to deal with the below
const struct algs_t {
  const lba_alg rr{0};
  const lba_alg rand{1};
  const lba_alg util{2};
  const lba_alg cxns{3};
} Algs;
// this is a list that should be able to be indexed using the enumerator
const std::vector<lba_func> LBA_FUNCTIONS = {
    lba::roundrobin, lba::random, lba::utilizationbased, lba::leastconnections};
const std::vector<std::string> LBA_NAMES = {"roundrobin", "random", "utilbased",
                                            "leastcxns"};
// =========================== END GLOBAL VARIABLES ============================

lba_alg name_to_index(std::string name) {
  for (size_t ii = 0; ii < LBA_NAMES.size(); ii++) {
    if (name == LBA_NAMES[ii]) return ii;
  }
  return -1;
}

// tests a load balancing algorithm with 'nodes', 'num_iter' times
void test_lba(std::function<int(std::vector<ServiceNode>)> lba,
              std::vector<ServiceNode> nodes, int num_iters = 25) {
  for (int ii = 0; ii < num_iters; ii++) {
    std::cout << "Job index: " << ii << " | "
              << "Chosen node: " << lba(nodes) << std::endl;
  }
}

// Function declarations
double getArrival();
node_list buildNodeList(int nNodes, size_t qSz);
node_idx dispatcher(node_list nodes, lba_func lba);
void log_sim(lba_func lba, int nNodes, int qSize, int nJobs, node_list nodes);

/* TO-DO:
 * Implement the function declartions below this list....
 */
void sqmsSimulation(int nNodes, lba_alg lba, size_t qSize, int nJobs);
void mqmsSimulation(int nNodes, lba_alg lba, size_t qSize, int nJobs);
void accumStats(node_list nodes, int nJobs, Model modelName,
                std::string funcName);
void serverDistribution(int nNodes, int nJobs);
void log_sim(std::string alg, int nNodes, int qSize, int nJobs,
             node_list nodes);
void printStats(node_list nodes, int totalRejects, int nJobs);

int main(int argc, char* argv[]) {
  // get command line arguments
  if (argc < 5) {
    std::cout << "Usage: " << argv[0] << " ";
    std::cout << "<nNodes> <lba_alg> <qSize> <nJobs> <seed>" << std::endl;
    return 1;
  }

  int nNodes{atoi(argv[1])};  // set the number of nodes

  // set the seed (check that seed was given)
  long int seed{argc < 6 ? 123456789 : atol(argv[5])};


  // pick the user's LBA
  lba_alg lbaChoice{name_to_index(argv[2])};
  if (lbaChoice < 0 || lbaChoice > (int)LBA_FUNCTIONS.size()) {
    std::cerr << "Invalid load balancing algorithm: " << argv[2] << std::endl;
    std::cerr << "Possible choices are: ";
    for (auto choice : LBA_NAMES) std::cout << choice << " ";
    std::cout << std::endl;
    return 1;
  }
  int qSize{atoi(argv[3])};
  int nJobs{atoi(argv[4])};
  std::cout << "Running simulation with: " << nNodes << " Nodes, " << argv[2]
            << " Algorithm, " << qSize << " Queue length, " << nJobs
            << " Jobs, " << seed << " Seed." << std::endl;

  PutSeed(seed);  // seed the RNG

  // testing mqms simulation
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << "MQMS SIMULATION:" << std::endl;
  mqmsSimulation(nNodes, lbaChoice, qSize, nJobs);
  
  // testing sqms simulation
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << "SQMS SIMULATION:" << std::endl;
  sqmsSimulation(nNodes, lbaChoice, qSize, nJobs);
}

// get a service time for a job
double getArrival() {
  static double prevArr{START};     // the previous arrival time
  double st{Uniform(0, HOUR_SEC)};  // choose an arrival time
  prevArr += st;                    // update the the

  return prevArr;
}

/**
 * @brief Build a list of service nodes
 *
 * @param nNodes The number of nodes to include in the model
 * @param qSz The queue size of the nodes
 * @return std::vector<ServiceNode>
 */
node_list buildNodeList(int nNodes, size_t qSz) {
  node_list tempList;

  for (int id = 0; id < nNodes; id++) {
    tempList.push_back(ServiceNode(id, qSz));
  }

  return tempList;
}

/**
 * @brief Pick a service node for the next job to go to
 *
 * dispatcher will choose a node's index to send a job to. However, this will
 * not ignore nodes with a full queue. (I.e., if a job is sent to a full node,
 * that job won't be able to run unless the dispatcher picks a node with space.)
 *
 * @param nodes A list of usable service nodes (that may have full queues)
 * @param lba The load-balancing algorithm to use to choose a node
 * @return int The index of the node to send a job to
 */
int dispatcher(node_list nodes, lba_func alg, double currT) {
  int nodeIdx{-1};              // -1 as no node will have this index
  nodeIdx = alg(nodes, currT);  // pick a node using the LBA

  return nodeIdx;
}

/**
 * @brief log information and utilization results of a simulation run
 *
 *  Call at end of mqmsSimulation and sqmsSimulation
 *
 *
 * @param lba The load-balancing algorithm to use to choose a node
 * @param nNodes The number of nodes to use in the simulation
 * @param qSize The number of jobs allowed in each server's queue
 * @param nJobs The number of jobs to "process" in the simulation
 * @param nodes node_list of nodes to get node utilizations.
 * @return void Writes/ appends to a csv file to log info and utilization
 * results
 */
void log_sim(std::string alg, int nNodes, int qSize, int nJobs,
             node_list nodes) {
  // not sure if this will work, if not can just do if or case/switches to get
  // name of alg std::string alg{std::to_string(lba)};
  std::ofstream logfile;
  logfile.open("simlog.csv", std::ios::app);
  logfile << alg << "," << nJobs << "," << nNodes << "," << qSize;
  for (int i = 0; i < nNodes; i++) {
    logfile << "," << nodes[i].getUtil();
  }
  logfile << ",\n";
  logfile.close();
}

/**
 * @brief Run a multi-queue, multi-server simulation
 *
 * The simulation will generate it's own list of nodes and use the LBA to send
 * nJobs to the nodes in the model.
 *
 * @param nNodes The number of nodes to use in the simulation
 * @param qSize The number of jobs allowed in each server's queue
 * @param lba The node balancing
 * @param nJobs The number of jobs to "process" in the simulation
 */
void mqmsSimulation(int nNodes, lba_alg lba, size_t qSize, int nJobs) {
  // select the algorithm besing used
  lba_func alg{LBA_FUNCTIONS[lba]};
  std::string funcName{LBA_NAMES[lba]};

  // build node list
  node_list nodes{buildNodeList(nNodes, qSize)};

  // track the total number of rejections
  int totalRejects{0};

  // run for the number of jobs
  for (int ii = 0; ii < nJobs; ii++) {
    // get the next jobs arrival
    Job job{getArrival()};

    // determine receiving server based on lba
    int receiver{dispatcher(nodes, alg, job.getArrival())};
    // std::cout << "Node " << receiver << " selected for job" << std::endl;

    // attempt to enter the job into the node
    if (nodes[receiver].enterNode(job)) {
      // node added successfully
      // std::cout << "Job successfully added" << std::endl;
    } else {
      // node unable to be added, this is where different rejection
      // techiniques could be used
      // std::cout << "Job unsuccessfully added" << std::endl;

      ++totalRejects;
    }
  }

  // get simulation results
  printStats(nodes, totalRejects, nJobs);

  // TODO: make this dependent on CLI flag
  // also, need better way to get alg name
  accumStats(nodes, nJobs, Model::mqms, funcName);
  log_sim(funcName, nNodes, qSize, nJobs, nodes);
}

/**
 * @brief Run a single-queue, multi-server simulation
 *
 * The simulation will generate it's own list of nodes and use the LBA to send
 * nJobs to the nodes in the model.
 *
 * @param nNodes The number of nodes to use in the simulation
 * @param lba The node balancing
 * @param qSize The size of the dispatcher's queue
 * @param nJobs The number of jobs to "process" in the simulation
 */
void sqmsSimulation(int nNodes, lba_alg lba, size_t qSize, int nJobs) {
  // select the algorithm besing used
  lba_func alg{LBA_FUNCTIONS[lba]};
  std::string funcName{LBA_NAMES[lba]};

  // build node list
  node_list nodes{buildNodeList(nNodes, 0)};

  // the total number of rejections
  int totalRejects{0};

  // the dispatcher's queue
  std::queue<Job> jobQueue;

  // run for the number of jobs
  for (int ii = 0; ii < nJobs; ii++) {
    Job job{getArrival()};  // get a job's arrival time

    // check to make sure the job can be queued
    if (jobQueue.size() < qSize) {
      jobQueue.push(job);  // the job is able to enter the queue.
    } else {
      ++totalRejects;
    }

    // pick the service node to send the current job to
    int receiver{dispatcher(nodes, alg, job.getArrival())};

    // send the job to the selected node
    if (nodes[receiver].enterNode(job)) {
      jobQueue.pop();  // remove the job from the queue as it can be serviced
    }
  }

  // get simulation results
  printStats(nodes, totalRejects, nJobs);

  accumStats(nodes, nJobs, Model::sqms, funcName);
  log_sim(funcName, nNodes, 0, nJobs, nodes);
}

void printStats(node_list nodes, int totalRejects, int nJobs) {
  // calculate the fraction of rejected jobs
  double rejectRatio{(static_cast<double>(totalRejects) / nJobs) * 100};

  // print the reject amount
  std::cout << std::setprecision(5) 
            << "Rejection amount: " << rejectRatio  << "%" << std::endl;
  
  // print the node-wise data
  for (ServiceNode node : nodes) {
    std::cout << node << std::endl;
  }

}

/**
 * @brief Find the distribution of servers for a load-balancing algorithm
 *
 * @param nNodes The number of nodes for the alogrithm to choose from
 * @param nJobs The number of "jobs" to send to the nodes
 */
void serverDistribution(int nNodes, int nJobs) {
  std::cout << "> Testing node choice distribution..." << std::endl;
  node_list nodes{buildNodeList(nNodes, 0)};  // queue size doesn't matter

  // list of available load-balancing algorithms
  std::vector<lba_func> funcs = {lba::roundrobin, lba::random,
                                 lba::utilizationbased, lba::leastconnections};

  std::ofstream lba_dat("lba-data.csv");

  for (lba_func alg : funcs) {
    for (int i = 0; i < nJobs - 1; i++) {
      lba_dat << alg(nodes, 0 /*lbas depend on dynamic state*/) << ",";
    }
    lba_dat << alg(nodes, 0 /*lbas depend on dynamic state*/) << std::endl;
  }

  lba_dat.close();

  std::cout << "> ... done" << std::endl;
}

void accumStats(node_list nodes, int nJobs, Model modelName,
                std::string funcName) {
  std::string model = (modelName == Model::mqms) ? "mqms" : "sqms";
  std::ofstream data(model + "_" + funcName + ".csv");

  // write the headers
  data << "sid,avg_x,avg_s,avg_q,avg_d,n_jobs" << std::endl;

  // will need to get n_jobs
  int nodeId{0};
  for (ServiceNode node : nodes) {
    data << nodeId++ << ","                      // sid
         << node.getUtil() << ","                // avg_x
         << node.calcAvgSt() << ","              // avg_s
         << node.calcAvgQueue() << ","           // avg_q
         << node.calcAvgDelay() << ","           // avg_d
         << node.getNumProcJobs() << std::endl;  // n_jobs
  }

  data.close();
}
