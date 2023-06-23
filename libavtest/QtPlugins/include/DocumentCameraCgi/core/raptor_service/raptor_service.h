#ifndef raptor_service_h
#define raptor_service_h

#include <QLibrary>
#include "raptor_service/message_def.h"
#include "raptor_service/raptor_defs.h"
#include "raptor_service/picojson.h"

class raptor_service
{
private:
    static QLibrary m_libCurl;      //!< libcurl.dll
    bool isExportLog;
    int token;
    std::string url;
    std::string ip;

protected:
    void writeLog(std::string log);
    void writePicoObjectLog(picojson::object obj);

    picojson::object post_request(std::string url, std::string body, long lTimeout);
    picojson::object sendRequest(int token, int msg_id, std::string type, std::string param);
    picojson::object send_request_rpc(std::string body, long lTimeout);

public:
    static bool loadLib();

    raptor_service();
    raptor_service(std::string _ip);
    virtual ~raptor_service();

    void setIp(std::string _ip);
    void enableExportLog(bool enable);

    std::string getCurrentIP();

    StartSession_Rsp startSession();
    CommonResponse stopSession();

    ClCommonRPC_Rsp request_command(std::string body, long lTimeout);
};

#endif
