//--------------------------------------------------------------------------------//
/*!
	@file	ClStreamDecoder.cpp
	@brief	ストリームデコーダ
	@author	大橋　ysuda
*/
//--------------------------------------------------------------------------------//
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

#include "ClStreamDecoder.h"
//#include "ClPlayAudioThread.h"

#ifdef USE_MAINCONCEPT
#ifdef Q_OS_MAC
#include "mac/sample_common_misc.h"
#else
#include "sample_common_misc.h"	//
#endif
#endif

//デバックファイルにqDebugの情報を書き込む場合はコメントを外す
//#define USE_DEBUGFILE

#define AV_TIMETEST	0
#if AV_TIMETEST
#include "QtdxutTimer/qtdxuttimer.h"
QtDXUTTimer g_timer;
#endif

#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif

#include "CommonFunction.h"
extern "C"{
#include <libavutil/opt.h>
#include <libavutil/log.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <avconv.h>
}

#define QUEUEDIMAGE_MAX	(1)
#define QUEUEDFRAME_MAX	(10)
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
((unsigned long)(unsigned char)(ch0) | ((unsigned long)(unsigned char)(ch1) << 8) | \
((unsigned long)(unsigned char)(ch2) << 16) | ((unsigned long)(unsigned char)(ch3) << 24 ))

static QFile debugFile;
static QTextStream logStream;

static void appMessageHandler(QtMsgType type,
	const QMessageLogContext&, const QString& msg)
{
	Q_UNUSED(type)
	if(debugFile.isOpen()){
//		logStream << QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") << msg << endl;
	}
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
	@brief		コンストラクタ / デストラクタ
	@param[in]	pParent : 親
*/
//--------------------------------------------------------------------------------//
ClDecodeFrame::ClDecodeFrame(QObject *pParent) : QThread(pParent)
{
	m_bIsStop = false;
	m_dPts = NULL;
	m_dDurationAvg = NULL;
	m_pContextVideo = NULL;

}
ClDecodeFrame::~ClDecodeFrame()
{
	m_bIsStop = true;
	quit();
	wait(1000);

	//フレームの開放
	while(!m_listPacketData.isEmpty()){
		av_free_packet(m_listPacketData.dequeue());
	}
}

//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief		初期化
	@retval		成否
*/
//--------------------------------------------------------------------------------//
bool ClDecodeFrame::init(AVCodecContext *pContextVideo, qreal dPts, qreal dDurationAvg)
{
	if(false == isRunning()){
		qDebug("start()");

		//ビデオ再生用コンテキスト
		m_pContextVideo = pContextVideo;

		m_dDurationAvg = dDurationAvg;
		m_dPts = dPts;

		//優先順位の変更
//		setPriority(QThread::HighPriority);
#ifdef USE_MAINCONCEPT
		decode_init();	// MainConcept初期化
#endif
		start();
	}else{
		return false;
	}
	return true;
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief		データ追加
	@param[in]	pFrame : デコード済みフレーム
*/
//--------------------------------------------------------------------------------//
void ClDecodeFrame::addPacket(AVPacket *pPacket)
{
#ifdef Q_OS_WIN
	AVPacket *pClone = av_packet_alloc();
	int iCpRet = av_packet_copy_props(pClone, pPacket);
	if(0 > iCpRet){
		return;
	}
	pClone->data = (uint8_t*)malloc(pPacket->size + AV_INPUT_BUFFER_PADDING_SIZE);
	pClone->size = pPacket->size;
	memcpy(pClone->data, pPacket->data, pPacket->size);
	memset(pClone->data + pPacket->size, 0, AV_INPUT_BUFFER_PADDING_SIZE);

	mutex.lock();
	m_listPacketData.enqueue(pClone);
	mutex.unlock();
#else
    AVPacket clone = {};
    av_init_packet(&clone);
    clone.data = (uint8_t*)malloc(pPacket->size + AV_INPUT_BUFFER_PADDING_SIZE);
    clone.size = pPacket->size;
    clone.flags = pPacket->flags;
    memcpy(clone.data, pPacket->data, pPacket->size);
    memset(clone.data + pPacket->size, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    mutex.lock();
    m_listPacketData.enqueue(&clone);
    mutex.unlock();
#endif
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief		メイン処理
*/
//--------------------------------------------------------------------------------//
void ClDecodeFrame::run()
{
	AVPacket *pAvPacket = NULL;
	volatile bool bNeedFirst = true;										//頭出し要求

	while(false == m_bIsStop)
	{
		if(false == m_listPacketData.isEmpty())
		{
			mutex.lock();
			pAvPacket = m_listPacketData.dequeue();
			mutex.unlock();

			if(NULL == pAvPacket){
				continue;
			}

//			qDebug("m_listPacketData.count %d",m_listPacketData.count());

			//フレームの頭出し
			if(true == bNeedFirst){
				if(!(AV_PKT_FLAG_KEY & pAvPacket->flags)){
					av_free_packet(pAvPacket);	//パケット必要なくなったので解放
					continue;
				}else{
					bNeedFirst = false;
				}
			}
			//バッファが溜まってきたらデコードをやめる
			if(30 < m_listPacketData.count()){
				bNeedFirst = true;
			}

#ifndef USE_MAINCONCEPT
			//映像フレーム
			AVFrame *pFrameDec = av_frame_alloc();
			int iGot = 0;
			int iPktSize = pAvPacket->size;
			uint8_t *pcPktData = pAvPacket->data;
#if AV_TIMETEST
			double dbeginloop = g_timer.getAbsoluteTime();
#endif
#if defined(Q_OS_MAC)
            while(iPktSize > 0 && pFrameDec != NULL){
#else
			while(iPktSize > 0){
#endif
				int resDecVod = avcodec_decode_video2(m_pContextVideo, pFrameDec, &iGot, pAvPacket);	//デコード
//				qDebug("resDecVod : %d, flags %08x pktsize %d", resDecVod, (quint32)pFrameDec->flags, pAvPacket->size);
				if(resDecVod < 0){
					break;
				}
				iPktSize -= resDecVod;
				pcPktData += resDecVod;
#if AV_TIMETEST
				double dendloop = g_timer.getAbsoluteTime();
				qDebug("decode time:%f", (dendloop - dbeginloop) * 1000);
#endif
#if AV_TIMETEST
				qDebug("keyframe=%d pktdts=%lld, pts=%lld dts=%lld size=%d flag=%d duration=%lld",
					   pFrameDec->key_frame ,pFrameDec->pkt_dts,pAvPacket->pts,pAvPacket->dts,pAvPacket->size,pAvPacket->flags,pAvPacket->duration);
#endif
				if(iGot != 0)
				{
					//解像度変更対応
					if(m_nowResolution.isEmpty()){
						m_nowResolution = QSize(pFrameDec->width, pFrameDec->height);
					}else if(m_nowResolution != QSize(pFrameDec->width, pFrameDec->height)){
						//解像度の変更を検出
						emit sigChangeResolution(QSize(pFrameDec->width, pFrameDec->height));
						//キーフレームまで頭出し
						m_nowResolution = QSize(pFrameDec->width, pFrameDec->height);
						av_frame_free(&pFrameDec);
						continue;
					}
					AVFrame *pFrameCon = av_frame_alloc();
					pFrameCon->width = pFrameDec->width;
					pFrameCon->height = pFrameDec->height;
					pFrameCon->pkt_dts = pFrameDec->pkt_dts;
					pFrameCon->pts = pFrameDec->pts;
					pFrameCon->pict_type = pFrameDec->pict_type;
					pFrameCon->key_frame = pFrameDec->key_frame;
					int64_t *pllDrt = new int64_t;
					*pllDrt = m_dPts;
					pFrameCon->opaque = pllDrt;
					if(convertPixelFormat(pFrameDec, pFrameCon, AV_PIX_FMT_RGB24))	//フォーマット変換
					{
						ClAvImage lastImg(pFrameCon, m_dDurationAvg);
//						qDebug("PTS : %lld", pFrameCon->pkt_dts);
						emit sigImage(lastImg);

						av_free(pFrameCon->data[0]);	//フォーマット変換時の生成メモリ解放
						av_frame_free(&pFrameCon);
					}else{
						//フォーマット変換に失敗した
						delete pFrameCon->opaque;
						av_frame_free(&pFrameCon);
					}
				}
				av_frame_free(&pFrameDec);
				free(pAvPacket->data);
				av_free_packet(pAvPacket);	//パケット必要なくなったので解放
			}
#else	// MainConceptCodec
			//デコード処理
			if(decode_frame(pAvPacket) < 0){	// デコードデータ有
				//デコードエラーなので次のIフレまで飛ばす
				bNeedFirst = true;
			}
#endif	//USE_MAINCONCEPT

		}else{
			usleep(1000);
		}
	}
#ifdef USE_MAINCONCEPT
	decode_done();
#endif	//USE_MAINCONCEPT

}

#ifdef USE_MAINCONCEPT
//--------------------------------------------------------------------------------//
/*!
	@brief		MainConceptCodec
*/
//--------------------------------------------------------------------------------//
void ClDecodeFrame::decode_init(void)
{
	videobs = open_h264in_Video_stream_ex(get_rc, 0, 0);
	if(!videobs){
//		qDebug() << "Failed to open h264 decode stream";
		return;
	}
	int32_t err;
	err = videobs->auxinfo(videobs, 0, PARSE_INIT, NULL, 0);
	err = videobs->auxinfo(videobs, SMP_BY_PICTURES, SET_SMP_MODE, NULL, 0);
	err = videobs->auxinfo(videobs, 1, SET_LOOP_FILTER_MODE, NULL, 0);
	err = videobs->auxinfo(videobs, 4, SET_CPU_NUM, NULL, 0);
	err = videobs->auxinfo(videobs, LOW_LATENCY_FLAG, PARSE_OPTIONS, 0, 0);
	err = videobs->auxinfo(videobs, 0, PARSE_SEQ_HDR, NULL, 0);
	err = videobs->auxinfo(videobs, 0, INIT_FRAME_PARSER, NULL, 0);
	err = videobs->auxinfo(videobs, SKIP_NONE, PARSE_FRAMES, NULL, 0);
}
//--------------------------------------------------------------------------------//
qint32 ClDecodeFrame::decode_frame(AVPacket *packet)
{
	uint8_t *data = packet->data;
	qint32 len = packet->size;
	qint32 iRet = 0;
	do {
		uint32_t consumed = videobs->copybytes(videobs, data, len);
		data += consumed;
		len -= consumed;
		uint32_t state = videobs->auxinfo(videobs, 0, CLEAN_PARSE_STATE, NULL, 0); // get state and check

		if(state & (PIC_DECODED_FLAG | PIC_VALID_FLAG | PIC_FULL_FLAG)){
			struct SEQ_Params *pSEQ;      // pointer to sequence header info
			if(BS_OK == videobs->auxinfo(videobs, 0, GET_SEQ_PARAMSP, &pSEQ, sizeof(pSEQ))) {
				uint32_t dim_x = pSEQ->horizontal_size;
				uint32_t dim_y = pSEQ->vertical_size;
				int32_t fourcc = MAKEFOURCC('R','G','B','3');
				int32_t img_start = 0, stride = 0;
				int32_t pic_size =  get_video_frame_size(dim_x, dim_y, fourcc, &stride, &img_start);
				uint8_t *pbuffer_cache = (uint8_t *)malloc(pic_size + 16);
				uint8_t *pbuffer = (uint8_t* )(((uint64_t)pbuffer_cache + 15) & (~15));

				if( pbuffer_cache ) {
					frame_tt frame = {0};
					init_frame(&frame, fourcc, dim_x, dim_y, pbuffer, img_start != 0, stride);
					if( BS_OK == videobs->auxinfo(videobs, 0, GET_PIC, &frame, sizeof(frame_tt)) )
					{
						//解像度変更対応
						if(m_nowResolution.isEmpty()){
							m_nowResolution = QSize(dim_x, dim_y);
						}else if(m_nowResolution != QSize(dim_x, dim_y)){
							//解像度の変更を検出
							emit sigChangeResolution(QSize(dim_x, dim_y));
							//キーフレームまで頭出し
							m_nowResolution = QSize(dim_x, dim_y);
							iRet = -1;
							break;
						}
						QImage img(pbuffer, dim_x, dim_y, QImage::Format_RGB888);
                        QImage qImage = QImage();
                        if(img.isNull() == false)
                        {
                            qImage = img.mirrored();
                            ClAvImage clImg(qImage, m_dDurationAvg);
                            clImg.setDts(m_dPts);
                            clImg.setPts(m_dPts);
                            emit sigImage(clImg);
                        }
					}
					free(pbuffer_cache);
				}
			}
		}else if(state & PIC_ERROR_FLAG){
			iRet = -1;
		}
	} while(len);

	return iRet;
}
//--------------------------------------------------------------------------------//
void ClDecodeFrame::decode_done(void)
{
	videobs->copybytes(videobs, NULL, 0);
	videobs->done(videobs, 0);
	videobs->free(videobs);
}
//--------------------------------------------------------------------------------//
#endif	//USE_MAINCONCEPT

//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
	@param[in]	pParent : 親
*/
//--------------------------------------------------------------------------------//
ClStreamDecoder::ClStreamDecoder(QObject *pParent) : QThread(pParent)
{
	m_bIsStop = false;
	m_pThreadDecode = NULL;
	m_pThreadAudio = NULL;
	m_bUsesTcp = false;

	debugFile.setFileName(qApp->applicationDirPath() + QDir::separator() + "debug.log");
	if(true == debugFile.open(QIODevice::WriteOnly)){
		logStream.setDevice(&debugFile);
	}
#ifdef USE_DEBUGFILE
	qInstallMessageHandler(appMessageHandler);
#endif
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
	if(NULL != m_pThreadDecode)
	{
		delete m_pThreadDecode;
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
/*!
	@brief		RTSP over HTTP
*/
//--------------------------------------------------------------------------------//
void ClStreamDecoder::setUsesTcp(bool bUsesTcp)
{
	m_bUsesTcp = bUsesTcp;
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

	if(NULL != m_pThreadDecode)
	{
		delete m_pThreadDecode;	//変換停止
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

	setPriority(QThread::HighPriority);

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

	delete m_pThreadDecode;
	m_pThreadDecode = NULL;

	delete m_pThreadAudio;
	m_pThreadAudio = NULL;
}
//--------------------------------------------------------------------------------//

#ifdef USE_HWACCEL_DXVA
static AVPixelFormat getHwFormat(AVCodecContext *pCodecCtx, const AVPixelFormat *)
{
	InputStream* pIst = (InputStream*)pCodecCtx->opaque;
	pIst->active_hwaccel_id = HWACCEL_DXVA2;
	pIst->hwaccel_pix_fmt = AV_PIX_FMT_DXVA2_VLD;
	return pIst->hwaccel_pix_fmt;
}
#endif
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
	int iVideoStreamIndex = -1, iAudioStreamIndex = -1;
	int iVideoSkipFrame = 0, iAudioSkipFrame = 0;
	volatile bool bNeedFirst = true;										//頭出し要求
	int64_t llLastPts = 0;
	AVPacket packet = {};

	//各ストリーム
	AVCodecContext *pContextVideo = NULL, *pContextAudio = NULL;
	AVCodec *pCodecVideo = NULL, *pCodecAudio = NULL;

	AVFormatContext *pContext = NULL;

	//TCPストリーム要求
	AVDictionary *opts = NULL;
	av_dict_set(&opts, "buffer_size", "3000000", 0);
	if (m_bUsesTcp) {
		av_dict_set(&opts, "rtsp_transport", "tcp", 0);
		av_dict_set(&opts, "sdp_flags", "rtcp_to_source", 0);
	}

	//ストリーム要求開始
	int e = 0;
	if(0 != (e = avformat_open_input(&pContext, m_strStream.toLatin1().data(), NULL, &opts)))
	{
		qDebug("init 0x%x", -e);
		emit sigError();
		return;
	}
	//オプションの開放
	av_dict_free(&opts);

	//通信コールバック設定
	AVIOInterruptCB callback = {decodeCallback, (void*)&m_bIsStop};
	pContext->interrupt_callback = callback;

	//ストリーム解析
	if((e = avformat_find_stream_info(pContext, NULL)) < 0)
	{
		qDebug("find stream 0x%x", -e);
		emit sigError();
//#ifdef Q_OS_WIN
#if 0
        goto END;
#else
        if(NULL != pContextVideo)
        {
            avcodec_close(pContextVideo);
        }
        if(NULL != pContextAudio)
        {
            avcodec_close(pContextAudio);
        }
        avformat_close_input(&pContext);
        return;
#endif
	}
	//Dump information
	av_dump_format(pContext, 0, m_strStream.toLatin1().data(), 0);

	//映像ストリーム初期化
	for(uint i = 0; i < pContext->nb_streams; i++)
	{
		if(iVideoStreamIndex == -1 &&
				pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			AVDictionary *codec_opts = NULL;

			pContextVideo = pContext->streams[i]->codec;
			pCodecVideo = avcodec_find_decoder(pContextVideo->codec_id);
			av_dict_set(&codec_opts, "idct", "simplemmx", 0);
			av_dict_set(&codec_opts, "flags2", "fast", 0);
			av_dict_set(&codec_opts, "skip_frame", "default", 0);
#ifdef USE_HWACCEL_DXVA
			pContextVideo->flags |= CODEC_CAP_DR1;
			pContextVideo->flags |= CODEC_CAP_HWACCEL;

			InputStream *pIst = new InputStream();
			pIst->hwaccel_id = HWACCEL_AUTO;
			pIst->hwaccel_device = (char *)"dxva2";
			pIst->dec = pCodecVideo;
			pIst->dec_ctx = pContextVideo;
			pContextVideo->coded_width = 1920;
			pContextVideo->coded_height = 1080;
			pContextVideo->opaque = pIst;
			dxva2_init(pContextVideo);
			pContextVideo->opaque = pIst;
			pContextVideo->get_buffer2 = pIst->hwaccel_get_buffer;
			pContextVideo->get_format = getHwFormat;
			pContextVideo->thread_safe_callbacks = 1;
#endif
			int iRetOpen2 = avcodec_open2(pContextVideo, pCodecVideo, &codec_opts);
			if(iRetOpen2 == 0)
			{
				iVideoStreamIndex = i;
				iVideoSkipFrame = pContext->streams[iVideoStreamIndex]->codec_info_nb_frames;

			}else{
				qDebug("ERROR avcodec_open2 : 0x%08x", iRetOpen2);
			}
			av_dict_free(&codec_opts);
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
//				m_pThreadAudio = new ClPlayAudioThread();
//				if(NULL != m_pThreadAudio)
//				{
//					m_pThreadAudio->init(pContextAudio);
//				}
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
		emit sigError();
//#ifdef Q_OS_WIN
#if 0
		goto END;
#else
        if(NULL != pContextVideo)
        {
            avcodec_close(pContextVideo);
        }
        if(NULL != pContextAudio)
        {
            avcodec_close(pContextAudio);
        }
        avformat_close_input(&pContext);
        return;
#endif
	}


	//平均画像表示時間設定(平均フレームレートから取得)
	qreal dDurationAvg = 0, dTimeBase = 0;
	dDurationAvg = (qreal)pContext->streams[iVideoStreamIndex]->avg_frame_rate.den / (qreal)pContext->streams[iVideoStreamIndex]->avg_frame_rate.num * 1000;
	//表示時間の基準値
	dTimeBase = (qreal)pContext->streams[iVideoStreamIndex]->time_base.num / (qreal)pContext->streams[iVideoStreamIndex]->time_base.den * 1000;
	qreal dDifferentialPts = dDurationAvg / dTimeBase;
	qDebug("avg_frame_rate %d %d dTimeBase %f", pContext->streams[iVideoStreamIndex]->avg_frame_rate.den, pContext->streams[iVideoStreamIndex]->avg_frame_rate.num, dTimeBase);

	//デコード用スレッド生成
	m_pThreadDecode = new ClDecodeFrame();
	if(NULL != m_pThreadDecode)
	{
		m_pThreadDecode->init(pContextVideo,dDifferentialPts,dDurationAvg);
		connect(m_pThreadDecode,SIGNAL(sigImage(ClAvImage)),this,SIGNAL(sigImage(ClAvImage)));
		connect(m_pThreadDecode,SIGNAL(sigChangeResolution(QSize)),this,SIGNAL(sigChangeResolution(QSize)));
	}

#if AV_TIMETEST
	double dlooptime1 = g_timer.getAbsoluteTime();
	double dlooptime2 = 0;
#endif

	av_init_packet(&packet);				//パケット初期化

	//デコード処理
	while(false == m_bIsStop)
	{
#if AV_TIMETEST
		double dlooptime;
		dlooptime2 = g_timer.getAbsoluteTime();
		dlooptime = (dlooptime2 - dlooptime1) * 1000;
		dlooptime1 = dlooptime2;
#endif
		int resRdFrm = av_read_frame(pContext, &packet);
		if(resRdFrm == AVERROR(EAGAIN) && packet.buf == NULL){
			qDebug("got EAGAIN");
			continue;
		}
		if(resRdFrm < 0){
			qDebug("Failed to av_read_frame");
			emit sigError();
			av_free_packet(&packet);	//パケット必要なくなったので解放
//			goto END;
		}
		if(3000 > packet.duration){
			qDebug("Failed to duration : %lld", packet.duration);
			emit sigError();
			av_free_packet(&packet);	//パケット必要なくなったので解放
//			goto END;
		}

		//映像フレーム
		if(NULL != m_pThreadDecode && packet.stream_index == iVideoStreamIndex)
		{
			if(iVideoSkipFrame > 0)	//初期化中のフレームを飛ばす
			{
				iVideoSkipFrame--;
				continue;
			}

			//フレームの頭出し
			if(true == bNeedFirst){
				if(!(AV_PKT_FLAG_KEY & packet.flags)){
					av_free_packet(&packet);	//パケット必要なくなったので解放
					continue;
				}else{
					bNeedFirst = false;
				}
			}
			//Corrupt Frame対策
			if(AV_PKT_FLAG_CORRUPT & packet.flags){
				//破損？
				//キーフレームまで頭出し
				qDebug("Packet is corrupt");
				bNeedFirst = true;
				av_free_packet(&packet);	//パケット必要なくなったので解放
				continue;
			}
			//パケットロス対策
			if(!(AV_PKT_FLAG_KEY & packet.flags)){
				//パケットロス判定
				if((llLastPts + packet.duration) < packet.pts){
					//キーフレームまで頭出し
					bNeedFirst = true;
					av_free_packet(&packet);	//パケット必要なくなったので解放
					continue;
				}
			}
			//PTSを覚えておく
			llLastPts = packet.pts;

			//デコードスレッドに登録
			m_pThreadDecode->addPacket(&packet);
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
			avcodec_decode_audio4(pContextAudio, pFrameDec, &iGot, &packet);	//デコード
			if(iGot != 0)
			{
//				m_pThreadAudio->addAudio(pFrameDec);
			}
			av_frame_free(&pFrameDec);
		}
		av_free_packet(&packet);	//パケット必要なくなったので解放
	}

//END:
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
