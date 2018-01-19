#include "spi.h"

#define Lin_Dbg
#ifdef Lin_Dbg
    #define PDBG(fmt, args...)  printf(fmt, ## args)
    #else
    #define PDBG(fmt, args...) /* empty debug slot */
#endif

int gFdSpi;




///////////////////////////////////////////////////////////////
/**
 * @brief spi_open
 * @param path, device node path
 * @return
 */
int  spi_open (const char *path)//open the spi device
{
    gFdSpi = open (path, O_RDWR);
    if (gFdSpi < 0)
    {
        PDBG("%s[%d]\n", __func__, __LINE__);
        return -1;
    }

    return gFdSpi;
}


void spi_close (void)
{
    close (gFdSpi);
}


/**
 * @brief spi_set_mode
 * @param mode(unsigned char) is : SPI_MODE_0,SPI_MODE_1,SPI_MODE_2,SPI_MODE_3;
 * @return res, operation error if res < 0
 */
int  spi_set_mode (uint8_t mode)//set spi device write and read mode!
{
    int res;

    if ((res = (ioctl(gFdSpi, SPI_IOC_RD_MODE, &mode))) != 0)
    {
        PDBG("%s[%d]:: res:%d\n", __func__, __LINE__);
        return -1;
    }
    if ((res = (ioctl(gFdSpi, SPI_IOC_WR_MODE, &mode))) != 0) {
        PDBG("%s[%d]:: res:%d\n", __func__, __LINE__);
        return -1;
    }

    PDBG("%s[%d]:: spi_set_mode OK, res:%d\n", __func__, __LINE__);

    return res;
}

/**
 * @brief spi_set_bits, set the spi trans data bit
 * @param bits: usually is 8, ususlly range(8---16)
 * @return res, operation error if res < 0
 */
int  spi_set_bits (uint8_t bits)//set the data bit for translate...... usally is 8 bit.
{
    int res;

    if ((res = ioctl(gFdSpi, SPI_IOC_WR_BITS_PER_WORD, &bits)) != 0){
        PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
        return -1;
    }
    if ((res = ioctl(gFdSpi, SPI_IOC_RD_BITS_PER_WORD, &bits)) != 0){
        PDBG("%s[%d]:: res:%d\n", __func__, __LINE__);
        return -1;
    }

    PDBG("[%s]:%d, spi_set_bits OK, res:%d \n", __func__, __LINE__, res);
    return res;
}

/**
 * @brief spi_set_speed, set the SPI clock speed!
 * @param speed, a uint32_t type, , I set it 6000000(6M / s) here
 * @return it should be > 0, when error occured return < 0
 */
int  spi_set_speed (uint32_t speed)
{
    int res;

    if ((res = ioctl(gFdSpi, SPI_IOC_WR_MAX_SPEED_HZ, &speed)) != 0)
    {
        PDBG("[%s]:%d, set WR speed error...: res:%d \n", __func__, __LINE__, res);
        return -1;
    }
    if ((res = ioctl(gFdSpi, SPI_IOC_RD_MAX_SPEED_HZ, &speed)) != 0)
    {
        PDBG("[%s]:%d, set RD speed error..: res:%d \n", __func__, __LINE__, res);
        return -1;
    }

    PDBG("[%s]:%d, set WR speed OK, res:%d \n", __func__, __LINE__, res);
    return res;
}
///////////////////////////////////////////////////////////////


//basic spi ioctl function, for ID and SR reading
int do_spi_msg(uint8_t CMD, uint8_t *ur_rx_bf, int len)
{
	uint8_t buf[8];
	struct spi_ioc_transfer xfer[2];
	memset(xfer, 0x00, sizeof xfer);
	int cnt, res;
	buf[0] = CMD;
	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = 1;
	xfer[0].speed_hz = SPI_MAX_SPEED;

	xfer[1].rx_buf = (unsigned long)ur_rx_bf;
	xfer[1].len = len;
	//xfer[1].cs_change = 1;
	xfer[1].speed_hz = SPI_MAX_SPEED;
	
	if ((res = ioctl(gFdSpi, SPI_IOC_MESSAGE(2), xfer)) < 0){
		PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
		return -1;
    }
    
//    PDBG("%s[%d]:: len %d, ioctl res:%d \n", __func__, __LINE__, len, res);
    
//    for(cnt = 0; cnt < len; cnt++){
//		PDBG("%02x,", ur_rx_bf[cnt]);
//	}
//	printf("\n");
	return res;
}


/**
 * @brief spi_rdid, get the spi flash id, store the id , in id array
 * @param id
 * @param size
 * @return
 */
int  spi_rdid (char *id, size_t size)
{
    if (gFdSpi < 0 || NULL == id || size < 0)
    {
        return -1;
    }
    PDBG("[%s]:%d, STEP01.. \n", __func__, __LINE__);

    int res;
    uint8_t cmd = FLASH_CMD_RDID;
    
    PDBG("[%s]:%d, STEP02, wr CMD: %x\n", __func__, __LINE__, cmd);
    res = do_spi_msg(cmd, id, 3);   
    if(res>-1){
		PDBG("[%s]:%d, STEP04...ID OK, res:%d\n", __func__, __LINE__, res);
	}

    return res;
}

/**
 * @brief spi_rdsr
 * @param status
 * @return
 */
int  spi_rdsr (char *status)
{
    if (gFdSpi < 0 || NULL == status)
    {
        return -1;
    }
    //PDBG("[%s]:%d, STEP01.. \n", __func__, __LINE__);

    int res;
    uint8_t cmd = FLASH_CMD_RDSR;
    
    //PDBG("[%s]:%d, STEP02, wr CMD: %x\n", __func__, __LINE__, cmd);
    res = do_spi_msg(cmd, status, 1);   
    if(res>-1){
		//PDBG("[%s]:%d, STEP04...read SR OK, res:%d\n", __func__, __LINE__, res);
	}
	return res;
}

/**
 * @brief spi_wren
 * @return
 */
int  spi_wren (void)
{
    if (gFdSpi < 0)
    {
        return -1;
    }

	int res,cnt;
	uint8_t buf[8];
    struct spi_ioc_transfer xfer[1];
    
	memset(xfer, 0x00, sizeof xfer);
	memset(buf, 0x00, sizeof(buf));
	
	buf[0] = FLASH_CMD_WREN;
	
	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = 1;
	xfer[0].speed_hz = SPI_MAX_SPEED;
	
	if ((res = ioctl(gFdSpi, SPI_IOC_MESSAGE(1), xfer)) < 0){
		PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
		return -1;
    }
    //PDBG("%s[%d]::ioctl res:%d \n", __func__, __LINE__, res);

    return res;
}

/* ////useless, we don't use this function 
int  spi_wrsr (uint8_t reg)
{
    if (gFdSpi < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = FLASH_CMD_WRSR;
    uint8_t status = reg;

    spi_wren();

    if ((res = write (gFdSpi, &cmd, sizeof (cmd))) < 0)
    {
		PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
        return -1;
    }

    if ((res = write (gFdSpi, &status, sizeof (status))) < 0)
    {
		PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
        return -1;
    }

    return res;
}
*/

/* ////useless, we don't use this function 
int  spi_se (unsigned int address)
{
    if (gFdSpi < 0)
    {
        return -1;
    }
    uint8_t buf[8];
    struct spi_ioc_transfer xfer[1];
    int res;
	int cnt;

    spi_wren();

	memset(xfer, 0x00, sizeof xfer);
	
	buf[0] = FLASH_CMD_SE;
	buf[1] = (uint8_t)(address & 0x00ffffff) >> 16;
	buf[2] = (uint8_t)(address & 0x00ffffff) >> 8;
	buf[3] = (uint8_t)(address & 0x00ffffff);
	
	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = 4;
	xfer[0].speed_hz = SPI_MAX_SPEED;
	xfer[0].cs_change = 1;

	
	if ((res = ioctl(gFdSpi, SPI_IOC_MESSAGE(1), xfer)) < 0){
		PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
		return -1;
    }
    
    PDBG("%s[%d]:: ioctl res:%d \n", __func__, __LINE__, res);
    
	return res;

//////////////////////////////////////////////
    if ((res = write (gFdSpi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }

    uint8_t addr;
    // H8
    addr = (address & 0x00ffffff) >> 16;
    if ((res = write (gFdSpi, &addr, 1)) < 0)
    {
        return -1;
    }
    // M8
    addr = (address & 0x00ffffff) >> 8;
    if ((res = write (gFdSpi, &addr, 1)) < 0)
    {
        return -1;
    }
    // L8
    addr = address & 0x00ffffff;
    if ((res = write (gFdSpi, &addr, 1)) < 0)
    {
        return -1;
    }


    return res;
}
*/
int  spi_be (void)
{
    if (gFdSpi < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = FLASH_CMD_BE;

    spi_wren();
    spi_wait_write_ready();
    spi_wait_ready();
    
    if ((res = write (gFdSpi, &cmd, 1)) < 0)
    {
        return -1;
    }
	PDBG("[%s]:%d, gFdSpi:%d, res:%d\n", __func__, __LINE__, gFdSpi, res);

    return res;
}


/**
 * @brief spi_read
 * @param address
 * @param data
 * @param size
 * @return
 */
int  spi_read (unsigned int address, char *data, size_t size)
{
    if (gFdSpi < 0 || address < 0)
    {
        return -1;
    }
    if (NULL == data || size < 0)
    {
        return -1;
    }

    int res;
    int cnt;
    char buf[8];
    char rx_buf[256];
	struct spi_ioc_transfer xfer[2];
	
	memset(xfer, 0x00, sizeof xfer);
	memset(buf, 0x00, sizeof(buf));
	
	buf[0] = FLASH_CMD_READ;
	buf[1] = (uint8_t)(address>>16);
	buf[2] = (uint8_t)(address>>8);
	buf[3] = (uint8_t)(address);
	
	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = 4;
	xfer[0].speed_hz = SPI_MAX_SPEED;
	xfer[0].delay_usecs = 1;
	
	xfer[1].rx_buf = (unsigned long)data;
	xfer[1].len = size;
	//xfer[1].cs_change = 1;
	xfer[1].speed_hz = SPI_MAX_SPEED;
	
	if ((res = ioctl(gFdSpi, SPI_IOC_MESSAGE(2), xfer)) < 0){
		PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
		return -1;
    }
    //PDBG("[%s]:%d, gFdSpi:%d, res:%d\n\n\n", __func__, __LINE__, gFdSpi, res);
    //PDBG("===>> %s[%d]:: == add_buf[0]:%02x, add_buf[1]:%02x, add_buf[2]:%02x\n", 
		//__func__, __LINE__, buf[1],buf[2],buf[3]);

    return (res-4);
}


int  spi_pp (unsigned int address, char *data, size_t size_0_to_255)
{
    if (gFdSpi < 0 || address < 0)
    {
        return -1;
    }
    if (NULL == data || size_0_to_255 < 0)
    {
        return -1;
    }


    int res,cnt;
	uint8_t buf[300];
    struct spi_ioc_transfer xfer[1];
    
    spi_wait_ready();
	spi_wren();
	spi_wait_write_ready();
    spi_wait_ready();
    
	memset(xfer, 0x00, sizeof xfer);
	memset(buf, 0x00, sizeof(buf));
	
    
	for(cnt = 4; cnt < (size_0_to_255+4); cnt++)
	{
		buf[cnt] = *(data+cnt-4);
	}

	buf[0] = FLASH_CMD_PP;
	buf[1] = (uint8_t)((address & 0x00ffffff) >> 16);
	buf[2] = (uint8_t)((address & 0x00ffffff) >> 8);
	buf[3] = (uint8_t)(address & 0x00ffffff);
	
	//PDBG("===>> %s[%d]:: data to be writen:%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n == add_buf[0]:%02x, add_buf[1]:%02x, add_buf[2]:%02x\n", 
		//__func__, __LINE__, buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11],  buf[1],buf[2],buf[3]);
		
	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = size_0_to_255+4;
	xfer[0].speed_hz = SPI_MAX_SPEED;
	xfer[0].delay_usecs = 1;
	
	if ((res = ioctl(gFdSpi, SPI_IOC_MESSAGE(1), xfer)) < 0){
		PDBG("%s[%d]:: res:%d\n", __func__, __LINE__, res);
		return -1;
    }
    //PDBG("%s[%d]:ioctl res:%d address: %06xH\n\n\n", __func__, __LINE__, res, address);

    return size_0_to_255;
    
}


int  spi_write (unsigned int address, char *data, size_t size)
{
    int cnt = 0;
    int res = 0;
    int tmp = size;
    unsigned int addr = address;

    while (tmp / 256)
    {
        res = spi_pp (address + cnt, data + cnt, 256);
        if (res < 0)
        {
            return -1;
        }
        tmp -= res;
        cnt += res;
    }

    while (tmp % 256)
    {
        res = spi_pp (address + cnt, data + cnt, (tmp % 256));
        if (res < 0)
        {
            return -1;
        }
        tmp -= res;
        cnt += res;
    }

    return cnt;
}


/**
 * @brief spi_is_busy
 * @return , 0 if spi is not busy, if busy return none zero
 */
int  spi_is_busy (void)
{
    char status;

    if (spi_rdsr(&status) < 0)
    {
		PDBG("%s[%d]:: error\n", __func__, __LINE__);
        return -1;
    }
    //PDBG("%s[%d]:: status:%02x\n", __func__, __LINE__, status);
    return status & 0x01;
}


int  spi_write_enable (void)
{
    char status;

    if (spi_rdsr(&status) < 0)
    {
		PDBG("%s[%d]:: error\n", __func__, __LINE__);
        return -1;
    }
    //PDBG("%s[%d]:: status:%02x\n", __func__, __LINE__, status);
    return status & 0x02;
}


void spi_wait_ready (void)
{
    while (1)
    {
        if (0 == spi_is_busy())
        {
            break;
        }
        else
        {
            usleep (50);
            continue;
        }
    }

    return ;
}

void spi_wait_write_ready (void)
{
    while (1)
    {
        if (spi_write_enable() == 0x02)
        {
            break;
        }
        else
        {
            usleep (50);
            spi_wren();
            continue;
        }
    }

    return ;
}
