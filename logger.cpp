#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

#include "logger.hpp"
#include "worker.hpp"
#include "job.hpp"

extern std::mutex g_out;
extern thread_local std::vector<std::string> m_logs;

Logger::Logger() {
  m_executor = std::thread(&Logger::run, this);
}

Logger::~Logger() {
  m_executor.join();
}

void Logger::register_worker(Worker* w) {
  {
    std::lock_guard<std::mutex> guard(g_out);
    std::cout << "[" << std::this_thread::get_id() << "] Registering worker!\n";
  }
  m_workers.push_back(w);
  {
    std::lock_guard<std::mutex> guard(g_out);
    std::cout << "[" << std::this_thread::get_id() << "] m_workers size:" << m_workers.size() << "\n";
  }

}

void Logger::run() {
  std::cout << "[" << std::this_thread::get_id() << "] Logger starting up!!\n";
  //while (true) {
    if ( m_logs.size() > 0 ) {
      for ( auto l : m_logs )  {
        std::cout << "[" << std::this_thread::get_id() << "] LOGGING.. " << l << "\n";
      }
    }
    for ( auto w : m_workers ) {
      JobPtr j = std::make_shared<Job>();
      j->type = "flush_logs";

      std::promise<std::vector<std::string>> p;
      std::future<std::vector<std::string>> f = p.get_future();

      j->logs_buffer = std::move(p);
      w->add_job(j);

      f.wait();
      {
        std::lock_guard<std::mutex> guard(g_out);
        std::cout << "Got results!\n";
      }
    }

    //std::unique_lock<std::mutex> lck(m_logs_ready_mtx);
    //m_logs_ready_cond.wait(lck);
  //}
}
