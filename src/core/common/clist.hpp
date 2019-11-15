#ifndef CLIST_HPP
#define CLIST_HPP

#include <stddef.h>

#include "common/list.hpp"

namespace vc {

class ClistNode : public ListNode
{
public:
    void RightPush(ClistNode *aNode)
    {
        if (this->mNext)
        {
            aNode->mNext = this->mNext->mNext;
            this->mNext->mNext = aNode;
        }
        else
        {
            aNode->mNext = aNode;
        }

        this->mNext = aNode;
    }

    void LeftPush(ClistNode *aNode)
    {
        if (this->mNext)
        {
            aNode->mNext = this->mNext->mNext;
            this->mNext->mNext = aNode;
        }
        else
        {
            aNode->mNext = aNode;
            this->mNext = aNode;
        }
    }

    ClistNode *LeftPop(void)
    {
        if (this->mNext)
        {
            ClistNode *first = static_cast<ClistNode *>(this->mNext->mNext);

            if (this->mNext == first)
            {
                this->mNext = NULL;
            }
            else
            {
                this->mNext->mNext = first->mNext;
            }

            return first;
        }
        else
        {
            return NULL;
        }
    }

    void LeftPopRightPush(void)
    {
        if (this->mNext)
        {
            this->mNext = this->mNext->mNext;
        }
    }

    ClistNode *LeftPeek(void)
    {
        if (this->mNext)
        {
            return static_cast<ClistNode *>(this->mNext->mNext);
        }

        return NULL;
    }

    ClistNode *RightPeek(void)
    {
        return static_cast<ClistNode *>(this->mNext);
    }

    ClistNode *RightPop(void)
    {
        if (this->mNext)
        {
            ListNode *last = this->mNext;

            while (this->mNext->mNext != last)
            {
                this->LeftPopRightPush();
            }

            return this->LeftPop();
        }
        else
        {
            return NULL;
        }
    }

    ClistNode *FindBefore(const ClistNode *aNode)
    {
        ClistNode *pos = static_cast<ClistNode *>(this->mNext);

        if (!pos)
        {
            return NULL;
        }

        do
        {
            pos = static_cast<ClistNode *>(pos->mNext);

            if (pos->mNext == aNode)
            {
                return pos;
            }

        } while (pos != this->mNext);

        return NULL;
    }

    ClistNode *Find(const ClistNode *aNode)
    {
        ClistNode *tmp = this->FindBefore(aNode);

        if (tmp)
        {
            return static_cast<ClistNode *>(tmp->mNext);
        }
        else
        {
            return NULL;
        }
    }

    ClistNode *Remove(ClistNode *aNode)
    {
        if (this->mNext)
        {
            if (this->mNext->mNext == aNode)
            {
                return this->LeftPop();
            }
            else
            {
                ClistNode *tmp = this->FindBefore(aNode);

                if (tmp)
                {
                    tmp->mNext = tmp->mNext->mNext;

                    if (aNode == this->mNext)
                    {
                        this->mNext = tmp;
                    }
                    return aNode;
                }
            }
        }

        return NULL;
    }

    size_t Count(void)
    {
        ClistNode *node = static_cast<ClistNode *>(this->mNext);
        size_t cnt = 0;

        if (node)
        {
            do
            {
                node = static_cast<ClistNode *>(node->mNext);
                ++cnt;
            } while (node != this->mNext);
        }

        return cnt;
    }
};

} // namespace vc

#endif /* CLIST_HPP */
