#ifndef __USB_DATA_TRANSFER_H__
#define __USB_DATA_TRANSFER_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined(Q_OS_MACX)
#include <unistd.h>
#else defined(Q_OS_WIN)
#include <WinSock2.h>
#include <Windows.h>
#include <io.h>
#endif
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

#include <QThread>
#include <QDebug>
/**
____________________________________________________________
|  Len(4)  | Magic(4) | Seq(2) | Type(2) |  Payload   |
------------------------------------------------------------
**/

static const uint32_t Magic = 0x7363616d; // SCAM
static const uint16_t CmdToCam   = 0x0000; // HostPC -> Camera
static const uint16_t CmdFromCam = 0x000f; // HostPC <- Camera
static const uint16_t CmdReset   = 0x0ff0; // Reset seqential number
static const uint16_t DataContinue = 0x8000; // Indicate data is continuous
static const std::string VolumeName("SafieVolume");

#define VolumeBase  (11264*512)
#define CmdOffset   (VolumeBase + 4096) // under 4k page empty area
#define MaxDataSize (1000*512)      // 1000 sectors (500kb)  // free space is 3072+8 ~ 4096 = 1016 sectors -> 508kb
#define HeaderSize  (12)
#define AccessInterval (500000)     // 500ms
#define WinAccessInterval	(1000)
#define WinBuffSize (512)

namespace spyder {

class UsbDataTransfer : public QThread{
	Q_OBJECT
public:
  typedef enum {
    kTransferTypeHost = 0,
    kTransferTypeDevice,
  } kTransferType;

  typedef enum {
    kSuccess = 0,
    kReset,
    kErrorInvalidParam,
    kErrorInvalidStatus,
    kErrorTimeout,
    kErrorCritical,
  } kTransferResult;

  UsbDataTransfer(){
    this->mFd = -1;
    this->mSendSeqN = -1;
    this->mRecvSeqN = -1;
#if defined(Q_OS_WIN)
	this->sPageSize = 4096;
#else defined(Q_OS_MACX)
    this->sPageSize = sysconf(_SC_PAGESIZE);
#endif
//    qDebug() << "PageSize " << this->sPageSize;
  }
  ~UsbDataTransfer(){
  }
  virtual kTransferResult Init(const char* cmdFile) = 0;

  // for camera
  virtual kTransferResult RecvData(uint8_t **, size_t *, uint8_t)
  {return kSuccess;}
  virtual kTransferResult SendReply(uint8_t *, size_t)
  {return kSuccess;}

  // for HostPC
  virtual kTransferResult SendData(uint8_t *, size_t , uint8_t **, size_t *)
  {return kSuccess;}
  virtual kTransferResult SendReset()
  {return kSuccess;}
signals:
	void sigChangeDevice(bool bIsConnect);

protected:
  static   long sPageSize;
  int      mFd;
  int16_t mSendSeqN;
  int16_t mRecvSeqN;
};

}
#endif // #ifndef __USB_DATA_TRANSFER_H__

