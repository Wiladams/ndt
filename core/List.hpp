#pragma once

template <typename T>
struct ListNode {
    T value;
    ListNode<T> *left;
    ListNode<T> *right;

    ListNode(T value, ListNode<T> *left, ListNode<T> *right)
        : left(left), right(right), value(value)
    {
    }
} ;

template <typename T>
struct List {
    size_t count;
    ListNode<T> * farLeft;
    ListNode<T> * farRight;

    List() 
        : count(0), farLeft(nullptr), farRight(nullptr){}

    int length() const {
        return count;
    }

    bool isEmpty() const {
        return count <= 0;
    }

    void pushRight(T value) {
        ListNode<T> * newNode = new ListNode<T>(value, farRight, nullptr);

        if (farRight != nullptr) {
            farRight->right = newNode;
        }

        farRight = newNode;

        if (farLeft == nullptr) {
            farLeft = newNode;
        }

        count = count + 1;
    }

    T popRight()
    {
        ListNode<T> * valueNode = farRight;
        if (valueNode == nullptr) {
            return T();
        }

        T retValue = valueNode->value;
        if (valueNode->left != nullptr) {
            farRight = valueNode->left;
            farRight->right = nullptr;
        } else {
            farRight = nullptr;
        }


        if (farLeft == valueNode) {
            farLeft = nullptr;
        }
        count = count - 1;

        // Make sure we don't have a memory leak
        delete valueNode;

        return retValue;
    }



    T popLeft()
    {
        ListNode<T> * valueNode = farLeft;
        if (valueNode == nullptr) {
            return T();
        }

        T retValue = valueNode->value;
        if (valueNode->right != nullptr) {
            farLeft = valueNode->right;
            farLeft->left = nullptr;
        } else {
            farLeft = nullptr;
        }

        if (farRight == valueNode) {
            farRight = nullptr;
        }
        
        count = count - 1;

        // Make sure we don't have a memory leak
        delete valueNode;

        return retValue;
    }

    T peekRight() const {
        if (farRight == nullptr) {
            // BUGBUG - maybe throw exception
            return T(NULL);
        }

        return farRight->value;
    }

    T peekLeft() const
    {
        if (farLeft == nullptr) {
            // BUGBUG - maybe throw exception
            return T();
        }

        return farLeft->value;
    }

    T nthFromLeft(const int n) const
    {
        int count = 0;
        ListNode<T> sentinel = farLeft;
        if (sentinel == nullptr) {
            return T();
        }

        while (count < n) {
            sentinel = sentinel->right;
            if (sentinel == nullptr) {
                return T();
            }
            count = count + 1;
        }
        return sentinel->value;
    }

    T nthFromRight(const int n) const
    {
        int count = n;
        ListNode<T> * sentinel = farRight;
        // If the list is empty
        // return nothing
        // BUGBUG - should throw (index out of range) exception
        if (sentinel == nullptr) {
            return T();
        }

        while (count > 0) {
            sentinel = sentinel->left;
            // BUGBUG - should throw out of range exception
            if (sentinel == nullptr) {
                return T();
            }
            count = count - 1;
        }

        return sentinel->value;
    }
};
