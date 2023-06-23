//--------------------------------------------------------------------------------//
/*!
	@file	ClPlayAudioThread.cpp
	@brief	音声再生スレッド
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#include "ClPlayAudioThread.h"

#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavresample/avresample.h>
#include <libavutil/opt.h>
}
#include <QAudioFormat>
//#include <QAudioOutput>
#define AUDIO_SAMPEL_FROMAT AV_SAMPLE_FMT_S16
#define AUDIO_SAMPLE_BIT	16
#define AUDIO_SAMPLE_TYPE	QAudioFormat::SignedInt

//--------------------------------------------------------------------------------//
/*!
	@brief	コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClPlayAudioThread::ClPlayAudioThread() : QThread()
{
	m_bIsStop = false;
	m_pAudio = NULL;
	m_pResample = NULL;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	デストラクタ
*/
//--------------------------------------------------------------------------------//
ClPlayAudioThread::~ClPlayAudioThread()
{
	m_bIsStop = true;
	quit();
	wait();

	if(NULL != m_pAudio)
	{
		delete m_pAudio;
	}
	if(NULL != m_pResample)
	{
		if(1 == avresample_is_open(m_pResample))
		{
			avresample_close(m_pResample);
		}
		avresample_free(&m_pResample);
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		初期化
	@param[in]	pContext : 音声コンテキスト
	@retval		成否
*/
//--------------------------------------------------------------------------------//
bool ClPlayAudioThread::init(AVCodecContext *pContext)
{
	m_listAudioData.clear();	//以前の音声データクリア

	QAudioFormat format;
	format.setChannelCount(pContext->channels);
	format.setSampleRate(pContext->sample_rate);
//	format.setSampleSize(AUDIO_SAMPLE_BIT);
    format.setSampleFormat(QAudioFormat::Int32);
//	format.setCodec("audio/pcm");
	if(false == format.isValid())
	{
		return false;
	}

	//音声出力初期化
#if 0
	if(NULL != m_pAudio)
	{
        if(m_pAudio->format() != format)
		{
			delete m_pAudio;
			m_pAudio = NULL;
		}
	}
#endif
	if(NULL == m_pAudio)
	{
        m_pAudio = new QAudioOutput(format);
		m_pAudio->setBufferSize(format.sampleRate() * (format.sampleSize() / 8) * format.channelCount());	//1秒分バッファ
	}

	//音声変換初期化
	if(m_pResample == NULL)
	{
		m_pResample = avresample_alloc_context();
		if(NULL == m_pResample)
		{
			goto ERROR;
		}
	}
	else if(1 == avresample_is_open(m_pResample))
	{
		avresample_close(m_pResample);
	}
	av_opt_set_int(m_pResample, "in_channel_layout", pContext->channel_layout, 0);
	av_opt_set_int(m_pResample, "in_sample_fmt", pContext->sample_fmt, 0);
	av_opt_set_int(m_pResample, "in_sample_rate", pContext->sample_rate, 0);
	av_opt_set_int(m_pResample, "out_channel_layout", pContext->channel_layout, 0);
	av_opt_set_int(m_pResample, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	av_opt_set_int(m_pResample, "out_sample_rate", pContext->sample_rate, 0);

	if(0 != avresample_open(m_pResample))
	{
		goto ERROR;
	}

	return true;
ERROR:
	if(NULL != m_pAudio)
	{
		delete m_pAudio;
		m_pAudio = NULL;
	}
	if(NULL != m_pResample)
	{
		avresample_free(&m_pResample);
		m_pResample = NULL;
	}
	return false;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		データ追加
	@param[in]	pFrame : デコード済みフレーム
*/
//--------------------------------------------------------------------------------//
void ClPlayAudioThread::addAudio(AVFrame *pFrame)
{
	if(NULL != m_pResample)
	{
		//音声変換
		int iLineSize = 0;
        int iDestSize = av_samples_get_buffer_size(&iLineSize, 1,
												   pFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);
		QByteArray dataAudio(iDestSize, 0);
		char *pBuffer = dataAudio.data();
		if(0 == avresample_convert(m_pResample, reinterpret_cast<uint8_t**>(&pBuffer),
						   iLineSize, pFrame->nb_samples,
						   pFrame->data, pFrame->linesize[0], pFrame->nb_samples))
		{
			return;	//変換失敗
		}

		mutex.lock();
		m_listAudioData.enqueue(dataAudio);
		mutex.unlock();

		if(false == isRunning())
		{
			start();
		}
		else
		{
			quit();
		}
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		メイン処理
*/
//--------------------------------------------------------------------------------//
void ClPlayAudioThread::run()
{
	if(NULL == m_pAudio)
	{
		return;
	}
	if(QAudio::ActiveState == m_pAudio->state())
	{
		m_pAudio->stop();
	}

	QIODevice *pDevAudio = m_pAudio->start();
	while(false == m_bIsStop)
	{
		if(false == m_listAudioData.isEmpty())
		{
			mutex.lock();
			QByteArray dataAudio = m_listAudioData.dequeue();
			mutex.unlock();

			while(false == m_bIsStop &&
				  0 == pDevAudio->write(dataAudio))
			{
				pDevAudio->waitForBytesWritten(-1);
			}

			if(false == m_listAudioData.isEmpty())
			{
				continue;
			}
		}
		exec();
	}
}
//--------------------------------------------------------------------------------//
