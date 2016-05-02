#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <vector>
#include <memory>

class Worker;

class Logger
{
private:
  std::thread m_executor;
  std::vector<Worker*> m_workers;
public:
  Logger();
  ~Logger();
  void register_worker(Worker* w);
  void run();
};
typedef std::shared_ptr<Logger> LoggerPtr;

#endif //  _LOGGER_HPP_
