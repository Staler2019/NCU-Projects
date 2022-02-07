/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include "stdio.h"
#include "..\common.h"
#include "..\..\Utilities\STM32_EVAL\Common\stm32_eval_spi_sd.h"
#include "..\..\Libraries\STM32F10x_StdPeriph_Driver\inc\stm32f10x_rtc.h"
#include "time.h"

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
void disk_initialize_p(void){};
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
    u8 state;

    if(drv)
    {
        return STA_NOINIT;  //僅支持磁盤0的操作
    }
    printf("SD_Init\n\r");
    state = SD_Init(); //Initializes the SD/SD communication.
    if(state != SD_RESPONSE_NO_ERROR) //Sequence failed
    {
    	printf("STA_NODISK\n\r");
        return STA_NOINIT;
    }
//    else if(state != 0)
//    {
//        return STA_NOINIT;  //其他錯誤：初始化失敗
//    }
    else
    {
        return 0;           //初始化成功
    }
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
void disk_status_p(void){};
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
    if(drv)
    {
        return STA_NOINIT;  //僅支持磁盤0操作
    }

    //檢查SD卡是否插入
    if(!SD_Detect())
    {
        return STA_NODISK;
    }
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
void disk_read_p(void){};
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{

	SD_Error res = SD_RESPONSE_NO_ERROR;
	printf("[disk_read]disk_read\n\r");
    if (drv || !count)
    {
    	printf("[disk_read]RES_PARERR Failed\n\r");
        return RES_PARERR;  //僅支持單磁盤操作，count不能等於0，否則返回參數錯誤
    }
    if(!SD_Detect())
    {
        printf("[disk_read]SD_Detect Failed\n\r");
        return RES_NOTRDY;  //沒有檢測到SD卡，報NOT READY錯誤
    }

    if(count==1)            //1個sector的讀操作
    {
        res = SD_ReadBlock(buff,sector,SD_DATA_SIZE);
    }
    else                    //多個sector的讀操作
    {
        res = SD_ReadMultiBlocks(buff,sector,SD_DATA_SIZE, count);
    }
	/*
    do
    {
        if(SD_ReadBlock(sector, buff)!=0)
        {
            res = 1;
            break;
        }
        buff+=512;
    }while(--count);
    */
    //處理返回值，將SPI_SD_driver.c的返回值轉成ff.c的返回值
    if(res == SD_RESPONSE_NO_ERROR)
    {
        printf("[disk_read]disk_read End\n\r");
		printf("\r\n SELECT * from T_Value \r\n");  

        return RES_OK;
    }
    else
    {
    	printf("[disk_read]SD_Read Failed\n\r");
        return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
void disk_write_p(void){};
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	SD_Error res;

    printf("[disk_write]disk_write\n\r");
    if (drv || !count)
    {
        return RES_PARERR;  //僅支持單磁盤操作，count不能等於0，否則返回參數錯誤
    }
    if(!SD_Detect())
    {
        return RES_NOTRDY;  //沒有檢測到SD卡，報NOT READY錯誤
    }

    // 讀寫操作
    if(count == 1)
    {
        res = SD_WriteBlock(buff,sector,SD_DATA_SIZE);
    }
    else
    {
        res = SD_WriteMultiBlocks(buff,sector,SD_DATA_SIZE, count);
    }
    // 返回值轉換
    if(res == SD_RESPONSE_NO_ERROR)
    {
        printf("[disk_write]disk_write End\n\r");
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
void disk_ioctl_p(void){};
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;
    SD_Error errorstatus = SD_RESPONSE_NO_ERROR;
    SD_CardInfo sdcardinfo;


    if (drv)
    {
        return RES_PARERR;  //僅支持單磁盤操作，否則返回參數錯誤
    }

    //FATFS目前版本僅需處理CTRL_SYNC，GET_SECTOR_COUNT，GET_BLOCK_SIZ三個命令
    switch(ctrl)
    {
    case CTRL_SYNC:
        SD_CS_LOW();
//        if(SD_WaitReady()==0)
//        {
            res = RES_OK;
//        }
//        else
//        {
//            res = RES_ERROR;
//        }
        SD_CS_HIGH();
        break;

    case GET_BLOCK_SIZE:
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        errorstatus = SD_GetCardInfo(&sdcardinfo);
        if (errorstatus == SD_RESPONSE_NO_ERROR)
            res = RES_OK;
        else
            res = RES_ERROR;
        *(DWORD*)buff = sdcardinfo.CardCapacity;
        break;
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}


/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
void get_fattime_p(void){};
DWORD get_fattime (void)
{
    struct tm t;
    DWORD date;
	time_t t_t;
	struct tm *t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = localtime(&t_t);
	t_tm->tm_year += 1900;	//localtime轉換結果的tm_year是相對值，需要轉成絕對值
	t= *t_tm;

    t.tm_year -= 1980;		//年份改為1980年起
    t.tm_mon++;         	//0-11月改為1-12月
    t.tm_sec /= 2;      	//將秒數改為0-29

    date = 0;
    date = (t.tm_year << 25)|(t.tm_mon<<21)|(t.tm_mday<<16)|\
            (t.tm_hour<<11)|(t.tm_min<<5)|(t.tm_sec);

    return date;
}





