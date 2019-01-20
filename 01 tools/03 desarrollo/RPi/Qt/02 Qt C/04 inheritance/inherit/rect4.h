#ifndef RECT4_H
#define RECT4_H

class Rect4
{
public:
    Rect4(int width,int height); //Constructor
    ~Rect4(); //Destructor
    int getArea();
private:
    int *_width;
    int *_height;
};

#endif // RECT4_H
