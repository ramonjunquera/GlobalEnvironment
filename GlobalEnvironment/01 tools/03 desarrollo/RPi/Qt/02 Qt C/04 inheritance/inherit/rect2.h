#ifndef RECT2_H
#define RECT2_H

class Rect2
{
public:
    Rect2(int width,int height); //Constructor
    int getArea();
private:
    int _width; //Puntero a anchura
    int _height; //Puntero a altura
};

#endif // RECT2_H
