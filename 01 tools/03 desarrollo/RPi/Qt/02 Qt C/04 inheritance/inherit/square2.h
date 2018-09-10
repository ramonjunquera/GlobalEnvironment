#ifndef SQUARE2_H
#define SQUARE2_H

#include <rect2.h>

class Square2 : public Rect2
{
public:
    Square2(int side);
private:
    void setMeasures(int width, int height);
};

#endif // SQUARE2_H
