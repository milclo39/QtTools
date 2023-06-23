//--------------------------------------------------------------------------------//
/*!
    @file	ClDocumentCameraCgiWorker.h
    @brief  Document Camera CGI Worker thread class
    @author Le Thanh Trung
*/
//--------------------------------------------------------------------------------//
#ifndef CLDOCUMENTCAMERACGIWORKER_H
#define CLDOCUMENTCAMERACGIWORKER_H


#include <QThread>
#include "DocumentCameraCgiDefines.h"


class raptor_service;


//--------------------------------------------------------------------------------//
/*!
    @brief
*/
//--------------------------------------------------------------------------------//
class ClDocumentCameraCgiWorker : public QThread
{
    Q_OBJECT

public:
    ClDocumentCameraCgiWorker(QString strAddr);
    virtual ~ClDocumentCameraCgiWorker();

    void setAddr(QString strAddr);
    QString getAddr();
    bool request(TAG_REQ_CMD* pReqCmd);

protected:
    void run();
    void procCmd(TAG_REQ_CMD* pReqCmd);

signals:
    void sigResponse(TAG_RES_CMD* pResCmd);

private:
    TAG_REQ_CMD* m_pReqCmd;
    raptor_service* m_pclRaptorService;
};

#endif // CLDOCUMENTCAMERACGIWORKER_H
