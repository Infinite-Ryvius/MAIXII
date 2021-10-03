#include "xop/RtspServer.h"
#include "net/Timer.h"
#include <thread>
#include <memory>
#include <iostream>
#include <string>
#include "librtsp.h"



typedef struct tagRtspInfo_S
{
    int port;
    int client;
    int bRunning;
    RtspGetFrame *pGetFrame;
    std::thread *pthread;
    xop::MediaSession *session;
    std::shared_ptr<xop::EventLoop> *pevent_loop;
    std::shared_ptr<xop::RtspServer> server;
    xop::MediaSessionId session_id;
} RTSP_INFO_S;

RTSP_INFO_S stRtspInfo;

void SendFrameThread(xop::RtspServer* rtsp_server, xop::MediaSessionId session_id, int& clients);

int RTSP_Init(int port, char *stream, RtspGetFrame *pgetframe)
{
    if(port == NULL || stream == NULL || pgetframe == NULL)
    {
        return -1;
    }

    std::string rtsp_url = "rtsp://127.0.0.1:554/live";

    stRtspInfo.client = 0;
    stRtspInfo.port = port;
    stRtspInfo.pGetFrame = pgetframe; 
    stRtspInfo.pevent_loop = new std::shared_ptr<xop::EventLoop>(new xop::EventLoop());  
    stRtspInfo.server = xop::RtspServer::Create(stRtspInfo.pevent_loop->get());
    if(!stRtspInfo.server->Start("0.0.0.0", stRtspInfo.port))
    {
        return -1;
    }
#ifdef AUTH_CONFIG
	stRtspInfo.server->SetAuthConfig("-_-", "admin", "12345");
#endif
    stRtspInfo.session = xop::MediaSession::CreateNew(stream); // url: rtsp://ip/live
	stRtspInfo.session->AddSource(xop::channel_0, xop::H264Source::CreateNew()); 
	stRtspInfo.session->AddSource(xop::channel_1, xop::AACSource::CreateNew(44100,2));

	stRtspInfo.session->AddNotifyConnectedCallback([] (xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port){
		printf("RTSP client connect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
        stRtspInfo.client++;
	});
   
	stRtspInfo.session->AddNotifyDisconnectedCallback([](xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port) {
		printf("RTSP client disconnect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
        stRtspInfo.client--;
	});

    std::cout << "URL: " << rtsp_url << std::endl;
    stRtspInfo.session_id = stRtspInfo.server->AddSession(stRtspInfo.session); 

    return 0;
}


int RTSP_Start()
{
	stRtspInfo.bRunning = 1;
    stRtspInfo.pthread = new std::thread(SendFrameThread, stRtspInfo.server.get(), stRtspInfo.session_id, std::ref(stRtspInfo.client));
    stRtspInfo.pthread->detach();
    return 0;
}

int RTSP_Stop()
{
	stRtspInfo.bRunning = 0;
	return 0;
}

void SendFrameThread(xop::RtspServer* rtsp_server, xop::MediaSessionId session_id, int& clients)
{
    int ret;
    RtspGetFrame *pGetFrame = stRtspInfo.pGetFrame;
    void *pbuf;
	while(stRtspInfo.bRunning)
	{
		if(clients > 0) /* 会话有客户端在线, 发送音视频数据 */
		{
			{
				//获取一帧 H264, 打包
				xop::AVFrame videoFrame = {0};
                ret = (*pGetFrame)(&pbuf, &videoFrame.type, &videoFrame.size);
                if(ret != 0)
                {
                    printf("*pGetFrame fail\n");
                    continue;
                }
                videoFrame.buffer.reset(new uint8_t[videoFrame.size]);
                memcpy(videoFrame.buffer.get(), pbuf, videoFrame.size);
				videoFrame.timestamp = xop::H264Source::GetTimestamp(); // 时间戳, 建议使用编码器提供的时间戳
				rtsp_server->PushFrame(session_id, xop::channel_0, videoFrame); //送到服务器进行转发, 接口线程安全
				
			}
                    
			{				
				/*
				//获取一帧 AAC, 打包
				xop::AVFrame audioFrame = {0};
				audioFrame.type = xop::AUDIO_FRAME;
				audioFrame.size = audio frame size;  /* 音频帧大小 
				audioFrame.timestamp = xop::AACSource::GetTimestamp(44100); // 时间戳
				audioFrame.buffer.reset(new uint8_t[audioFrame.size]);                    
				memcpy(audioFrame.buffer.get(), audio frame data, audioFrame.size);

				rtsp_server->PushFrame(session_id, xop::channel_1, audioFrame); // 送到服务器进行转发, 接口线程安全
				*/
			}		
		}

		xop::Timer::Sleep(1); /* 实际使用需要根据帧率计算延时! */
	}
}






