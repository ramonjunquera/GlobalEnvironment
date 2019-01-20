#ifndef POINT_H
#define POINT_H

#include <iostream> //Para cout

using namespace std; //Para cout

class Point
{
public:
    Point(int x,int y);
    void show();
private:
    int _x,_y;
};

#endif // POINT_H
