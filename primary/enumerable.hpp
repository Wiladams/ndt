#pragma once


// With templates
template <typename T>
struct IEnumerator {
    virtual bool moveNext() = 0;
    virtual void reset() = 0;
    
    virtual T getCurrent() const =0;
};

template <typename T>
struct IEnumerable {
    virtual IEnumerator<T> GetEnumerator() = 0;
};

