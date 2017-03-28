/*
 * General dma for Phascan and Phascan-II
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>

#include <linux/timer.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pm.h>
#include <linux/delay.h>

#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
#include <plat/dma.h>
#include <plat/gpmc.h>
#endif

#if (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
#include <linux/platform_data/dma-imx.h>
#include <linux/platform_data/dma-imx-sdma.h>
#endif

#include <linux/io.h>
#include <linux/slab.h>
#include <linux/mman.h>

#include <linux/workqueue.h>

#define VERSION "20170327"

#define	FB_FRAME_SIZE	  768
#define DATA_FRAME_SIZE  1024
#define BUFF_FRAME_SIZE   512
#define FB_FRAME_COUNT    400
#define DATA_FRAME_COUNT   96
#define BUFF_FRAME_COUNT 1536

#define FB_DMA_SIZE     768 * 400 * 2

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
#define DMA_SOURCE_ADDR          0x28000000 

#define DMA_START_ADDR           0x8f000000     // 240 -- 256M
#define DMA_DATA_LENGTH          0x01000000     // 16M

#define BUFF_START_ADDR          0x90000000
#define BUFF_DATA_LENGTH         0x10000000
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
#define DMA_SOURCE_ADDR          0x30000000

#define DMA_START_ADDR           0x38000000     // 496 -- 512M
#define DMA_DATA_LENGTH          0x08000000     // 16M

#define BUFF_START_ADDR          0x40000000
#define BUFF_DATA_LENGTH         0x08000000
#endif

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
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    int            ch;		/* Assigned DMA channel number */
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    struct dma_chan *ch;
    struct dma_slave_config dma_m2m_config;
    struct dma_async_tx_descriptor *dma_m2m_desc;
    struct completion dma_m2m_ok;
#endif
    unsigned int   phys_from;
    unsigned int   phys_to;
    unsigned short data_type;	/* Data length */
    unsigned short frame_size;	/* Elements to transfer */
    unsigned short frame_count;	/* Framses to transfer */
    unsigned int   status;		/* Callback status */
};

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
static void dmatest_work(unsigned long data);
static void dma_memcpy_callback_from_fpga(int lch, u16 ch_status, void *data);
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
static int  dmatest_work(void *data);
static void dma_memcpy_callback_from_fpga(void *data);
#endif
static int  dma_mem_transfer_to_store_buffer(void) ;

static unsigned int  data_addr ;
static unsigned int  buff_addr ;
//static struct dma_transfer dma_video  ;
static struct dma_transfer dma_data   ; 
static struct dma_transfer dma_buffer ;

static int bDmaStoreProcessing  = 0 ;

#if (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
static bool dma_m2m_filter (struct dma_chan *chan, void *param)
{
    if (!imx_dma_is_general_purpose(chan))
        return false;
    chan->private = param;
    return true;
}
#endif

// dma data to buffer 0x90000000
// dma finish callback function
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
static void dma_memcpy_callback_to_buffer(int lch, u16 ch_status, void *data)
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
static void dma_memcpy_callback_to_buffer(void *data)
#endif
{
printk ("xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);
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
printk ("xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);

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
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
        /* set dma frame size and frame count */
        omap_set_dma_transfer_params(dma->ch, dma->data_type, dma->frame_size,
                                     dma->frame_count, OMAP_DMA_SYNC_BLOCK,
                                     OMAP24XX_DMA_NO_DEVICE , 0);
#endif
    }
    /* set dma source address */
    dma->phys_from   = OFFSET_ADDR[DataDmaCounter & 0x00000003] ;

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    omap_set_dma_src_params(dma->ch, 0,
                            OMAP_DMA_AMODE_SINGLE_IDX,
                            dma->phys_from, 1, 1); /*^^^^^^^^^^*/
    /* set dma destination */
    omap_set_dma_dest_params(dma->ch, 0,
                             OMAP_DMA_AMODE_SINGLE_IDX,
                             dma->phys_to, 1, 1);  /*^^^^^^^^*/
    omap_start_dma(dma->ch);
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma->dma_m2m_config.direction = DMA_MEM_TO_MEM;
    dma->dma_m2m_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
    dmaengine_slave_config(dma->ch, &dma->dma_m2m_config);
    dma->dma_m2m_desc = dma->ch->device->device_prep_dma_memcpy(dma->ch,
                                        dma->phys_to, dma->phys_from,
                                        dma->data_type * dma->frame_size * dma->frame_count,0);
    dma->dma_m2m_desc->callback = dma_memcpy_callback_to_buffer;
    dmaengine_submit(dma->dma_m2m_desc);
    dma_async_issue_pending (dma->ch);
#endif
}

// dma to buffer channel init
static int dma_mem_transfer_to_store_buffer(void)
{
    struct dma_transfer* dma = &dma_buffer ;
printk ("xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);

#if (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma_cap_mask_t dma_m2m_mask;
    struct imx_dma_data m2m_dma_data = {0};

    dma_cap_zero (dma_m2m_mask);
    dma_cap_set (DMA_SLAVE, dma_m2m_mask);
    m2m_dma_data.peripheral_type = IMX_DMATYPE_MEMORY;
    m2m_dma_data.priority = DMA_PRIO_HIGH;
#endif

    memset(dma, 0, sizeof(struct dma_transfer));
    dma->frame_count =  192 ;
    dma->phys_from   =  DMA_START_ADDR   ;
    dma->phys_to     =  BUFF_START_ADDR  ;

    dma->frame_size  =  DATA_SAVE_BLOCK_SIZE_WORD  ;
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    dma->data_type   = OMAP_DMA_DATA_TYPE_S16;
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma->data_type   = 0x01;
#endif
    dma->status      = 0;

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    /* Get a DMA channel */
    if (omap_request_dma(OMAP24XX_DMA_NO_DEVICE, "mem to buffer",
                         dma_memcpy_callback_to_buffer, (void *)dma, &dma->ch)) {
        printk(KERN_ERR "Could not get DMA with omap_request_dma()  mem to buffer \n");
        return -ENOMEM;
    }

    /* Transfer len = (data_type * elem_count * frame_count) */
    omap_set_dma_transfer_params(dma->ch, dma->data_type, dma->frame_size,
                                 dma->frame_count, OMAP_DMA_SYNC_BLOCK,
                                 OMAP24XX_DMA_NO_DEVICE, 0);
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma->ch = dma_request_channel(dma_m2m_mask, dma_m2m_filter, &m2m_dma_data);
    if (!dma->ch) {
        printk(KERN_ERR "Could not get DMA with dma_request_channel()\n");
        return -ENOMEM;
    }
#endif

    return 0;
}

/*
 * The callback gets called by the DMA interrupt handler after
 * the transfer is complete.
 */
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
static void dma_memcpy_callback_from_fpga(int lch, u16 ch_status, void *data)
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
static void dma_memcpy_callback_from_fpga(void *data)
#endif
{
    DmaFrameBuffer = 0xfffffff ;
printk ("xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);

    dma_to_store_buffer() ;
    do {
        config[1]++ ;
        OFFSET = config[1] & 0x00000003 ;
    } while(OFFSET == config[2]) ;

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    if(dma_data.frame_count != StoreFrameCount) {
        dma_data.frame_count = StoreFrameCount ;
        omap_set_dma_transfer_params(dma_data.ch, dma_data.data_type,  dma_data.frame_size,
                                     dma_data.frame_count, OMAP_DMA_SYNC_BLOCK,
                                     OMAP34XX_DMA_EXT_DMAREQ3 , 0);
    }
    dma_data.phys_to  =  OFFSET_ADDR[OFFSET];
    omap_set_dma_dest_params(dma_data.ch, 0, OMAP_DMA_AMODE_SINGLE_IDX, dma_data.phys_to, 1, 1);
    omap_start_dma(dma_data.ch);
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    if (dma_data.frame_count != StoreFrameCount) {
        dma_data.frame_count = StoreFrameCount ;
    }
    dma_data.phys_to = OFFSET_ADDR[OFFSET];

    dma_data.dma_m2m_config.direction = DMA_MEM_TO_MEM;
    dma_data.dma_m2m_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
    dmaengine_slave_config(dma_data.ch, &dma_data.dma_m2m_config);
    dma_data.dma_m2m_desc = dma_data.ch->device->device_prep_dma_memcpy(dma_data.ch,
                                        dma_data.phys_to, dma_data.phys_from,
                                        dma_data.data_type * dma_data.frame_size * dma_data.frame_count,0);
    dma_data.dma_m2m_desc->callback = dma_memcpy_callback_from_fpga;
    dmaengine_submit(dma_data.dma_m2m_desc);
    dma_async_issue_pending (dma_data.ch);
#endif
    
    return ;
}

static int dma_mem_transfer_from_fpga (void)
{
    struct dma_transfer* dma = &dma_data ;

printk ("xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);

#if (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma_cap_mask_t dma_m2m_mask;
    struct imx_dma_data m2m_dma_data = {0};

    dma_cap_zero (dma_m2m_mask);
    dma_cap_set (DMA_SLAVE, dma_m2m_mask);
    m2m_dma_data.peripheral_type = IMX_DMATYPE_MEMORY;
    m2m_dma_data.priority = DMA_PRIO_HIGH;
#endif
    memset(&dma_data, 0, sizeof(struct dma_transfer));
    dma->phys_from	= DMA_SOURCE_ADDR;
    dma->phys_to	= DMA_START_ADDR ;
    
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    dma->data_type = OMAP_DMA_DATA_TYPE_S16; 
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma->data_type = 0x01;
#endif

    dma->status = 0;
    dma->frame_size  =  DATA_SAVE_BLOCK_SIZE_WORD ; //DATA_FRAME_SIZE ;
    dma->frame_count =  192; //DATA_FRAME_COUNT;

    /* Get a DMA channel */
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    if (omap_request_dma(OMAP34XX_DMA_EXT_DMAREQ3, "gpmc to mem",
                         dma_memcpy_callback_from_fpga, (void *)dma, &dma->ch)) {
        printk(KERN_ERR "Could not get DMA with omap_request_dma()\n");
        return -ENOMEM;
    }
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma->ch = dma_request_channel(dma_m2m_mask, dma_m2m_filter, &m2m_dma_data);
    if (!dma->ch) {
        printk(KERN_ERR "Could not get DMA with dma_request_channel()\n");
        return -ENOMEM;
    }
#endif

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    /* Transfer len = (data_type * elem_count * frame_count) */
    omap_set_dma_transfer_params(dma->ch, dma->data_type, dma->frame_size,
                                 dma->frame_count, OMAP_DMA_SYNC_BLOCK,
                                 OMAP34XX_DMA_EXT_DMAREQ3 , 0);

    /* Source EMFIFF, increase source address after each transfer */
    omap_set_dma_src_params(dma->ch, 0, OMAP_DMA_AMODE_SINGLE_IDX, dma->phys_from, 1, 1);
    /* Destination EMIFF, increase dest address after each transfer */
    omap_set_dma_dest_params(dma->ch, 0 , OMAP_DMA_AMODE_SINGLE_IDX, dma->phys_to, 1, 1);
    
    omap_start_dma(dma->ch);
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    dma->dma_m2m_config.direction = DMA_MEM_TO_MEM;
    dma->dma_m2m_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
    dmaengine_slave_config(dma->ch, &dma->dma_m2m_config);
    dma->dma_m2m_desc = dma->ch->device->device_prep_dma_memcpy(dma->ch,
                                        dma->phys_to, dma->phys_from,
                                        dma->data_type * dma->frame_size * dma->frame_count,0);
    dma->dma_m2m_desc->callback = dma_memcpy_callback_from_fpga;
    dmaengine_submit(dma->dma_m2m_desc);
    dma_async_issue_pending (dma->ch);
#endif

    return 0;
}

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
static void dmatest_work (unsigned long data)
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
static int  dmatest_work (void *data)
#endif
{
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    struct thread_data * thread = (struct thread_data *)data;

    /* Wait for 10 secs before starting */
    daemonize("%s", thread->name);
#endif

printk ("xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);
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
    // DMA frome gpmc/eim to Main memory
    dma_mem_transfer_from_fpga();
    // DMA data to store buffer
    dma_mem_transfer_to_store_buffer() ;

#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    return ;
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    return 0;
#endif
}

static char *name = "dmatest";

static int __init dmatest_init(void)
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
#if (LINUX_VERSION_CODE == KERNEL_VERSION (2, 6, 32))
    thread->pid = kernel_thread((void *)dmatest_work, thread, 0);
#elif (LINUX_VERSION_CODE == KERNEL_VERSION (4, 1, 18))
    kthread_run (dmatest_work, thread, thread->name);
#endif
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

static void __exit dmatest_exit(void)
{
    return;
}

MODULE_AUTHOR("Doppler");
MODULE_DESCRIPTION("Phascan DMA module");
MODULE_LICENSE("Dual BSD/GPL");
module_init(dmatest_init);
module_exit(dmatest_exit);
