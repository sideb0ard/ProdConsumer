#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

#include "worker.hpp"
#include "logger.hpp"

extern std::mutex g_out;
extern thread_local std::vector<std::string> m_logs;

Worker::Worker(LoggerPtr l) {
  m_logger = l;
  l->register_worker(this);
  m_executor = std::thread(&Worker::run, this);
}

Worker::~Worker() {
  m_executor.join();
}

void Worker::add_job(JobPtr j) {
  {
    std::lock_guard<std::mutex> guard(g_out);
    std::cout << "received Job = type: " << j->type << "\n";
  }
  m_jobs.push_back(j);
  m_jobs_ready_cond.notify_all();
}

// ## private ##
void Worker::gen_log_msg() {
  auto my_id = std::this_thread::get_id();
  std::stringstream ss;
  ss << "[" << my_id << "] POPPED A JOBBIE - LOGLINE" << std::to_string(rand() % 100);
  std::string l = ss.str();
  {
    std::lock_guard<std::mutex> guard(g_out);
    std::cout << "[" << std::this_thread::get_id() << "] - Msg " << l << "\n";
  }
  m_logs.push_back(l);
}

void Worker::flush_logs(JobPtr j) {
  j->logs_buffer.set_value(m_logs);
  //while ( m_logs.size() > 0 ) {
  //    auto l = m_logs.front();
  //    m_logs.pop_front();
  //}
}

void Worker::run() {
  {
    std::lock_guard<std::mutex> guard(g_out);
    std::cout << "[" << std::this_thread::get_id() << "] Worker starting up yo!\n";
  }
  while (true) {
    while ( m_jobs.size() > 0 ) {
      JobPtr j = m_jobs.front();
      m_jobs.pop_front();
      if ( j->type.compare("wurk") == 0 ) {
        {
          std::lock_guard<std::mutex> guard(g_out);
          std::cout << "[" << std::this_thread::get_id() << "] Popped a jobbie!\n";
        }
        gen_log_msg();
      } else if ( j->type.compare("flush_logs") == 0 ) {
        {
          std::lock_guard<std::mutex> guard(g_out);
          std::cout << "[" << std::this_thread::get_id() << "] Flushing logs to Logger!\n";
        }
        flush_logs(j);
      }
    }
    std::unique_lock<std::mutex> lk(m_jobs_ready_mtx);
    m_jobs_ready_cond.wait(lk);
    {
      std::lock_guard<std::mutex> guard(g_out);
      std::cout << "Oh! new jobs ready!\n";
    }
  }
}
