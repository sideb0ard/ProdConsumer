#ifndef _WORKER_HPP_
#define _WORKER_HPP_

#include <condition_variable>
#include <deque>
#include <mutex>
#include <vector>

#include "logger.hpp"
#include "job.hpp"

class Worker {
private:
  thread m_executor;
  deque<JobPtr> m_jobs;
  LoggerPtr m_logger;

  std::mutex m_jobs_ready_mtx;
  std::condition_variable m_jobs_ready_cond;

  void gen_log_msg();
  void flush_logs();
  void run();
public:
  Worker(LoggerPtr l);
  ~Worker();
  void add_job(JobPtr j);
};

#endif // _WORKER_HPP_
