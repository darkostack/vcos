#include "net/tasklet.hpp"

namespace vc {
namespace Net {

TaskletScheduler::TaskletScheduler(void)
    : mHead(NULL)
    , mTail(NULL)
{
}

} // namespace Net
} // namespace vc
