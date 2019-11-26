#ifndef NET_TASKLET_HPP
#define NET_TASKLET_HPP

#include "vcos-core-config.h"

#include <stdio.h>

#include <vcos/net/error.h>
#include <vcos/net/tasklet.h>

#include "common/locator.hpp"

namespace vc {
namespace Net {

class TaskletScheduler;

class Tasklet : public InstanceLocator
{
    friend class TaskletScheduler;

public:
    typedef void (*Handler)(Tasklet &aTasklet);

    Tasklet(Handler aHandler);

    vcNetError Post(void);

private:
    void RunTask(void) { mHandler(*this); }

    Handler  mHandler;
    Tasklet *mNext;
};

class TaskletContext : public Tasklet
{
public:
    TaskletContext(Handler aHandler, void *aContext)
        : Tasklet(aHandler)
        , mContext(aContext)
    {
    }

    void *GetContext(void) { return mContext; }

private:
    void *mContext;
};

class TaskletScheduler
{
public:
    TaskletScheduler(void);

    vcNetError Post(Tasklet &aTasklet);

    bool AreTaskletsPending(void) const { return mHead != NULL; }

    void ProcessQueuedTasklets(void);

private:
    Tasklet *PopTasklet(void);

    Tasklet *mHead;
    Tasklet *mTail;
};

} // namespace Net
} // namespace vc

#endif /* NET_TASKLET_HPP */
