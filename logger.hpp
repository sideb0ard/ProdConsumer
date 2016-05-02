#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <vector>

#include "worker.hpp"

class Logger {
private:
  thread m_executor;
  std::mutex m_logs_ready_mtx;
  std::condition_variable m_logs_ready_cond;
  std::vector<Worker*> m_workers;
public:
  Logger();
  ~Logger();
  void register_worker(Worker* w);
  void run();
};
typedef std::shared_ptr<Logger> LoggerPtr;
