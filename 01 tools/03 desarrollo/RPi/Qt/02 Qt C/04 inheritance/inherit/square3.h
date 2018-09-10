#ifndef SQUARE3_H
#define SQUARE3_H

#include <rect3.h>

class Square3 : public Rect3
{
public:
    Square3(); //Constructor
    void setSide(int side);
    ~Square3(); //Destructor
private:
    void setMeasures(int width, int height);
};

#endif // SQUARE3_H
