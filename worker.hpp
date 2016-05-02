#ifndef _WORKER_HPP_
#define _WORKER_HPP_

#include <condition_variable>
#include <deque>
#include <mutex>
#include <vector>
#include <memory>

#include "logger.hpp"
#include "job.hpp"

class Worker {
private:
  std::thread m_executor;
  std::deque<JobPtr> m_jobs;
  LoggerPtr m_logger;

  std::mutex m_jobs_ready_mtx;
  std::condition_variable m_jobs_ready_cond;

  void gen_log_msg();
  void flush_logs(JobPtr j);
  void run();
public:
  Worker(LoggerPtr l);
  ~Worker();
  void add_job(JobPtr j);
};
typedef std::shared_ptr<Worker> WorkerPtr;

#endif // _WORKER_HPP_
