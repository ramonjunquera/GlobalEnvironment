/*
 * Lua RTOS, SPIFFS low access
 *
 * Copyright (C) 2015 - 2017
 * IBEROXARXA SERVICIOS INTEGRALES, S.L. & CSS IBÉRICA, S.L.
 * 
 * Author: Jaume Olivé (jolive@iberoxarxa.com / jolive@whitecatboard.org)
 * 
 * All rights reserved.  
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

#include <stdlib.h>

#include "esp_spiffs.h"
#include "esp_attr.h"

#include "spiffs_core.h"

#include <esp_spi_flash.h>

int32_t IRAM_ATTR esp32_spi_flash_read(uint32_t addr, uint32_t size, uint8_t *dst) {
	uint32_t aaddr;
	uint8_t *buff = NULL;
	uint8_t *abuff = NULL;
	uint32_t asize;

	asize = size;
	
	// Align address to 4 byte
	aaddr = (addr + (4 - 1)) & (uint32_t)-4;
	if (aaddr != addr) {
		aaddr -= 4;
		asize += (addr - aaddr);
	}

	// Align size to 4 byte
	asize = (asize + (4 - 1)) & (uint32_t)-4;

	if ((aaddr != addr) || (asize != size)) {
		// Align buffer
		buff = malloc(asize + 4);
		if (!buff) {
			return SPIFFS_ERR_INTERNAL;
		}

		abuff = (uint8_t *)(((ptrdiff_t)buff + (4 - 1)) & (uint32_t)-4);

		if (spi_flash_read(aaddr, (void *)abuff, asize) != 0) {
			free(buff);
			return SPIFFS_ERR_INTERNAL;
		}

		memcpy(dst, abuff + (addr - aaddr), size);

		free(buff);
	} else {
		if (spi_flash_read(addr, (void *)dst, size) != 0) {
			return SPIFFS_ERR_INTERNAL;
		}
	}
	
    return SPIFFS_OK;
}

int32_t IRAM_ATTR esp32_spi_flash_write(uint32_t addr, uint32_t size, const uint8_t *src) {
	uint32_t aaddr;
	uint8_t *buff = NULL;
	uint8_t *abuff = NULL;
	uint32_t asize;

	asize = size;
	
	// Align address to 4 byte
	aaddr = (addr + (4 - 1)) & -4;
	if (aaddr != addr) {
		aaddr -= 4;
		asize += (addr - aaddr);
	}

	// Align size to 4 byte
	asize = (asize + (4 - 1)) & -4; 

	if ((aaddr != addr) || (asize != size)) {
		// Align buffer
		buff = malloc(asize + 4);
		if (!buff) {
			return SPIFFS_ERR_INTERNAL;
		}

		abuff = (uint8_t *)(((ptrdiff_t)buff + (4 - 1)) & -4);

		if (spi_flash_read(aaddr, (void *)abuff, asize) != 0) {
			free(buff);
			return SPIFFS_ERR_INTERNAL;
		}

		memcpy(abuff + (addr - aaddr), src, size);

		if (spi_flash_write(aaddr, (uint32_t *)abuff, asize) != 0) {
			free(buff);
			return SPIFFS_ERR_INTERNAL;
		}

		free(buff);
	} else {
		if (spi_flash_write(addr, (uint32_t *)src, size) != 0) {
			return SPIFFS_ERR_INTERNAL;
		}
	}
	
    return SPIFFS_OK;
}

int32_t IRAM_ATTR esp32_spi_flash_erase(uint32_t addr, uint32_t size) {
	if (spi_flash_erase_sector(addr >> 12) != 0) {
		return SPIFFS_ERR_INTERNAL;
	}
	
    return SPIFFS_OK;
}
