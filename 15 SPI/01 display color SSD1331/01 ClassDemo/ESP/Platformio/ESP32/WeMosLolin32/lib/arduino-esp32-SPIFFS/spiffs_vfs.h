/*
 * spiffs VFS public function
 *
 * Author: LoBo (loboris@gmail.com / https://github.com/loboris)
 *
 * Part of this code is copied from or inspired by LUA-RTOS_ESP32 project:
 *
 * https://github.com/whitecatboard/Lua-RTOS-ESP32
 * IBEROXARXA SERVICIOS INTEGRALES, S.L. & CSS IBÉRICA, S.L.
 * Jaume Olivé (jolive@iberoxarxa.com / jolive@whitecatboard.org)
 *
 */

// From sdkconfig
// On Arduino the values are defined based on this table: https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/default.csv
#define CONFIG_SPIFFS_LOG_PAGE_SIZE 256
#define CONFIG_SPIFFS_BASE_ADDR 0x291000
#define CONFIG_SPIFFS_SIZE 1478656
#define CONFIG_SPIFFS_LOG_BLOCK_SIZE 8192
#define SPIFFS_ERASE_SIZE 4096


int vfs_spiffs_register(const char * mountpoint);
int spiffs_mount();
int spiffs_unmount(int unreg);
void spiffs_fs_stat(uint32_t *total, uint32_t *used);
