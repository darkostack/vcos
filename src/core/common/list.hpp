#ifndef LIST_HPP
#define LIST_HPP

namespace vc {

class ClistNode;

class ListNode
{
    friend class ClistNode;

public:
    void Add(ListNode *aNode)
    {
        aNode->mNext = this->mNext;
        this->mNext = aNode;
    }

    ListNode *RemoveHead(void)
    {
        ListNode *head = this->mNext;

        if (head)
        {
            this->mNext = head->mNext;
        }
        return head;
    }

    static ListNode *Remove(ListNode *aList, ListNode *aNode)
    {
        while (aList->mNext)
        {
            if (aList->mNext == aNode)
            {
                aList->mNext = aNode->mNext;
                return aNode;
            }
            aList = aList->mNext;
        }
        return aList->mNext;
    }

    ListNode *GetNext(void) { return mNext; }

    void SetNext(ListNode *aNext) { mNext = aNext; }

private:
    ListNode *mNext;
};

} // namespace vc

#endif /* LIST_HPP */
