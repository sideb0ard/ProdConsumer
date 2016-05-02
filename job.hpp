#include <future>
#include <vector>

class Job {
public:
  string type;
  std::future<vector<string>> logs_buffer;
};
typedef std::shared_ptr<Job> JobPtr;
