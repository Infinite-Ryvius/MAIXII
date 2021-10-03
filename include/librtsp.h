#ifndef __LIBRTSP_H__
#define __LIBRTSP_H__

#ifdef __cplusplus //����һ���־��Ǹ��߱����������������__cplusplus(�������cpp�ļ��� 
extern "C"{ //��Ϊcpp�ļ�Ĭ�϶����˸ú�),�����C���Է�ʽ���б���
#endif

typedef int RtspGetFrame(void **pBuf, unsigned char*type, unsigned int *lens);
int RTSP_Init(int port, char *stream, RtspGetFrame *pgetframe);
int RTSP_Start();
int RTSP_Stop();

#ifdef __cplusplus //����һ���־��Ǹ��߱����������������__cplusplus(�������cpp�ļ��� 
}
#endif

#endif



