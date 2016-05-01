#include <iostream>
#include <future>
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;

std::mutex g_out;

std::mutex g_shutdown;
std::condition_variable cv;

thread_local deque<string> m_messages;

class Worker {
private:

  thread m_executor;

  void gen_message() {
    string l{"LOGLINE"};
    l += to_string(rand() % 100);
    { 
      lock_guard<mutex> guard(g_out);
      cout << "[" << this_thread::get_id() << "] - Msg " << l << "\n";
    }
    m_messages.push_back(l);
  }

public:

  Worker() {
    cout << "Worker starting up yo!\n";
  }

  ~Worker() {
    m_executor.join();
  }

  void start() {
    m_executor = std::thread(&Worker::run, this);
  }


  void run() {
    while (true) {
      gen_message();
    }
  }

  void get_log() {
    if ( !m_messages.empty() ) {
      //{ 
      //  lock_guard<mutex> guard(g_out);
      //  cout << "[" << this_thread::get_id() << "] Before:: Size of messages: " << m_messages.size() << "\n";
      //}
      string l = m_messages.front();
      m_messages.pop_front();
      { 
        lock_guard<mutex> guard(g_out);
        cout << "[" << this_thread::get_id() << "] - Msg " << l << "\n";
      }
    }
  }
};

typedef std::shared_ptr<Worker> WorkerPtr;

class Logger {
public:
  void add_worker(WorkerPtr w) {
    m_workers.push_back(w);
  }
  void run() {
    while (true) {
      for ( auto w : m_workers )  {
        w->get_log();
      }
    }
  }
private:
  vector<WorkerPtr> m_workers;
};

int main()
{
  cout << "[" << this_thread::get_id() << "] Mainthread\n";
  srand (time(NULL));
  auto w1 = std::make_shared<Worker>();
  w1->start();
  auto w2 = std::make_shared<Worker>();
  w2->start();
  auto w3 = std::make_shared<Worker>();
  w3->start();

  Logger l;
  l.add_worker(w1);
  l.add_worker(w2);
  l.add_worker(w3);
  l.run();

  std::unique_lock<std::mutex> lk(g_shutdown);
  cv.wait(lk);
  
}
