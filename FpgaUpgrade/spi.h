#ifndef __SPI_H
#define __SPI_H

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <stdio.h>

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <termios.h>
#include <time.h>

#define FLASH_CMD_WREN    0x06
#define FLASH_CMD_WRDI    0x04
#define FLASH_CMD_RDID    0x9f
#define FLASH_CMD_RDSR    0x05
#define FLASH_CMD_WRSR    0x01
#define FLASH_CMD_READ    0x03
#define FLASH_CMD_PP      0x02
#define FLASH_CMD_SE      0xd8
#define FLASH_CMD_BE      0xc7
#define FLASH_CMD_RES     0xab

#define SPI_MAX_SPEED 22000000

extern int gFdSpi;

extern int  spi_open (const char *path);
extern int  spi_set_mode (uint8_t mode);
extern int  spi_set_bits (uint8_t bits);
extern int  spi_set_speed (uint32_t speed);
extern void spi_close (void);

extern int  spi_rdid (char *id, size_t size);
extern int  spi_rdsr ( char *status);
extern int  spi_wrsr (uint8_t reg);
extern int  spi_wren (void);
extern int  spi_se (unsigned int address);
extern int  spi_be (void);
extern int  spi_pp (unsigned int address, char *data, size_t size_0_to_255);
extern int  spi_read (unsigned int address, char *data, size_t size);
extern int  spi_write (unsigned int address, char *data, size_t size);
extern int  spi_is_busy (void);
extern void spi_wait_ready(void);
extern void spi_wait_write_ready(void);


#endif //__SPI_H
