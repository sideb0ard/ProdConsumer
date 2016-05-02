#include <condition_variable>
#include <iostream>
#include <mutex>

#include <stdlib.h>

#include "worker.hpp"
#include "logger.hpp"
#include "job.hpp"

std::mutex g_out;

std::mutex g_shutdown;
std::condition_variable g_shutdown_cond;

thread_local std::deque<std::string> m_logs;

int main()
{
  std::cout << "[" << std::this_thread::get_id() << "] Mainthread\n";
  srand (time(NULL));

  LoggerPtr l = std::make_shared<Logger>();
  WorkerPtr w1 = std::make_shared<Worker>(l);

  for ( int i = 0; i < 5; i++ ) {
    JobPtr j = std::make_shared<Job>();
    j->type = "wurk";
    w1->add_job(j);
  }

  std::unique_lock<std::mutex> lk(g_shutdown);
  g_shutdown_cond.wait(lk);

}
