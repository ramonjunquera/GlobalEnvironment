#ifndef SQUARE4_H
#define SQUARE4_H

#include <rect4.h>

class Square4 : public Rect4
{
public:
    Square4(int side);
    ~Square4();
private:
    void setMeasures(int width, int height);
};

#endif // SQUARE4_H
