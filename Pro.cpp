#include <iostream>
#include <future>
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;

std::mutex g_out;

std::mutex g_shutdown;
std::condition_variable g_shutdown_cond;

//thread_local deque<string> m_messages;

class Job {
public:
  string name;
};
typedef std::shared_ptr<Job> JobPtr;
  

class Worker {
private:
  thread m_executor;
  deque<JobPtr> m_jobs;

  std::mutex m_jobs_ready_mtx;
  std::condition_variable m_jobs_ready_cond;

  //void gen_message() {
  //  string l{"LOGLINE"};
  //  l += to_string(rand() % 100);
  //  { 
  //    lock_guard<mutex> guard(g_out);
  //    cout << "[" << this_thread::get_id() << "] - Msg " << l << "\n";
  //  }
  //  m_messages.push_back(l);
  //}

  void run() {
    {
      lock_guard<mutex> guard(g_out);
      cout << "[" << this_thread::get_id() << "] Worker starting up yo!\n";
    }
    while (true) {
      while ( m_jobs.size() > 0 ) {
          JobPtr j = m_jobs.front();
          m_jobs.pop_front();
          {
            lock_guard<mutex> guard(g_out);
            cout << "Popped a jobbie!\n";
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
  Worker() {
    m_executor = std::thread(&Worker::run, this);
  }

  ~Worker() {
    m_executor.join();
  }

  void send(JobPtr j) {
    {
      lock_guard<mutex> guard(g_out);
      cout << "received Job = type: " << j->name << "\n";
    }
    m_jobs.push_back(j);
    m_jobs_ready_cond.notify_all();
  }

};
typedef std::shared_ptr<Worker> WorkerPtr;

class Logger {
private:
  thread m_executor;
  vector<WorkerPtr> m_workers;
public:
  Logger() {
    m_executor = std::thread(&Logger::log, this);
  }
  ~Logger() {
    m_executor.join();
  }
  void add_worker(WorkerPtr w) {
    m_workers.push_back(w);
  }
  void log() {
    while (true) {
      for ( auto w : m_workers )  {
        JobPtr j = std::make_shared<Job>();
        j->name = "get logs";
        w->send(j);
      }
    }
  }
};

int main()
{
  cout << "[" << this_thread::get_id() << "] Mainthread\n";
  srand (time(NULL));
  auto w1 = std::make_shared<Worker>();

  Logger l;
  l.add_worker(w1);

  std::unique_lock<std::mutex> lk(g_shutdown);
  g_shutdown_cond.wait(lk);
  
}
