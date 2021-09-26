#include <stdio.h>
#include "define.h"

#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utils/plat_log.h>
#include "media/mpi_sys.h"
#include "media/mm_comm_vi.h"
#include "media/mpi_vi.h"
#include "media/mpi_isp.h"

#define imHeight 1080
#define imWidth  1920

sint32 test_sys_Init()
{
    sint32 s32Ret;
    MPP_SYS_CONF_S stSysConf = {0};
    stSysConf.nAlignWidth = 32;
    s32Ret = AW_MPI_SYS_SetConf(&stSysConf);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_SYS_SetConf fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_SYS_Init();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_SYS_Init fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}

sint32 test_sys_Fini()
{
    sint32 s32Ret;

    s32Ret = AW_MPI_SYS_Exit();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_SYS_Exit fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}


sint32 test_vi_Init()
{
    sint32 s32Ret;
    sint32 ViDev = 0, ViChn = 0;
    sint32 iIspDev = 0;
    VI_ATTR_S stAttr = {0};
    
    stAttr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    stAttr.memtype = V4L2_MEMORY_MMAP;
    stAttr.format.pixelformat = V4L2_PIX_FMT_NV12M;
    //stAttr.format.pixelformat = V4L2_PIX_FMT_YUV420M;
    stAttr.format.field = V4L2_FIELD_ANY;
    stAttr.format.width = imWidth;
    stAttr.format.height = imHeight;
    stAttr.nbufs = 5;
    stAttr.nplanes = 2;
    stAttr.fps = 30;
    stAttr.use_current_win = 0;
    stAttr.wdr_mode = 0;
    stAttr.capturemode = V4L2_MODE_VIDEO;
    stAttr.drop_frame_num = 0;

    s32Ret = AW_MPI_VI_CreateVipp(ViDev);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_CreateVipp fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_SetVippAttr(ViDev, &stAttr);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_SetVippAttr fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_EnableVipp(ViDev);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_EnableVipp fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_ISP_Init();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_ISP_Init fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_ISP_Run(iIspDev);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_ISP_Run fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_CreateVirChn(ViDev, ViChn, NULL);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_CreateVirChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_SetVirChnAttr(ViDev, ViChn, NULL);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_SetVirChnAttr fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_EnableVirChn(ViDev, ViChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_EnableVirChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}


sint32 test_vi_Fini()
{
    sint32 s32Ret;
    sint32 ViDev = 0, ViChn = 0;
    sint32 iIspDev = 1;

    s32Ret = AW_MPI_VI_DisableVirChn(ViDev, ViChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_DisableVirChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_DestoryVirChn(ViDev, ViChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_DestoryVirChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }
    
    s32Ret = AW_MPI_ISP_Stop(iIspDev);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_ISP_Stop fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_ISP_Exit();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_ISP_Exit fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_DisableVipp(ViDev);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_DisableVipp fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_DestoryVipp(ViDev);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_DestoryVipp fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}


sint32 test_vi_GetFrame()
{
    sint32 ViDev = 0, ViChn = 0;
    sint32 s32Ret;
    VIDEO_FRAME_INFO_S stFrameInfo;
    static int count = 0;

    s32Ret = AW_MPI_VI_GetFrame(ViDev, ViChn, &stFrameInfo, -1);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_GetFrame fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    count++;
    print(COM_INFO, "Frame Height:%d, Width:%d\n", stFrameInfo.VFrame.mHeight, stFrameInfo.VFrame.mWidth);
    //print(COM_INFO, "VirAddr1:0x%x, VirAddr2:0x%x\n", stFrameInfo.VFrame.mpVirAddr[0], stFrameInfo.VFrame.mpVirAddr[1]);
    if(count % 30 == 0)
    {
        FILE *fd;
        char filename[128] = "./test_vi.yuv";
        //sprintf(filename, "/root/tmp/%d.yuv", count / 30);
        fd = fopen(filename, "wb");
        fwrite(stFrameInfo.VFrame.mpVirAddr[0], 1, stFrameInfo.VFrame.mWidth * stFrameInfo.VFrame.mHeight, fd);
        fwrite(stFrameInfo.VFrame.mpVirAddr[1], 1, stFrameInfo.VFrame.mWidth * stFrameInfo.VFrame.mHeight >> 1, fd);
        fclose(fd);
    }

    s32Ret = AW_MPI_VI_ReleaseFrame(ViDev, ViChn, &stFrameInfo);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_ReleaseFrame fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}


int main()
{
    sint32 s32Ret;
    sint32 count=300, i;

    s32Ret = test_sys_Init();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_sys_Init fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = test_vi_Init();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_vi_Init fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    for(i=0; i < count; i++)
    {
        s32Ret = test_vi_GetFrame();
        if(s32Ret != COM_SUCCESS)
        {
            print(COM_WARNNING, "test_vi_GetFrame fail! [0x%x]\n", s32Ret);
            continue;
        }
    }

    s32Ret = test_vi_Fini();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_vi_Fini fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = test_sys_Fini();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_sys_Fini fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }


    return COM_SUCCESS;
}




