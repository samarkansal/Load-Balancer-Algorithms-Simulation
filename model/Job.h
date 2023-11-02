#ifndef JOBS_H
#define JOBS_H

class Job {
 public:
  /**
   * @brief Construct a new Job object
   *
   * Construct a new job with just the given arrival. The service time is set
   * using getService(). Delay is not calculated, as nothing is can be known
   * about the previous delay.
   *
   * @param arrival The time when a job arrives
   */
  Job(double arrival);

  /**
   * @brief Construct a new Job object
   *
   * Construct a new Job with the given arrival and previous Job's departure
   * time. The service time is set from getService() and the delay is
   * calculated from the previous departure time.
   *
   * @param arrival The time when a job arrives.
   * @param prevDeparture The previous Job's departure time.
   */
  Job(double arrival, double prevDeparture);

  /**
   * @brief Set the Delay object
   *
   * If the object is constructed with a dparture time for the previous Job,
   * then the delay in a service node can be calculated.
   *
   * @param prevDeparture The previous Job's departure.
   * @return double The calculated delay in the model.
   */
  double setDelay(double prevDeparture);

  /**
   * @brief Calculate the jobs departure time.
   *
   * Calculate the Job's
   *
   * @return double
   */
  double calcDeparture();

  /**
   * @brief Get the Job's service time.
   * 
   * @return double The service time of this Job.
   */
  double getServiceTime() const;

  /**
   * @brief Get the Job's arrival time
   * 
   * @return double The Job's arrival time
   */
  double getArrival() const;

  /**
   * @brief Get a Job's delay.
   * 
   * @return double The Job's delay time
   */
  double getDelay() const;

 private:
  /**
   * @brief Get the Service object
   *
   * Randomly generate a service time from an exponential distribution with a
   * mean of 4046 seconds. (Note that these parameters (exponential and mean)
   * were gathered from analyzing the Discovery cluster data.)
   *
   * @return double The service time in seconds.
   */
  double getService();

  /**
   * @brief Calculate the waiting the waiting time
   *
   * Calculate the waiting time from both the delay and the service time. If
   * there is no delay, WAIT == SERVICE.
   *
   * @return double The time waiting in a service node.
   */
  double calcWait();

  double arrival;  // a Jobs arrival time in seconds
  double delay;    // the time spent waiting for service to begin
  double service;  // the time for this Job's service time
};

#endif