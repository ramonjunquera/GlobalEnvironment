#ifndef RECT3_H
#define RECT3_H

class Rect3
{
public:
    Rect3(); //Cosntructor
    void setMeasures(int width,int height);
    ~Rect3(); //Destructor
    int getArea();
private:
    int *_width;
    int *_height;
};

#endif // RECT3_H
