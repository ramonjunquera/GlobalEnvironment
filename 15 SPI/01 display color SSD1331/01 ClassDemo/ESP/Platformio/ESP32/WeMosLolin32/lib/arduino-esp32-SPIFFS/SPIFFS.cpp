/* SPIFFS for Arduino ESP32
 * Author: Evandro Luis Copercini
 * Part of this code is copied from or inspired by SD_MMC library of Hristo Gochkov.
*/

#include "vfs_api.h"
#include <ctype.h>

extern "C" {
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "spiffs_vfs.h"
}
#include <SPIFFS.h>

using namespace fs;

SPIFFSVFS::SPIFFSVFS(FSImplPtr impl) : FS(impl)
{}

bool SPIFFSVFS::begin(const char * mountpoint)
{
	if(vfs_spiffs_register(mountpoint)){
	    _impl->mountpoint(mountpoint);
	    return true;
	}
	else
        return false;
}

void SPIFFSVFS::end()
{
        spiffs_unmount(1); //1 for unregister too
        _impl->mountpoint(NULL);
}


SPIFFSVFS SPIFFS = SPIFFSVFS(FSImplPtr(new VFSImpl()));