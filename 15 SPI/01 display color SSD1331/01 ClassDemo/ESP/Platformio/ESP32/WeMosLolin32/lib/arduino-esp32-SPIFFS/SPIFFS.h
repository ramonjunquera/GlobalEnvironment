/* SPIFFS for Arduino ESP32
 * Author: Evandro Luis Copercini
 * Part of this code is copied from or inspired by SD_MMC library of Hristo Gochkov.
*/

#ifndef _SPIFFS_H_
#define _SPIFFS_H_

#include "FS.h"

namespace fs
{
class SPIFFSVFS : public FS
{
public:
    SPIFFSVFS(FSImplPtr impl);
    bool begin(const char * mountpoint="/spiffs");
    void end();
};

} //namespace

extern fs::SPIFFSVFS SPIFFS;

#endif /* _SPIFFS_H_ */