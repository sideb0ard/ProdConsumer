#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

#include "worker.hpp"
#include "logger.hpp"

extern thread_local deque<string> m_logs;

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
    lock_guard<mutex> guard(g_out);
    cout << "received Job = type: " << j->type << "\n";
  }
  m_jobs.push_back(j);
  m_jobs_ready_cond.notify_all();
}

// ## private ##
void Worker::gen_log_msg() {
  auto my_id = this_thread::get_id();
  stringstream ss;
  ss << "[" << my_id << "] POPPED A JOBBIE - LOGLINE" << to_string(rand() % 100);
  string l = ss.str();
  {
    lock_guard<mutex> guard(g_out);
    cout << "[" << this_thread::get_id() << "] - Msg " << l << "\n";
  }
  m_logs.push_back(l);
}

void Worker::flush_logs() {
  while ( m_logs.size() > 0 ) {
      auto l = m_logs.front();
      m_logs.pop_front();
      m_logger->add_log(l);
  }
}

void Worker::run() {
  {
    lock_guard<mutex> guard(g_out);
    cout << "[" << this_thread::get_id() << "] Worker starting up yo!\n";
  }
  while (true) {
    while ( m_jobs.size() > 0 ) {
      JobPtr j = m_jobs.front();
      m_jobs.pop_front();
      if ( j->type.compare("wurk") == 0 ) {
        {
          lock_guard<mutex> guard(g_out);
          cout << "[" << this_thread::get_id() << "] Popped a jobbie!\n";
        }
        gen_log_msg();
      } else if ( j->type.compare("flush_logs") == 0 ) {
        {
          lock_guard<mutex> guard(g_out);
          cout << "[" << this_thread::get_id() << "] Flushing logs to Logger!\n";
        }
        flush_logs();
      }
    }
    std::unique_lock<std::mutex> lk(m_jobs_ready_mtx);
    m_jobs_ready_cond.wait(lk);
    {
      lock_guard<mutex> guard(g_out);
      cout << "Oh! new jobs ready!\n";
    }
  }
}
