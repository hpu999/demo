//#include <linux/config.h>
#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pm.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <linux/device.h>
#include <linux/dma-mapping.h>

#include <linux/io.h>

#include <plat/dma.h>
#include <plat/gpmc.h>
#include <linux/timer.h>

#include <linux/workqueue.h> //

#define VERSION "20111015"

#define	FB_FRAME_SIZE	  768
#define DATA_FRAME_SIZE  1024
#define BUFF_FRAME_SIZE   512
#define FB_FRAME_COUNT    400
#define DATA_FRAME_COUNT   96
#define BUFF_FRAME_COUNT 1536

#define FB_DMA_SIZE     768 * 400 * 2

#define DMA_SOURCE_ADDR          0x28000000 

#define DMA_START_ADDR           0x8f000000     // 240 -- 256M
#define DMA_DATA_LENGTH          0x01000000     // 16M

#define BUFF_START_ADDR          0x90000000
#define BUFF_DATA_LENGTH         0x10000000

#define CONFIG_START_ADDR_OFFSET 0x00100000
#define SCAN_DATA_MARK_OFFSET    0x00200000
#define CONFIG_DATA_LENGTH       0x00000100

#define REGION_SIZE              0x00040000     // 256K
#define DATA_SAVE_BLOCK_SIZE_BIT     1024
#define DATA_SAVE_BLOCK_SIZE_WORD     512



#define DmaFrameBuffer         config[0]
#define DataDmaCounter         config[1]
#define BufferInUse            config[2]
#define ScanSource             config[3]
#define StoreFrameCount        config[4]
#define EncoderCounterOffset   config[5]
#define StepsPerResolution     config[6]
#define ScanZeroIndexOffset    config[7]
#define MaxStoreIndex          config[8]
#define ScanTimmerCounter      config[9]
#define ScanTimmerCircled      config[10] 

volatile unsigned char* scan_mark  ;
volatile int* config;            // config[0]  DRAW condition
// config[1]  DMA counter
// config[2]  which Buffer of the four is in use by ARM
// config[3]  scan source 0: timmer 1: encoder
// config[4]  DMA DATA SIZE  (512B per unit)
// config[5]  PointQty + DMA_OFFSET + 4(5) * 4  (encoder data position)
// config[6]  StepPerResolution
// config[7]  Scan Zero Position Offset
// config[8]  Max store index
// config[9]  Scan Timer counter
// config[10] whether the data counter is circled in 256M memory



int OFFSET   ;
int OFFSET_ADDR[4] = {
    DMA_START_ADDR ,
    DMA_START_ADDR +     REGION_SIZE ,
    DMA_START_ADDR + 2 * REGION_SIZE ,
    DMA_START_ADDR + 3 * REGION_SIZE
} ;

static unsigned int video_phys_to = 0x809ac000;
module_param(video_phys_to, int, S_IRUGO)     ;

struct thread_data {
    int nr;
    pid_t pid;
    char * name;
};

struct dma_transfer {
    int            ch;		/* Assigned DMA channel number */
    unsigned int   phys_from;
    unsigned int   phys_to;
    unsigned short data_type;	/* Data length */
    unsigned short frame_size;	/* Elements to transfer */
    unsigned short frame_count;	/* Framses to transfer */
    unsigned int   status;		/* Callback status */
};

static void omap_dmatest_work(unsigned long data);
static void omap_dma_memcpy_callback_from_fpga(int lch, u16 ch_status, void *data);
static int  omap_dma_mem_transfer_to_store_buffer(void) ;

static unsigned int  data_addr ;
static unsigned int  buff_addr ;
//static struct dma_transfer dma_video  ;
static struct dma_transfer dma_data   ; 
static struct dma_transfer dma_buffer ;

static int bDmaStoreProcessing  = 0 ;

// dma data to buffer 0x90000000
// dma finish callback function
static void omap_dma_memcpy_callback_to_buffer(int lch, u16 ch_status, void *data)
{
    //printk("<0>DMA TO BUFFER!\n");
    //printk("<0>%d,%d,%d,%d,%d,%d,%d\n",ScanSource ,StoreFrameCount ,EncoderCounterOffset ,
    //          StepsPerResolution, ScanZeroIndexOffset, MaxStoreIndex , ScanTimmerCounter);
    bDmaStoreProcessing = 0 ;
    ScanTimmerCounter++ ;
}

// dma data to setted buffer address
static void dma_to_store_buffer(void)
{
    int  EncoderIndex   ;
    int* pEncoderIndex  ;
    int  nOffset        ;

    struct dma_transfer* dma = &dma_buffer ;
    if(bDmaStoreProcessing) {
        return ;
    }
    bDmaStoreProcessing = 1 ;

    if(ScanSource) {
        /* encoder */
        nOffset  = DataDmaCounter & 0x00000003 ;
        pEncoderIndex = & EncoderIndex ;
        memcpy((void*)pEncoderIndex , (void*)(nOffset *  REGION_SIZE + EncoderCounterOffset + data_addr) , 4);
        EncoderIndex  = EncoderIndex / StepsPerResolution + ScanZeroIndexOffset;
        if(EncoderIndex > MaxStoreIndex || EncoderIndex < 0) {
            /* out of range , do not dma */
            bDmaStoreProcessing = 0 ;
            return ;
        }
        scan_mark[EncoderIndex] = 0xff ;
        nOffset  = EncoderIndex * StoreFrameCount * DATA_SAVE_BLOCK_SIZE_BIT + BUFF_START_ADDR  ;
        dma->phys_to  =  nOffset  ;    // address to store
    } else {
        /* time */
        if(ScanTimmerCounter > MaxStoreIndex) {
            /* out of range , restart from 0x90000000 */
            ScanTimmerCounter = 0 ;
            ScanTimmerCircled++ ;
        }
        scan_mark[ScanTimmerCounter]  = 0xff ;
        dma->phys_to   = ScanTimmerCounter * StoreFrameCount * DATA_SAVE_BLOCK_SIZE_BIT + BUFF_START_ADDR;    // address to store
    }

    if(dma->frame_count != StoreFrameCount) {
        /* frame count */
        dma->frame_count = StoreFrameCount  ;
        /* set dma frame size and frame count */
        omap_set_dma_transfer_params(dma->ch, dma->data_type, dma->frame_size,
                                     dma->frame_count, OMAP_DMA_SYNC_BLOCK,
                                     OMAP24XX_DMA_NO_DEVICE , 0);
    }
    /* set dma source address */
    dma->phys_from   = OFFSET_ADDR[DataDmaCounter & 0x00000003] ;
    omap_set_dma_src_params(dma->ch, 0,
                            OMAP_DMA_AMODE_SINGLE_IDX,
                            dma->phys_from, 1, 1); /*^^^^^^^^^^*/
    /* set dma destination */
    omap_set_dma_dest_params(dma->ch, 0,
                             OMAP_DMA_AMODE_SINGLE_IDX,
                             dma->phys_to, 1, 1);  /*^^^^^^^^*/
    
    omap_start_dma(dma->ch);
}

// dma to buffer channel init
static int omap_dma_mem_transfer_to_store_buffer(void)
{
    struct dma_transfer* dma = &dma_buffer ;
    memset(dma, 0, sizeof(struct dma_transfer));
    dma->frame_size  =  DATA_SAVE_BLOCK_SIZE_WORD  ;
    dma->frame_count =  192 ;
    dma->phys_from   =  DMA_START_ADDR   ;
    dma->phys_to     =  BUFF_START_ADDR  ;
    
    dma->data_type   = OMAP_DMA_DATA_TYPE_S16;
    dma->status      = 0;

    /* Get a DMA channel */
    if (omap_request_dma(OMAP24XX_DMA_NO_DEVICE, "mem to buffer",
                         omap_dma_memcpy_callback_to_buffer, (void *)dma, &dma->ch)) {
        printk(KERN_ERR "Could not get DMA with omap_request_dma()  mem to buffer \n");
        return -ENOMEM;
    }

    /* Transfer len = (data_type * elem_count * frame_count) */
    omap_set_dma_transfer_params(dma->ch, dma->data_type, dma->frame_size,
                                 dma->frame_count, OMAP_DMA_SYNC_BLOCK,
                                 OMAP24XX_DMA_NO_DEVICE, 0);

    return 0;
}

/*
 * The callback gets called by the DMA interrupt handler after
 * the transfer is complete.
 */
static void omap_dma_memcpy_callback_from_fpga(int lch, u16 ch_status, void *data)
{
    DmaFrameBuffer = 0xfffffff ;

    dma_to_store_buffer() ;
    do {
        config[1]++ ;
        OFFSET = config[1] & 0x00000003 ;
    } while(OFFSET == config[2]) ;
    
    if(dma_data.frame_count != StoreFrameCount) {
        dma_data.frame_count = StoreFrameCount ;
        omap_set_dma_transfer_params(dma_data.ch, dma_data.data_type,  dma_data.frame_size,
                                     dma_data.frame_count, OMAP_DMA_SYNC_BLOCK,
                                     OMAP34XX_DMA_EXT_DMAREQ3 , 0);
    }
    dma_data.phys_to  =  OFFSET_ADDR[OFFSET];
    omap_set_dma_dest_params(dma_data.ch, 0, OMAP_DMA_AMODE_SINGLE_IDX, dma_data.phys_to, 1, 1);
    omap_start_dma(dma_data.ch);
    
    return ;
}

static int omap_dma_mem_transfer_from_fpga (void)
{
    struct dma_transfer* dma = &dma_data ;
    memset(&dma_data, 0, sizeof(struct dma_transfer));
    dma->phys_from	= DMA_SOURCE_ADDR;
    dma->phys_to	= DMA_START_ADDR ;
    
    dma->data_type = OMAP_DMA_DATA_TYPE_S16;
    dma->status = 0;

    dma->frame_size  =  DATA_SAVE_BLOCK_SIZE_WORD ; //DATA_FRAME_SIZE ;
    dma->frame_count =  192; //DATA_FRAME_COUNT;

    /* Get a DMA channel */
    if (omap_request_dma(OMAP34XX_DMA_EXT_DMAREQ3, "gpmc to mem",
                         omap_dma_memcpy_callback_from_fpga, (void *)dma, &dma->ch))
    {
        printk(KERN_ERR "Could not get DMA with omap_request_dma()\n");
        return -ENOMEM;
    }
    /* Transfer len = (data_type * elem_count * frame_count) */
    omap_set_dma_transfer_params(dma->ch, dma->data_type, dma->frame_size,
                                 dma->frame_count, OMAP_DMA_SYNC_BLOCK,
                                 OMAP34XX_DMA_EXT_DMAREQ3 , 0);

    /* Source EMFIFF, increase source address after each transfer */
    omap_set_dma_src_params(dma->ch, 0, OMAP_DMA_AMODE_SINGLE_IDX, dma->phys_from, 1, 1);
    /* Destination EMIFF, increase dest address after each transfer */
    omap_set_dma_dest_params(dma->ch, 0 , OMAP_DMA_AMODE_SINGLE_IDX, dma->phys_to, 1, 1);
    
    omap_start_dma(dma->ch);
    return 0;
}

static void omap_dmatest_work(unsigned long data)
{
    struct thread_data * thread = (struct thread_data *)data;

    /* Wait for 10 secs before starting */
    daemonize("%s", thread->name);
    allow_signal(SIGTERM);
    current->state = TASK_INTERRUPTIBLE;
    do{
        schedule_timeout(HZ * 1);
    }while(!StoreFrameCount) ;
    current->state = TASK_RUNNING;
    /* FIXME: Does not free mem in SIGTERM currently */
    if (signal_pending(current)) {
        flush_signals(current);
    }

    /* Set up transfer data */
    // DMA frome gpmc to Main memory
    omap_dma_mem_transfer_from_fpga();
    // DMA data to store buffer
    omap_dma_mem_transfer_to_store_buffer() ;

    return ;
}

static char *name = "dmatest";

static int __init omap_dmatest_init(void)
{
    struct thread_data * thread;
    printk("<0>DOPPLER DMA MODULE START!\n");
    /* Schedule multiple concurrent dma tests */
    thread = kmalloc(sizeof(struct thread_data), GFP_KERNEL);
    if (!thread) {
        goto free_threads;
    }

    memset(thread, 0, sizeof(struct thread_data));
    thread->nr = 1;
    thread->name = name;

    /* Schedule the test thread */
    thread->pid = kernel_thread((void *)omap_dmatest_work, thread, 0);

    request_mem_region(DMA_START_ADDR, DMA_DATA_LENGTH, "dma_data");
    data_addr = (unsigned int )ioremap(DMA_START_ADDR, DMA_DATA_LENGTH);
    memset((void*)data_addr , 0 , 0x100100) ;

    request_mem_region(BUFF_START_ADDR, BUFF_DATA_LENGTH , "buffer_data");
    buff_addr = (unsigned int )ioremap( BUFF_START_ADDR , BUFF_DATA_LENGTH );

    config = (unsigned int*)(data_addr + CONFIG_START_ADDR_OFFSET);
    scan_mark = (char*)(data_addr + SCAN_DATA_MARK_OFFSET)  ;

    printk("<0>config addr %x  data_addr %x \n", (int)config , (int)data_addr);
    printk("<0>dma module is running !\n");
    return 0;

free_threads:
    kfree(thread);

    return -ENOMEM;
}

static void __exit omap_dmatest_exit(void)
{
    return;
}

MODULE_AUTHOR("Doppler");
MODULE_DESCRIPTION("Phascan DMA module");
MODULE_LICENSE("Dual BSD/GPL");
module_init(omap_dmatest_init);
module_exit(omap_dmatest_exit);
