#ifndef RECT_H
#define RECT_H

#include <shape.h>
#include <costpaint.h>

class Rect : public Shape , public CostPaint
{
public:
    Rect(int width,int height);
    int area();
};

#endif // RECT_H
