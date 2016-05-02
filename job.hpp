#ifndef _JOB_HPP_
#define _JOB_HPP_

#include <future>
#include <vector>

class Job {
public:
  std::string type;
  std::future<std::vector<std::string>> logs_buffer;
};
typedef std::shared_ptr<Job> JobPtr;

#endif // _JOB_HPP_
