#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

#include "worker.hpp"
#include "job.hpp"

extern std::mutex g_out;
extern thread_local deque<string> m_logs;

Logger::Logger() {
  m_executor = std::thread(&Logger::run, this);
}

Logger::~Logger() {
  m_executor.join();
}

void Logger::register_worker(Worker* w) {
  {
    lock_guard<mutex> guard(g_out);
    cout << "[" << this_thread::get_id() << "] Registering worker!\n";
  }
  m_workers.push_back(w);
  {
    lock_guard<mutex> guard(g_out);
    cout << "[" << this_thread::get_id() << "] m_workers size:" << m_workers.size() << "\n";
  }

}

void Logger::run() {
  cout << "[" << this_thread::get_id() << "] Logger starting up!!\n";
  while (true) {
    if ( m_logs.size() > 0 ) {
      for ( auto l : m_logs )  {
          cout << "[" << this_thread::get_id() << "] LOGGING.. " << l << "\n";
      }
    }
    for ( auto w : m_workers ) {
      JobPtr j = std::make_shared<Job>();
      j->type = "flush_logs";
      w->add_job(j);
    }

    std::unique_lock<std::mutex> lck(m_logs_ready_mtx);
    m_logs_ready_cond.wait(lck);
  }
}
