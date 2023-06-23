#ifndef CLPRESENTERWEBCAM_H
#define CLPRESENTERWEBCAM_H
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "linux/ClDeviceInfo.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>

#include <linux/videodev2.h>

//--------------------------------------------------------------------------------------

class ClPresenterWebcam : public ClDeviceInfo
{
public:
	ClPresenterWebcam(qint32 lProductId = PRODUCT_NONE, QObject *parent = 0);
	virtual ~ClPresenterWebcam();

	virtual qint32 open();									//open for a this device
	virtual void close();									//close for a this device

	virtual qint32 CommandPresenter(qint32 CmdID, QVariant& qParameter);

protected:
	ClPresenterWebcam(){}

	virtual qint64	readData ( char * data, qint64 maxSize );
	virtual qint64	writeData ( const char * data, qint64 maxSize );

	virtual qint32 getPicture(QVariant &qParam);			//get picture from presenter
	virtual qint32 execCommand(ClUsbCmd &clCmd);			//execute some command with device
	virtual qint32 getModelName(QVariant &qParam);			//get model name from device and output to QVariant
	virtual qint32 getVersion(QVariant &qParam);			//get version from device and output to QVariant

	qint32 xioctl(qint32 ifd, qint32 iRequest, void *pvArg);

#if 0
	bool querycap(v4l2_capability &cap);
	bool queryctrl(v4l2_queryctrl &qc);
	bool querymenu(v4l2_querymenu &qm);
	bool g_tuner(v4l2_tuner &tuner);
	bool g_input(int &input);
	bool s_input(int input);
	bool g_output(int &output);
	bool s_output(int output);
	bool g_audio(v4l2_audio &audio);
	bool s_audio(int input);
	bool g_audout(v4l2_audioout &audout);
	bool s_audout(int output);
	bool s_std(v4l2_std_id std);
	bool g_std(v4l2_std_id &std);
	bool g_frequency(v4l2_frequency &freq);
	bool s_frequency(v4l2_frequency &freq);
	bool s_frequency(int freq);
	bool g_fmt_cap(v4l2_format &fmt);
	bool g_fmt_out(v4l2_format &fmt);
	bool try_fmt(v4l2_format &fmt);
	bool s_fmt(v4l2_format &fmt);
	bool enum_input(v4l2_input &in, bool init = false);
	bool enum_output(v4l2_output &out, bool init = false);
	bool enum_audio(v4l2_audio &audio, bool init = false);
	bool enum_audout(v4l2_audioout &audout, bool init = false);
	bool enum_std(v4l2_standard &std, bool init = false, int index = 0);
	bool enum_fmt_cap(v4l2_fmtdesc &std, bool init = false, int index = 0);
	bool enum_fmt_out(v4l2_fmtdesc &std, bool init = false, int index = 0);
	bool enum_framesizes(v4l2_frmsizeenum &frm, __u32 init_pixfmt = 0, int index = 0);
	bool enum_frameintervals(v4l2_frmivalenum &frm, __u32 init_pixfmt = 0, __u32 w = 0, __u32 h = 0, int index = 0);

	bool reqbufs_mmap_cap(v4l2_requestbuffers &reqbuf, int count = 0);
	bool reqbufs_user_cap(v4l2_requestbuffers &reqbuf, int count = 0);
	bool dqbuf_mmap_cap(v4l2_buffer &buf);
	bool dqbuf_user_cap(v4l2_buffer &buf);
	bool qbuf(v4l2_buffer &buf);
	bool qbuf_mmap_cap(int index);
	bool qbuf_user_cap(int index, void *ptr, int length);
	bool streamon_cap();
	bool streamoff_cap();

	bool reqbufs_mmap_out(v4l2_requestbuffers &reqbuf, int count = 0);
	bool reqbufs_user_out(v4l2_requestbuffers &reqbuf);
	bool dqbuf_mmap_out(v4l2_buffer &buf);
	bool dqbuf_user_out(v4l2_buffer &buf);
	bool qbuf_mmap_out(int index, int bytesused);
	bool qbuf_user_out(void *ptr, int length);
	bool streamon_out();
	bool streamoff_out();
#endif

#define V4L2_DEVICE	"/dev/video0"

	v4l2_capability m_Capability;

	/* Number of retries finding a video source before failing */
	#define NUM_IOCTL_RETRIES        100

	/* Triple buffering for the capture driver */
	#define NUM_CAPTURE_BUFS         3

	/* Describes a capture frame buffer */
	typedef struct tagCAPTUREBUFFER {
		void		*pStart;
		quint32		lOffset;
		size_t		lLength;
	} stCAPTUREBUFFER;

	stCAPTUREBUFFER				*m_pstCapBuf;
	quint32						m_lNumCapBufs;
	qint32						m_lFd;

};
//--------------------------------------------------------------------------------------

#endif // CLPRESENTERWEBCAM_H
