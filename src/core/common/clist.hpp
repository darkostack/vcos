#ifndef CLIST_HPP
#define CLIST_HPP

#include <stddef.h>

#include "common/list.hpp"

namespace vc {

class ClistNode : public ListNode
{
};

class Clist : public ClistNode
{
public:
    void RightPush(ClistNode *aList, ClistNode *aNode)
    {
        if (aList->mNext) {
            aNode->mNext = aList->mNext->mNext;
            aList->mNext->mNext = aNode;
        } else {
            aNode->mNext = aNode;
        }
        aList->mNext = aNode;
    }

    void LeftPush(ClistNode *aList, ClistNode *aNode)
    {
        if (aList->mNext) {
            aNode->mNext = aList->mNext->mNext;
            aList->mNext->mNext = aNode;
        } else {
            aNode->mNext = aNode;
            aList->mNext = aNode;
        }
    }

    ClistNode *LeftPop(ClistNode *aList)
    {
        if (aList->mNext) {
            ClistNode *first = static_cast<ClistNode *>(aList->mNext->mNext);
            if (aList->mNext == first) {
                aList->mNext = NULL;
            } else {
                aList->mNext->mNext = first->mNext;
            }
            return first;
        } else {
            return NULL;
        }
    }

    void LeftPopRightPush(ClistNode *aList)
    {
        if (aList->mNext) {
            aList->mNext = aList->mNext->mNext;
        }
    }

    ClistNode *LeftPeek(const ClistNode *aList)
    {
        if (aList->mNext) {
            return static_cast<ClistNode *>(aList->mNext->mNext);
        }
        return NULL;
    }

    ClistNode *RightPeek(const ClistNode *aList)
    {
        return static_cast<ClistNode *>(aList->mNext);
    }

    ClistNode *RightPop(ClistNode *aList)
    {
        if (aList->mNext) {
            ListNode *last = aList->mNext;
            while (aList->mNext->mNext != last) {
                LeftPopRightPush(aList);
            }
            return LeftPop(aList);
        } else {
            return NULL;
        }
    }

    ClistNode *FindBefore(const ClistNode *aList, const ClistNode *aNode)
    {
        ClistNode *pos = static_cast<ClistNode *>(aList->mNext);

        if (!pos) {
            return NULL;
        }

        do {
            pos = static_cast<ClistNode *>(pos->mNext);
            if (pos->mNext == aNode) {
                return pos;
            }
        } while (pos != aList->mNext);

        return NULL;
    }

    ClistNode *Find(const ClistNode *aList, const ClistNode *aNode)
    {
        ClistNode *tmp = FindBefore(aList, aNode);
        if (tmp) {
            return static_cast<ClistNode *>(tmp->mNext);
        } else {
            return NULL;
        }
    }

    ClistNode *Remove(ClistNode *aList, ClistNode *aNode)
    {
        if (aList->mNext) {
            if (aList->mNext->mNext == aNode) {
                return LeftPop(aList);
            } else {
                ClistNode *tmp = FindBefore(aList, aNode);
                if (tmp) {
                    tmp->mNext = tmp->mNext->mNext;
                    if (aNode == aList->mNext) {
                        aList->mNext = tmp;
                    }
                    return aNode;
                }
            }
        }
        return NULL;
    }

    size_t Count(ClistNode *aList)
    {
        ClistNode *node = static_cast<ClistNode *>(aList->mNext);
        size_t cnt = 0;
        if (node) {
            do {
                node = static_cast<ClistNode *>(node->mNext);
                ++cnt;
            } while (node != aList->mNext);
        }
        return cnt;
    }
};

} // namespace vc

#endif /* CLIST_HPP */
