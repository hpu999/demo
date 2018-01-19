#include "spi.h"
// linux 读取spi flash代码linux SPI flash代码，linux读写spi flash
#define DEF_CHECK_SIZE  128
#define DEF_SIZE        (DEF_CHECK_SIZE * 2 * 1024)//256 k a block

#define DEF_SOFTWARE_SIZE_16M (16 * 1024 * 1024)// our flash must be 16M byts to Fit FPGA!
#define DEF_SOFTWARE_SIZE_8M (8 * 1024 * 1024)// our flash must be 16M byts to Fit FPGA!
#define DEF_SOFTWARE_OFFSET 0xa2



#define Lin_Dbg
#ifdef Lin_Dbg
    #define PDBG(fmt, args...)  printf(fmt, ## args)
    #else
    #define PDBG(fmt, args...) /* empty debug slot */
#endif

///////////////////////////////////////////////////////////////




//int gFdKey = -1;//mcu file descriptior

int gFdFile = -1;
int gHoleFileSize = -1;

int time_elapse(struct timeval t) {
    
    struct timeval now;
    int elapse = 0;
    gettimeofday(&now, NULL);
    elapse = 1000000 * (now.tv_sec - t.tv_sec) + now.tv_usec - t.tv_usec;
    return elapse;
}


/** gFdFile: file fd
 *  sizeYouWish: the size you wish
 *  return: ok, return zero, if large then sizeYouWish: return 1, 
 *  else if less then size YouWish return -1
 */
 
int check_file_size(int fd_file, int sizeYouWish)////(0x1000000 + 0xa2)
{

    if (lseek (fd_file, 0, SEEK_END) < sizeYouWish){
        PDBG("[%s]:%d, check file size Error! size should be more lagrger!\n", __func__, __LINE__);
		return -1;
    }else if (lseek (fd_file, 0, SEEK_END) > sizeYouWish){
        PDBG("[%s]:%d, check file size Error! size should be less then this!\n", __func__, __LINE__);
		return 1;
	}
	return 0;
}


int spi_setup_and_open_ttdev (void)
{
    int fd_mtd;
    int res, val;
    static uint8_t mode;
    static uint8_t bits = 8;
    static uint32_t speed = SPI_MAX_SPEED;
	mode = SPI_MODE_0;
	
    fd_mtd = spi_open("/dev/spidev4.1");
    if (fd_mtd < 0)
    {
        PDBG("spi_open is failed.\n");
        return -1;
    }
    PDBG("[%s]:%d using node /dev/spidev4.1\n", __func__, __LINE__);
    

    res = spi_set_mode(mode);
    if (res < 0)
    {
        PDBG("spi_set_mode is failed.\n");
        spi_close ();
        return -1;
    }

//    res = spi_set_bits(bits);
//    if (res < 0)
//    {
//        PDBG("spi_set_bits is failed. \n");
//        spi_close ();
//        return -1;
//    }

   res = spi_set_speed(speed);
    if (res < 0)
    {
        PDBG("spi_set_speed is failed. \n");
        spi_close ();
        return -1;
    }

	

    PDBG("[%s]:%d setup SPI ok\n", __func__, __LINE__);
    return 0;
}

int file_operate (const char *path, size_t *size)
{
    int fd;

    if (NULL == path || NULL == size)
    {
        return -1;
    }

    fd = open (path, O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }
    PDBG("[%s]:%d lseeking file \n", __func__, __LINE__);
    *size = lseek(fd, 0, SEEK_END);
    
    lseek(fd, 0, SEEK_SET);

    return fd;
}

/**
 * swap all bits!
 */
 
unsigned char convert (unsigned char num)
{
    unsigned char a = 0;
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        a = ((num >> i) & 0x01) | a << 1;
    }

    return a;
}



void read_spi_flash_ID(char *id)
{
    spi_rdid (id, 3);
    PDBG("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);
    if (id[0] != 0x20)
    {
        PDBG("[%s]%d: Can not find SPI FLASH. \n", __func__, __LINE__);
        exit(-1);
        return;
    }
    return;
}



/* int size: buffer size
 * 
 * 
 */
char *get_buffer_and_init(int size)
{
	//size -= SOFTWARE_OFFSET;// hole file to the end
    PDBG("[%s]%d: new size(useful data) is:%d\n",  __func__, __LINE__, size); //out FPGA size must be 16M Bytes!
	char *buff = malloc (size);//申请size大小的内存用于存放问及内容
    if (NULL == buff)
    {
		PDBG("[%s]%d: malloc return Error!\n", __func__, __LINE__);
        return NULL;
    }
	memset (buff, 0, size);//初始化申请的内容
	return buff;
}

/* wirte buffer data to spi flash
 *  SIZE MUST BE 16Mb
 * buff: buffer to be writen
 * size: the size to be writen
 * return: zero is ok, none zero error
 */
int write_all_spi_flash(char *buff, int size)
{
	int res = -1;
	int addr = 0;    
	while (addr < size)
    {
        spi_wait_ready();
        if ((size - addr) < DEF_SIZE)
        {
            res = spi_write(addr, buff + addr, size - addr);
            if (res < 0)
            {
                PDBG("[%s]:%d, Write Error 1, res: %d !\n", __func__, __LINE__, res);
                return addr;
            }
        }
        else 
        {
            res = spi_write(addr, buff + addr, DEF_SIZE);
            if (res < 0)
            {
                PDBG("[%s]:%d, Write Error2, res: %d !\n", __func__, __LINE__, res);
                return addr;
            }
        }

        addr += res;
        PDBG("[%s]:%d, Write flash addr = %x H, res: %d\n",  __func__, __LINE__, addr, res);
	}
	return 0;
}

int read_all_spi_flash(char *dst_buff, int size)
{
	int addr = 0;
	int res = -1;
	//read spi flash data
    while (addr < size)
    {
        if ((size - addr) < DEF_CHECK_SIZE)
        {
            res = spi_read(addr, dst_buff + addr, size - addr);
            if (res < 0)
            {
                PDBG("[%s]:%d, Read Error 1, res: %d !\n", __func__, __LINE__, res);
                return addr;
            } 
        }
        else
        {
	        res = spi_read(addr, dst_buff + addr, DEF_CHECK_SIZE);
            if (res < 0)
            {
                PDBG("[%s]:%d, Read Error 4K Bytes, res: %d !\n", __func__, __LINE__, res);
                return addr;
            }
        }
        addr += res;
        spi_wait_ready();
        PDBG("(Read!) flash addr = %x H\n", addr);
    }
    return 0;
}


/* fd_file: file descriptor
 * dst_buf: the dest buffer to read from file;
 * size: the size to read
 * 
 * 
 */
int read_file_to_buff(int fd_file, char * dst_buf, int size)
{
	int addr = 0;
	int res = -1;
	
	while (addr != size)//begin to read file from USB
    {
        res = read (fd_file, dst_buf + addr, size);
        if (res <= 0)
        {
            PDBG("xxx>> read file ERROR! addr: %x H, Actuly read file size = %d Bytes\n", addr, res);
            return addr;
        }
        addr += res;
        PDBG("read file final addr(size): %x H(%d), Actuly read file size = %d Bytes\n", addr, addr, res);
    }
    return 0;
}   

//main形式参数的用法main参数用法main函数形参argc argv用法
int main (int argc, char *argv[])
{
	int i, FlashNumber,ret;
	char *buff = NULL;
	char *buff1 = NULL;
    
    if (argc < 3){
        PDBG("Usage: %s <FilePath> Flash_Number(0 or 1)\n", argv[0]);
        return -1;
    }
    
	FlashNumber = atoi(argv[2]);
	if (1 < FlashNumber){
        PDBG("Usage: arg[2] is:%d, must be 0 or 1\n", FlashNumber);
        return -1;
    }
	
	
    spi_setup_and_open_ttdev();//open /dev/tt, open spi and config;
    
    gFdFile = file_operate (argv[1], &gHoleFileSize);//open file,get hole file size to gHoleFileSize
    if (gFdFile < 0) { PDBG("file_operate is failed. \n");return -1;}
    
    PDBG("[%s]:%d, gFdFile OK. gFdFile:%d, HoleFileSize:%d\n", __func__, __LINE__, 
		gFdFile, gHoleFileSize);
	
	if(check_file_size(gFdFile, (0x1000000 + 0xa2)) != 0) { ; }
	
    PDBG("[%s]:%d, begin to read id\n", __func__, __LINE__);
    
    // RDID, funny , must write lake that!!! 只能这样写,
    char id[3] = {0,0,0};
    read_spi_flash_ID(id);
    PDBG("[%s]:%d, ID reading finished\n", __func__, __LINE__);
    spi_wait_ready();

    // BE, block erase 擦除所有的flash
    spi_be(); 
    PDBG("Flsah Erasiing, Please Wait!\n * => Hole file size is %d \n", gHoleFileSize);
    spi_wait_ready();//wait for erase ok
    
		
	PDBG("[%s]:%d, begin to get_buffer_and_init\n", __func__, __LINE__);
	// malloc buffer for file
	if(0 == FlashNumber)//16 MB
		{
			buff = get_buffer_and_init(DEF_SOFTWARE_SIZE_16M);
			buff1 = get_buffer_and_init(DEF_SOFTWARE_SIZE_16M);
		}
	else if(1 == FlashNumber)//8 MB
		{
			buff = get_buffer_and_init(DEF_SOFTWARE_SIZE_8M);
			buff1 = get_buffer_and_init(DEF_SOFTWARE_SIZE_8M);
		}
		
    lseek(gFdFile, DEF_SOFTWARE_OFFSET, SEEK_SET);
	PDBG("[%s]:%d, lseek ok\n", __func__, __LINE__);

	if(0 == FlashNumber)//16 MB
	{
		if(read_file_to_buff(gFdFile, buff, DEF_SOFTWARE_SIZE_16M) != 0) { goto error_label; }
		PDBG("[%s]:%d, Finishing reading 16M file to buffer... \n", __func__, __LINE__);

		for (i = 0; i < DEF_SOFTWARE_SIZE_16M; i++){
			buff[i] = convert (buff[i]);
			}
		
		PDBG("[%s]:%d, 16M File buffer convertion is finished... \n\n\n", __func__, __LINE__);

		if(0 != write_all_spi_flash(buff, DEF_SOFTWARE_SIZE_16M)){ goto error_label; }
		PDBG("[%s]:%d, All Flash(16M) Writing is OK... \n", __func__, __LINE__);
		spi_wait_ready();
		if (0 != read_all_spi_flash(buff1, DEF_SOFTWARE_SIZE_16M)){ goto error_label; }
		PDBG("[%s]:%d, All Flash(16M) Reding is OK... \n", __func__, __LINE__);
		
//		for (i = 0; i < DEF_SOFTWARE_SIZE_16M; i++){
//			if(buff[i] != buff1[i]){
//				PDBG("[%s]:%d, not equal: buff[%d](%02x) != buff1[%d](%02x) ... \n", __func__, __LINE__,
//					i, buff[i], i, buff1[i]);
//					goto error_label;
//				}
//			}

		if( ret = memcmp(buff, buff1, DEF_SOFTWARE_SIZE_16M) != 0) { 
			PDBG("[%s]:%d, Verify Error(16M), Some thing wrong in Flash!, memcmp return:%d\n", ret);
			goto error_label;
		}
		PDBG("[%s]:%d, 16M File verify OK...\n", __func__, __LINE__);
	}else if(1 == FlashNumber){
		if(read_file_to_buff(gFdFile, buff, DEF_SOFTWARE_SIZE_8M) != 0) { goto error_label; }
		PDBG("[%s]:%d, Finishing reading 8M file to buffer... \n", __func__, __LINE__);

		for (i = 0; i < DEF_SOFTWARE_SIZE_8M; i++){
			buff[i] = convert (buff[i]);
			}
		
		PDBG("[%s]:%d, 8M buffer convertion finished... \n\n\n", __func__, __LINE__);

		if(0 != write_all_spi_flash(buff, DEF_SOFTWARE_SIZE_8M)){ goto error_label; }
		PDBG("[%s]:%d, All Flash(8M) Writing OK... \n", __func__, __LINE__);
		spi_wait_ready();
		if (0 != read_all_spi_flash(buff1, DEF_SOFTWARE_SIZE_8M)){ goto error_label; }
		PDBG("[%s]:%d, All Flash(8M) Reding OK... \n", __func__, __LINE__);

		if( ret = memcmp(buff, buff1, DEF_SOFTWARE_SIZE_8M) != 0) { 
			PDBG("[%s]:%d, Verify Error(8M), Some thing wrong in Flash!, memcmp return:%d\n", ret);
			goto error_label;
		}
		PDBG("[%s]:%d, 8M file Verify OK...\n", __func__, __LINE__);
	}

//------------------- fininsed spi operation, deal with result  --------------------//	
    PDBG("[%s]:%d, Main loop ! All OK .. \n", __func__, __LINE__);
    
	if(buff1)
		free (buff1);
	if(buff)
		free (buff);
    close (gFdFile);
    spi_close();

    return 0;

error_label:
    PDBG("[%s]:%d, Main loop ! ERROR LABEL .. \n", __func__, __LINE__);
    
	if(buff1)
		free (buff1);
	if(buff)
		free (buff);
		
    close (gFdFile);
    spi_close();

	return -1;
}
