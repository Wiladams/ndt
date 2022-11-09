#pragma once

#include "List.hpp"

// Implementation of a Stack
// first in last out
// enqueue(val)
// val dequeue()
template<typename T>
class Stack
{
    List<T> fList;

public:
    bool push(T value) {fList.pushRight(value); return true;}
    T pop() {return fList.popRight();}
    int length() const {return fList.length();}
    T top() {return fList.peekRight();}

    // remove all the entries from the stack
    // BUGBUG - this will not call the destructors of the
    // objects being removed, so it must be used
    // with caution.
    Stack & clear()
    {
        while (length() > 0) {
            pop();
        }

        return *this;
    }

    // duplicate the entry on the top of the stack
    Stack & dup()
    {
        if (length() > 0) {
            push(top());
        }
        return *this;
    }

    // Exchange the two items on the top of the stack
    // same as: 2 1 roll
    Stack & exch()
    {
        // Only do the exchange if there are two
        // or more items already on the stack
        if (length() >= 2)
        {
            T a = pop();
            T b = pop();
            push(a);
            push(b);
        }
        return *this;
    }

    // Return the nth item on the stack
    // the top of the stack is item '0'
    // the second item down the stack is item '1'
    // and so on.
    T nth(const int n) const {return fList.nthFromRight(n);}

    // Copy the top n items on the stack
    Stack & copy(const int n)
    {
        for (int i=0;i<n;i++) {
            push(nth(n-1));
        }
        return *this;
    }

    // Convenience operator so we can use
    // [] array indexing notation to get at items
    T operator[](const int idx) const {return nth(idx);}
};
