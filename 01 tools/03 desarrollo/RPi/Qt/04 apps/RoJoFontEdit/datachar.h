#ifndef DATACHAR_H
#define DATACHAR_H

#include <typedef.h>

class DataChar
{
public:
    DataChar();
    byte width;
    byte *data;
    void redimWidth(byte newWidth,byte pages);
    void redimPages(byte oldPages,byte newPages);
};

#endif // DATACHAR_H
