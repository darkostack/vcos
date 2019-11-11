#ifndef LIST_HPP
#define LIST_HPP

namespace vc {

class ListNode
{
public:
    ListNode *mNext;
};

class List : public ListNode
{
public:
    void Add(ListNode *aList, ListNode *aNode)
    {
        aNode->mNext = aList->mNext;
        aList->mNext = aNode;
    }

    ListNode *RemoveHead(ListNode *aList)
    {
        ListNode *head = aList->mNext;
        if (head) {
            aList->mNext = head->mNext;
        }
        return head;
    }

    ListNode *Remove(ListNode *aList, ListNode *aNode)
    {
        while (aList->mNext) {
            if (aList->mNext == aNode) {
                aList->mNext = aNode->mNext;
                return aNode;
            }
            aList = aList->mNext;
        }
        return aList->mNext;
    }
};

} // namespace vc

#endif /* LIST_HPP */
