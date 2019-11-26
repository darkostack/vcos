#include "net/tasklet.hpp"

#include "common/code_utils.hpp"
#include "common/locator-getters.hpp"

namespace vc {
namespace Net {

Tasklet::Tasklet(Handler aHandler)
    : mHandler(aHandler)
    , mNext(NULL)
{
}

vcNetError Tasklet::Post(void)
{
    return Get<TaskletScheduler>().Post(*this);
}

TaskletScheduler::TaskletScheduler(void)
    : mHead(NULL)
    , mTail(NULL)
{
}

vcNetError TaskletScheduler::Post(Tasklet &aTasklet)
{
    vcNetError error = VC_NET_ERROR_NONE;

    VerifyOrExit(mTail != &aTasklet && aTasklet.mNext == NULL, error = VC_NET_ERROR_ALREADY);

    VerifyOrExit(&aTasklet.Get<TaskletScheduler>() == this);

    if (mTail == NULL)
    {
        mHead = &aTasklet;
        mTail = &aTasklet;
        vcNetTaskletsSignalPending(&aTasklet.GetInstance());
    }
    else
    {
        mTail->mNext = &aTasklet;
        mTail        = &aTasklet;
    }

exit:
    return error;
}

Tasklet *TaskletScheduler::PopTasklet(void)
{
    Tasklet *task = mHead;

    if (task != NULL)
    {
        mHead = mHead->mNext;

        if (mHead == NULL)
        {
            mTail = NULL;
        }

        task->mNext = NULL;
    }

    return task;
}

void TaskletScheduler::ProcessQueuedTasklets(void)
{
    Tasklet *tail = mTail;
    Tasklet *cur;

    while ((cur = PopTasklet()) != NULL)
    {
        cur->RunTask();

        // only process tasklets that were queued at the time this method was called
        if (cur == tail)
        {
            if (mHead != NULL)
            {
                vcNetTaskletsSignalPending(&mHead->GetInstance());
            }

            break;
        }
    }
}

} // namespace Net
} // namespace vc
