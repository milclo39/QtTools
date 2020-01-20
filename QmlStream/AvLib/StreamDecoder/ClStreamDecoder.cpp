//--------------------------------------------------------------------------------//
/*!
	@file	ClStreamDecoder.cpp
	@brief	ストリームデコーダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#include "ClStreamDecoder.h"
#include "ClPlayAudioThread.h"

#define AV_TIMETEST	0
#if AV_TIMETEST
#include <QTime>
#endif

#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif

#include "CommonFunction.h"
extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

//--------------------------------------------------------------------------------//
/*!
	@brief		デコードコールバック
	@param[in]	pParam : 停止フラグ
	@retval		停止する場合1
*/
//--------------------------------------------------------------------------------//
static int decodeCallback(void *pParam)
{
	bool *pbIsStop = (bool*)pParam;
	return *pbIsStop;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
	@param[in]	pParent : 親
*/
//--------------------------------------------------------------------------------//
ClStreamDecoder::ClStreamDecoder(QObject *pclParent) : QThread(pclParent)
{
	m_bIsStop = false;
	m_pThreadAudio = NULL;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デストラクタ
*/
//--------------------------------------------------------------------------------//
ClStreamDecoder::~ClStreamDecoder()
{
	if(true == isRunning())
	{
		stop();
		wait(1000);
	}
	if(NULL != m_pThreadAudio)
	{
		delete m_pThreadAudio;
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		初期化(mainで実行)
*/
//--------------------------------------------------------------------------------//
void ClStreamDecoder::avStreamInit()
{
	av_log_set_level(AV_LOG_ERROR);
	av_register_all();
	avformat_network_init();
	qRegisterMetaType<ClAvImage>("ClAvImage");
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		終了(mainで実行)
*/
//--------------------------------------------------------------------------------//
void ClStreamDecoder::avStreamDeinit()
{
	avformat_network_deinit();
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		ストリーム設定
	@param[in]	strStream : 要求ストリーム
*/
//--------------------------------------------------------------------------------//
void ClStreamDecoder::setStream(QString strStream)
{
	if(true == isRunning() &&
			strStream == m_strStream)
	{
		return;
	}

	if(NULL != m_pThreadAudio)
	{
		delete m_pThreadAudio;	//音声再生停止
	}
	if(true == isRunning())
	{
		stop();	//ストリーム停止待ち
		wait();
	}

	m_bIsStop = false;
	m_strStream = strStream;
	start();
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		停止
*/
//--------------------------------------------------------------------------------//
void ClStreamDecoder::stop()
{
	m_bIsStop = true;
	delete m_pThreadAudio;
	m_pThreadAudio = NULL;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		メイン処理
*/
//--------------------------------------------------------------------------------//
void ClStreamDecoder::run()
{
	if(true == m_strStream.isEmpty())
	{
		return;
	}

	//ストリーム設定値
	qreal dDurationAvg = 0, dTimeBase = 0;
	int iVideoStreamIndex = -1, iAudioStreamIndex = -1;
	int iVideoSkipFrame = 0, iAudioSkipFrame = 0;
#if AV_TIMETEST
	int iTimeVideoAvg = 0, iVideoFrameCount = 0, iTimeVideoMax = 0;
	int iTimeAudioAvg = 0, iAudioFrameCount = 0, iTimeAudioMax = 0;
#endif

	//各ストリーム
	AVCodecContext *pContextVideo = NULL, *pContextAudio = NULL;
	AVCodec *pCodecVideo = NULL, *pCodecAudio = NULL;

	AVFormatContext *pContext = NULL;

	//ストリーム要求開始
	int e = 0;
	if(0 != (e = avformat_open_input(&pContext, m_strStream.toLatin1().data(), NULL, NULL)))
	{
		qDebug("init 0x%x", -e);
		return;
	}

	//通信コールバック設定
	AVIOInterruptCB callback = {decodeCallback, (void*)&m_bIsStop};
	pContext->interrupt_callback = callback;

	//ストリーム解析
	if((e = avformat_find_stream_info(pContext, NULL)) < 0)
	{
		qDebug("find stream 0x%x", -e);
		goto END;
	}

	//映像ストリーム初期化
	for(uint i = 0; i < pContext->nb_streams; i++)
	{
		if(iVideoStreamIndex == -1 &&
				pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			pContextVideo = pContext->streams[i]->codec;
			pCodecVideo = avcodec_find_decoder(pContextVideo->codec_id);
			if(avcodec_open2(pContextVideo, pCodecVideo, NULL) == 0)
			{
				iVideoStreamIndex = i;
				iVideoSkipFrame = pContext->streams[iVideoStreamIndex]->codec_info_nb_frames;
			}
		}
		else if(iAudioStreamIndex == -1 &&
				pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			pContextAudio = pContext->streams[i]->codec;
			pCodecAudio = avcodec_find_decoder(pContextAudio->codec_id);
			if(avcodec_open2(pContextAudio, pCodecAudio, NULL) == 0)
			{
				iAudioStreamIndex = i;
				iAudioSkipFrame = pContext->streams[iAudioStreamIndex]->codec_info_nb_frames;
				m_pThreadAudio = new ClPlayAudioThread();
				if(NULL != m_pThreadAudio)
				{
					m_pThreadAudio->init(pContextAudio);
				}
			}
		}
		if(iVideoStreamIndex != -1 &&
				iAudioStreamIndex != -1)
		{
			break;
		}
	}

	if(-1 == iVideoStreamIndex)
	{
		qDebug("no video stream");
		goto END;
	}

	AVPacket packet;
	av_init_packet(&packet);

	//平均画像表示時間設定(平均フレームレートから取得)
	dDurationAvg = (qreal)pContext->streams[iVideoStreamIndex]->avg_frame_rate.den / (qreal)pContext->streams[iVideoStreamIndex]->avg_frame_rate.num * 1000;
	//表示時間の基準値
	dTimeBase = (qreal)pContext->streams[iVideoStreamIndex]->time_base.num / (qreal)pContext->streams[iVideoStreamIndex]->time_base.den * 1000;

	//デコード処理
	while(false == m_bIsStop)
	{
		if(av_read_frame(pContext, &packet) < 0)
		{
			continue;
		}
		//映像フレーム
		if(packet.stream_index == iVideoStreamIndex)
		{
			if(iVideoSkipFrame > 0)	//初期化中のフレームを飛ばす
			{
				iVideoSkipFrame--;
				continue;
			}
			AVFrame *pFrameDec = av_frame_alloc();
			int iGot = 0;
#if AV_TIMETEST
			QTime time1 = QTime::currentTime();
#endif
			avcodec_decode_video2(pContextVideo, pFrameDec, &iGot, &packet);	//デコード
#if AV_TIMETEST
			QTime time2 = QTime::currentTime();
			int iTimeDec = time1.msecsTo(time2);
#endif
			if(iGot != 0)
			{
				AVFrame *pFrameCon = av_frame_alloc();
				pFrameCon->width = pFrameDec->width;
				pFrameCon->height = pFrameDec->height;
				if(convertPixelFormat(pFrameDec, &pFrameCon, AV_PIX_FMT_RGB24))	//フォーマット変換
				{
#if AV_TIMETEST
					QTime time3 = QTime::currentTime();
					int iTimeConvert = time2.msecsTo(time3);
#endif
					qreal dDuration = packet.duration * dTimeBase;	//フレームの表示時間
					if(dDuration == 0)
					{
						dDuration = dDurationAvg;	//表示時間が入っていなかったので平均を設定
					}
					ClAvImage img(pFrameCon, dDuration);
					emit sigImage(img);
#if AV_TIMETEST
					QTime time4 = QTime::currentTime();
					int iTimeTotal = time1.msecsTo(time4);
					if(iTimeVideoMax < iTimeTotal)
					{
						iTimeVideoMax = iTimeTotal;
					}
					iTimeVideoAvg = ((iTimeVideoAvg * iVideoFrameCount) + iTimeTotal) / (iVideoFrameCount + 1);
					iVideoFrameCount++;
					qDebug("time video avg:%d max:%d total:%d decode:%d convert:%d qt:%d", iTimeVideoAvg, iTimeVideoMax, iTimeTotal, iTimeDec, iTimeConvert, time3.msecsTo(time4));
#endif
					av_free(pFrameCon->data[0]);	//フォーマット変換時の生成メモリ解放
				}
#if AV_TIMETEST
				else
				{
					qDebug("time video decode failed %d", iTimeDec);
				}
#endif
				av_frame_free(&pFrameCon);
			}
			av_frame_free(&pFrameDec);
		}
		//音声フレーム
		else if(NULL != m_pThreadAudio &&
				packet.stream_index == iAudioStreamIndex)
		{
			if(iAudioSkipFrame > 0)	//初期化中のフレームを飛ばす
			{
				iAudioSkipFrame--;
				continue;
			}
			AVFrame *pFrameDec = av_frame_alloc();
			int iGot = 0;
#if AV_TIMETEST
			QTime time1 = QTime::currentTime();
#endif
			avcodec_decode_audio4(pContextAudio, pFrameDec, &iGot, &packet);	//デコード
#if AV_TIMETEST
			QTime time2 = QTime::currentTime();
			int iTimeDecode = time1.msecsTo(time2);
#endif
			if(iGot != 0)
			{
				m_pThreadAudio->addAudio(pFrameDec);
#if AV_TIMETEST
				QTime time3 = QTime::currentTime();
				int iTimeTotal = time1.msecsTo(time3);
				if(iTimeAudioMax < iTimeTotal)
				{
					iTimeAudioMax = iTimeTotal;
				}
				iTimeAudioAvg = ((iTimeAudioAvg * iAudioFrameCount) + iTimeTotal) / (iAudioFrameCount + 1);
				iAudioFrameCount++;
				qDebug("time audio avg:%d max:%d total:%d decode:%d play:%d", iTimeAudioAvg, iTimeAudioMax, iTimeTotal, iTimeDecode, time2.msecsTo(time3));
#endif
			}
#if AV_TIMETEST
			else
			{
				qDebug("time audio decode failed %d", iTimeDecode);
			}
#endif
			av_frame_free(&pFrameDec);
		}
		av_free_packet(&packet);	//パケット必要なくなったので解放
	}

END:
	//解放処理
	if(NULL != pContextVideo)
	{
		avcodec_close(pContextVideo);
	}
	if(NULL != pContextAudio)
	{
		avcodec_close(pContextAudio);
	}
	avformat_close_input(&pContext);
}
//--------------------------------------------------------------------------------//
