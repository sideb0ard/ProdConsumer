#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

using namespace std;

std::mutex g_out;

std::mutex g_shutdown;
std::condition_variable g_shutdown_cond;

thread_local deque<string> m_logs;

class Job {
public:
  string type;
};
typedef std::shared_ptr<Job> JobPtr;


class Logger {
private:
  thread m_executor;
  std::mutex m_logs_ready_mtx;
  std::condition_variable m_logs_ready_cond;
public:
  Logger() {
    m_executor = std::thread(&Logger::log, this);
  }
  ~Logger() {
    m_executor.join();
  }

  void add_log(string l) {
    {
      lock_guard<mutex> guard(g_out);
      cout << "[" << this_thread::get_id() << "] got a message!" << l << "\n";
    }
    // TODO: create job so it ends up on thread local
  }

  void log() {
    while (true) {
      if ( m_logs.size() > 0 ) {
        for ( auto l : m_logs )  {
            cout << "[" << this_thread::get_id() << "] LOGGING.. " << l << "\n";
        }
      }
      std::unique_lock<std::mutex> lck(m_logs_ready_mtx);
      m_logs_ready_cond.wait(lck);
    }
  }
};
typedef std::shared_ptr<Logger> LoggerPtr;

class Worker {
private:
  thread m_executor;
  deque<JobPtr> m_jobs;
  LoggerPtr m_logger;

  std::mutex m_jobs_ready_mtx;
  std::condition_variable m_jobs_ready_cond;

  void gen_log_msg() {
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

  void flush_logs() {
      while ( m_logs.size() > 0 ) {
          auto l = m_logs.front();
          m_logs.pop_front();
          m_logger->add_log(l);
      }
  }

  void run() {
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

public:
  Worker(LoggerPtr l) {
    m_logger = l;
    m_executor = std::thread(&Worker::run, this);
  }

  ~Worker() {
    m_executor.join();
  }

  void send(JobPtr j) {
    {
      lock_guard<mutex> guard(g_out);
      cout << "received Job = type: " << j->type << "\n";
    }
    m_jobs.push_back(j);
    m_jobs_ready_cond.notify_all();
  }

};
typedef std::shared_ptr<Worker> WorkerPtr;


int main()
{
  cout << "[" << this_thread::get_id() << "] Mainthread\n";
  srand (time(NULL));

  LoggerPtr l = std::make_shared<Logger>();
  WorkerPtr w1 = std::make_shared<Worker>(l);

  for ( int i = 0; i < 5; i++ ) {
    JobPtr j = std::make_shared<Job>();
    j->type = "wurk";
    w1->send(j);
  }

  JobPtr j = std::make_shared<Job>();
  j->type = "flush_logs";
  w1->send(j);

  std::unique_lock<std::mutex> lk(g_shutdown);
  g_shutdown_cond.wait(lk);

}
