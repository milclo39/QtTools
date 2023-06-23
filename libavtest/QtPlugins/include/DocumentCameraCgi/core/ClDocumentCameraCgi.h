//--------------------------------------------------------------------------------//
/*!
    @file	ClDocumentCameraCgi.h
    @brief  Document Camera CGI commands class
    @author Le Thanh Trung
*/
//--------------------------------------------------------------------------------//
#ifndef CLDOCUMENTCAMERACGI_H
#define CLDOCUMENTCAMERACGI_H


#include <QWidget>
#include <QQueue>
#include <QMutex>
#include "DocumentCameraCgiDefines.h"


class ClDocumentCameraCgiWorker;


//--------------------------------------------------------------------------------//
/*!
    @brief
*/
//--------------------------------------------------------------------------------//
class ClDocumentCameraCgi : public QWidget
{
    Q_OBJECT

public:
    static bool loadLib();

public:
    ClDocumentCameraCgi(QString strAddr, QWidget* pclParent = NULL);
    virtual ~ClDocumentCameraCgi();

    void setAddr(QString strAddr);
    void cancelAllRequests();
    QString getAddr();
    qint32 getQueuedRequestCount();

private:
    void writeParamReqLog(TAG_REQ_CMD* pReqCmd);

public slots:
    void slotRequest(TAG_REQ_CMD* pReqCmd);

protected slots:
    void slotExecQueuedRequest();

signals:
    void sigResponse(TAG_RES_CMD* pResCmd);

private:
    ClDocumentCameraCgiWorker*  m_pclCgiWorker;
    QMutex                      m_clCmdQueueMutex;
    QQueue<TAG_REQ_CMD*>        m_clQueuedRequest;
};

#endif // CLDOCUMENTCAMERACGI_H
