#ifndef _JOB_HPP_
#define _JOB_HPP_

#include <future>
#include <vector>
#include <string>
#include <memory>

class Job {
public:
  std::string type;
  std::promise<std::vector<std::string>> logs_buffer;
};
typedef std::shared_ptr<Job> JobPtr;

#endif // _JOB_HPP_
