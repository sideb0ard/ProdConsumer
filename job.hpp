#ifndef _JOB_HPP_
#define _JOB_HPP_

#include <future>
#include <vector>

class Job {
public:
  string type;
  std::future<vector<string>> logs_buffer;
};
typedef std::shared_ptr<Job> JobPtr;

#endif // _JOB_HPP_
