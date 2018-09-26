#ifndef DATAFONT_H
#define DATAFONT_H

#include <typedef.h>
#include <datachar.h>

class DataFont
{
public:
    DataFont();
    byte charMin;
    byte charMax;
    byte pages;
    DataChar *dataChar;
    byte charCount();
    void redimPages(byte newPages);
    void redimCharMax(byte newCharMax);
    void redimCharMin(byte newCharMin);
    void clear();
};

#endif // DATAFONT_H
