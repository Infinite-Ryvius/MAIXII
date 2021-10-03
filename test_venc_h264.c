#include <stdio.h>
#include "define.h"
#include "avilib.h"

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
#include "media/mm_common.h"
#include "media/mm_comm_vi.h"
#include "media/mm_comm_venc.h"
#include "media/mpi_videoformat_conversion.h"
#include "media/mpi_isp.h"
#include "media/mpi_vi.h"
#include "media/mpi_venc.h"


#define imHeight 1080
#define imWidth  1920
#define imFps    30

void print_hex(char *buf, int len)
{
    int i;
    print(COM_INFO, "0x");
    for(i=0; i < len; i++)
    {
        printf("%02x", buf[i]);
    }
    printf("\n");
}

sint32 test_sys_Init()
{
    sint32 s32Ret;
    MPP_SYS_CONF_S stSysConf = {0};
    stSysConf.nAlignWidth = 8;
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
    stAttr.format.field = V4L2_FIELD_ANY;
    stAttr.format.width = imWidth;
    stAttr.format.height = imHeight;
    //stAttr.nbufs = 5;
    stAttr.nbufs = 4;
    stAttr.nplanes = 2;
    stAttr.fps = imFps;
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

    //s32Ret = AW_MPI_VI_EnableVirChn(ViDev, ViChn);
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

    //s32Ret = AW_MPI_VI_DisableVirChn(ViDev, ViChn);
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

sint32 test_venc_Init()
{
    sint32 s32Ret;
    sint32 VencChn = 0;
    VENC_CHN_ATTR_S VencAttr = {0};
    VENC_FRAME_RATE_S VencRate = {0};

    VencAttr.VeAttr.Type = PT_H264;
    VencAttr.VeAttr.MaxKeyInterval = 30; //I帧间隔
    VencAttr.VeAttr.SrcPicHeight = imHeight;
    VencAttr.VeAttr.SrcPicWidth = imWidth;
    VencAttr.VeAttr.Field = VIDEO_FIELD_FRAME;
    //VencAttr.VeAttr.Field = VIDEO_FIELD_INTERLACED;
    VencAttr.VeAttr.mColorSpace = V4L2_COLORSPACE_JPEG;
    //VencAttr.VeAttr.mColorSpace = V4L2_COLORSPACE_REC709;
    VencAttr.VeAttr.PixelFormat = map_V4L2_PIX_FMT_to_PIXEL_FORMAT_E(V4L2_PIX_FMT_NV12M);
    
    VencAttr.VeAttr.AttrH264e.bByFrame = TRUE;
    VencAttr.VeAttr.AttrH264e.PicHeight = imHeight;
    VencAttr.VeAttr.AttrH264e.PicWidth = imWidth;
    VencAttr.VeAttr.AttrH264e.mbPIntraEnable = TRUE;

    VencAttr.RcAttr.mRcMode = VENC_RC_MODE_H264CBR;
    VencAttr.RcAttr.mAttrH264Cbr.mBitRate = 4*1024*1024;

    //VencAttr.RcAttr.mRcMode = VENC_RC_MODE_MJPEGCBR;
    //VencAttr.RcAttr.mAttrMjpegeCbr.mBitRate = 4*1024*1024; // 1M码率
    

    s32Ret = AW_MPI_VENC_CreateChn(VencChn, &VencAttr);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VENC_CreateChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    VencRate.SrcFrmRate = imFps;
    VencRate.DstFrmRate = imFps;
    s32Ret = AW_MPI_VENC_SetFrameRate(VencChn, &VencRate);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VENC_SetFrameRate fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}

sint32 test_venc_Fini()
{
    sint32 s32Ret;
    sint32 VencChn = 0;

    s32Ret = AW_MPI_VENC_DestroyChn(VencChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VENC_DestroyChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}

sint32 test_sys_Bind()
{
    sint32 s32Ret;
    sint32 ViDev = 0, ViChn = 0;
    sint32 VencDev = 0, VencChn = 0;
    MPP_CHN_S MPP_CHN1 = {MOD_ID_VIU, ViDev, ViChn};
    MPP_CHN_S MPP_CHN2 = {MOD_ID_VENC, VencDev, VencChn};

    s32Ret = AW_MPI_SYS_Bind(&MPP_CHN1, &MPP_CHN2);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_SYS_Bind fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}

sint32 test_sys_unBind()
{
    sint32 s32Ret;
    sint32 ViDev = 0, ViChn = 0;
    sint32 VencDev = 0, VencChn = 0;
    MPP_CHN_S MPP_CHN1 = {MOD_ID_VIU, ViDev, ViChn};
    MPP_CHN_S MPP_CHN2 = {MOD_ID_VENC, VencDev, VencChn};

    s32Ret = AW_MPI_SYS_UnBind(&MPP_CHN1, &MPP_CHN2);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_SYS_UnBind fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}

sint32 test_frame_Start() //启动帧流
{
    sint32 ViDev = 0, ViChn = 0, VencChn = 0;
    sint32 s32Ret;

    s32Ret = AW_MPI_VI_EnableVirChn(ViDev, ViChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_EnableVirChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VENC_StartRecvPic(VencChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VENC_StartRecvPic fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}

sint32 test_frame_Stop()
{
    sint32 ViDev = 0, ViChn = 0, VencChn = 0;
    sint32 s32Ret;

    s32Ret = AW_MPI_VENC_StopRecvPic(VencChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VENC_StopRecvPic fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = AW_MPI_VI_DisableVirChn(ViDev, ViChn);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VI_DisableVirChn fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    return COM_SUCCESS;
}


sint32 test_venc_GetFrame()
{
    sint32 s32Ret, i, lens;
    sint32 VencChn = 0;
    int count = 400;
    VENC_STREAM_S stStream = {0};
    VENC_PACK_S venc_pack;
    VencHeaderData VencSpsPps;
    avi_t *hAVIFile;
    void *pData;
    sint32 bIFrame;

    hAVIFile = AVI_open_output_file("./test_venc_h264.avi");
    if(hAVIFile == NULL)
    {
        print(COM_ERROR, "AVI_open_output_file fail! \n");
        return COM_FAILURE;
    }

    AVI_set_video(hAVIFile, imWidth, imHeight, imFps, "h264");

    s32Ret = AW_MPI_VENC_GetH264SpsPpsInfo(VencChn, &VencSpsPps);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AW_MPI_VENC_GetStream fail! [0x%x]\n", s32Ret);
        return s32Ret;
    }

    s32Ret = AVI_write_frame(hAVIFile, VencSpsPps.pBuffer, VencSpsPps.nLength, 0);
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "AVI_write_frame fail! [0x%x]\n", s32Ret);
    }

    for(i = 0; i < count; i++)
    {
        stStream.mPackCount = 1;
        stStream.mpPack = &venc_pack;

        s32Ret = AW_MPI_VENC_GetStream(VencChn, &stStream, -1);
        if(s32Ret != COM_SUCCESS)
        {
            print(COM_ERROR, "AW_MPI_VENC_GetStream fail! [0x%x]\n", s32Ret);
            continue;
        }

        if(i == 0 && stStream.mpPack->mDataType.enH264EType != H264E_NALU_ISLICE)
        {
            i--;
            AW_MPI_VENC_ReleaseStream(VencChn, &stStream);
            continue;
        }

        lens = stStream.mpPack->mLen0 - stStream.mpPack->mOffset;
        pData = stStream.mpPack->mpAddr0 + stStream.mpPack->mOffset;
        bIFrame = (stStream.mpPack->mDataType.enH264EType == H264E_NALU_ISLICE) ? 1 : 0;

        print(COM_INFO, "addr0:0x%x, lens:%d, type:%d, bIframe:%d\n", stStream.mpPack->mpAddr0, 
            stStream.mpPack->mLen0, stStream.mpPack->mDataType.enH264EType, bIFrame);
        print_hex(stStream.mpPack->mpAddr0, 64);


        s32Ret = AVI_write_frame(hAVIFile, pData, lens, bIFrame);
        if(s32Ret != COM_SUCCESS)
        {
            print(COM_ERROR, "AW_MPI_VENC_ReleaseStream fail! [0x%x]\n", s32Ret);
            AW_MPI_VENC_ReleaseStream(VencChn, &stStream);
            continue;
        }

        s32Ret = AW_MPI_VENC_ReleaseStream(VencChn, &stStream);
        if(s32Ret != COM_SUCCESS)
        {
            print(COM_ERROR, "AW_MPI_VENC_ReleaseStream fail! [0x%x]\n", s32Ret);
            continue;
        }
    }

    AVI_close(hAVIFile);

    return COM_SUCCESS;
}

int main()
{
    sint32 s32Ret;

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

    s32Ret = test_venc_Init();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_venc_Init fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = test_sys_Bind();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_sys_Bind fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = test_frame_Start();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_frame_Start fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }


    s32Ret = test_venc_GetFrame();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_WARNNING, "test_venc_GetFrame fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }


    s32Ret = test_frame_Stop();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_frame_Stop fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = test_sys_unBind();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_sys_unBind fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
    }

    s32Ret = test_venc_Fini();
    if(s32Ret != COM_SUCCESS)
    {
        print(COM_ERROR, "test_venc_Fini fail! [0x%x]\n", s32Ret);
        return COM_FAILURE;
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




