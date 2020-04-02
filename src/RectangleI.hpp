#pragma once
#include <math.h>

inline int MAXI(int a, int b) {return ((a>b)?a:b);}
inline int MINI(int a, int b) {return ((a<b)?a:b);}

struct RectangleI
{
    int x1;
    int y1;
    int x2;
    int y2;

    RectangleI(int x, int y, size_t awidth, size_t aheight)
    {
        x1 = x;
        y1 = y;

        x2 = x1 + awidth;
        y2 = y1 + aheight;
    }

    bool isEmpty()
    {
        return (x1 == x2) && (y1 == y2);
    }


    void setRect(int x1_, int y1_, int x2_, int y2_)
    {
            x1 = x1_;
            y1 = y1_;
            x2 = x2_;
            y2 = y2_;

            normalize();
    }

    size_t getWidth() const {return x2 - x1;}
    size_t getHeight() const {return y2 -y1;}

    // We want x1,y1 to represent top left
    // and x2,y2 to represent bottom right
    // so, we swap things around if needed
    void normalize()
    {
        int t;
        if (x1 > x2) { t = x1; x1 = x2; x2 = t; }
        if (y1 > y2) { t = y1; y1 = y2; y2 = t; }
    }

    bool clip(const RectangleI &r)
    {
            if (x2 > r.x2) x2 = r.x2;
            if (y2 > r.y2) y2 = r.y2;
            if (x1 < r.x1) x1 = r.x1;
            if (y1 < r.y1) y1 = r.y1;

            return isValid();
    }

    bool isValid()
    {
        return x1 <= x2 && y1 <= y2;
    }

    bool contains(int x, int y)
    {
        return ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2));
    }

    // Change the shape of this rectangle by intersecting
    // it with another one.
    void intersect(const RectangleI &rhs)
    {
        x1 = MAXI(x1, rhs.x1);
        x2 = MINI(x2, rhs.x2);
        y1 = MAXI(y1, rhs.y1);
        y2 = MINI(y2, rhs.y2);
    }

    // create a new rectangle from the intersection of 
    // two rectangles.
    static RectangleI intersection(const RectangleI &lhs, const RectangleI &rhs)
    {
        RectangleI res = lhs;
        res.intersect(rhs);
        
        return res;
    }
/*
        public static RectangleI Union(RectangleI left, RectangleI right)
        {
            int x1 = Math.Min(left.Left, right.Left);
            int x2 = Math.Max(left.Right, right.Right);
            int y1 = Math.Min(left.Bottom, right.Bottom);
            int y2 = Math.Max(left.Top, right.Top);

            return new RectangleI(x1, y1, x2, y2);
        }
*/
    void inflate(int dx, int dy)
    {
        normalize();
            
        x1 = x1 - dx;
        y1 = y1 - dy;
        x2 = x2 + dx;
        y2 = y2 + dy;
    }

    void inflate(int inflateSize)
    {
        normalize();
        x1 = x1 - inflateSize;
        y1 = y1 - inflateSize;
        x2 = x2 + inflateSize;
        y2 = y2 + inflateSize;
    }

    void offset(int x, int y)
    {
        x1 = x1 + x;
        y1 = y1 + y;
        x2 = x2 + x;
        y2 = y2 + y;
    }

};
