#ifndef LIST_HPP
#define LIST_HPP

#include <vcos/types.h>

namespace vc {

class ClistNode;

class List : public vcListNode
{
    friend class ClistNode;

public:
    void Add(List *aNode)
    {
        aNode->mNext = this->mNext;
        this->mNext = aNode;
    }

    List *RemoveHead(void)
    {
        List *head = static_cast<List *>(this->mNext);

        if (head)
        {
            this->mNext = head->mNext;
        }
        return head;
    }

    static List *Remove(List *aList, List *aNode)
    {
        while (aList->mNext)
        {
            if (aList->mNext == aNode)
            {
                aList->mNext = aNode->mNext;
                return aNode;
            }
            aList = static_cast<List *>(aList->mNext);
        }
        return static_cast<List *>(aList->mNext);
    }
};

} // namespace vc

#endif /* LIST_HPP */
