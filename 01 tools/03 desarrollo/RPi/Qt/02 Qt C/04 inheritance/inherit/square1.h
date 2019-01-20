#ifndef SQUARE1_H
#define SQUARE1_H

#include <rect1.h>

class Square1 : public Rect1
{
public:
    void setSide(int side);
private:
    void setMeasures(int width, int height);
};

#endif // SQUARE1_H
