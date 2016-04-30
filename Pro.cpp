#include <iostream>
#include <future>
#include <deque>
#include <vector>

using namespace std;

mutex g_out;

class Worker {
private:
  deque<string> m_messages;
  void gen_message() {
    int nom = rand() % 100;
    string l{"LOGLINE"};
    l += to_string(nom);
    //cout << l << "\n";
    m_messages.push_back(l);
    //{ 
    //  lock_guard<mutex> guard(g_out);
    //  cout << "[worker " << m_tid << " gen_msg] = Size of messages: " << m_messages.size() << "\n";
    //}
  }
public:

  void run() {
    while (true) {
      {
        lock_guard<mutex> guard(g_out);
        cout << "Hi! I'm " << this_thread::get_id() << "\n";
      }
      gen_message();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  void get_log() {
    { 
      lock_guard<mutex> guard(g_out);
      //cout << "[worker] get_log call - msg count: " << m_messages.size() << "\n"; 
    }
    if ( !m_messages.empty() ) {
      { 
        lock_guard<mutex> guard(g_out);
        cout << "[get_log] Before:: Size of messages: " << m_messages.size() << "\n";
      }
      string l = m_messages.front();
      m_messages.pop_front();
      { 
        lock_guard<mutex> guard(g_out);
        cout << "[get_log] After:: Size of messages: " << m_messages.size() << "\n";
        cout << "MESSAGE: " << l << "\n\n";
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
  srand (time(NULL));
  //Worker w1;
  auto w1 = std::make_shared<Worker>();
  thread tw1 (&Worker::run, w1);

  auto w2 = std::make_shared<Worker>();
  thread tw2 (&Worker::run, w2);

  Logger l;
  l.add_worker(w1);
  l.add_worker(w2);
  l.run();

  tw1.join();
  tw2.join();
  
}
