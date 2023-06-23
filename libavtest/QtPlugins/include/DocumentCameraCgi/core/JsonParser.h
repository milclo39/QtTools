//--------------------------------------------------------------------------------//
/*!
    @file	JsonParser.h
    @brief  Support to parser Json format
    @author Le Thanh Trung
*/
//--------------------------------------------------------------------------------//
#ifndef CLJSONPARSER
#define CLJSONPARSER


#include "DocumentCameraCgiDefines.h"


//################################### INTERFACE ##################################//
QString buildEmptyParams();
QString buildGetParams(QString val);
QString buildSetParams(QString val);
QString buildSetParams(qint32 val);
QString buildStateParams(bool val);
QString buildActionParams(QString val);
QString buildModeParams(QString val);
QString buildTargetParams(QString val);
QString buildDegreeParams(QString val);
QString buildDegreeParams(qint32 val);
QString buildNetworkApParams(TAG_NETWORK_AP* pVal);
QString buildNetworkStParams(TAG_NETWORK_ST* pVal);
QString buildCustomParams(TAG_CUSTOM* pVal);
QString buildResolutionParams(TAG_RESOLUTION* pVal);
QString buildJsonData(qint32 id, QString method, QString params);

bool parseGetResults(TAG_REQ_CMD* pReqCmd, QJsonObject result, TAG_RES_RESULT* pData);
bool parseVersionResults(QJsonObject result, TAG_RES_VERSION* pData);
bool parseStateResults(QJsonObject result, bool &state);
bool parseNetworkApResults(QJsonObject result, TAG_NETWORK_AP* pData);
bool parseNetworkStResults(QJsonObject result, TAG_NETWORK_ST* pData);
bool parseZoomResults(TAG_REQ_CMD* pReqCmd, QJsonObject result, TAG_ZOOM* pData);
bool parseBrightnessResults(TAG_REQ_CMD* pReqCmd, QJsonObject result, TAG_BRIGHTNESS_FOCUS* pData);
bool parseFocusResults(TAG_REQ_CMD* pReqCmd, QJsonObject result, TAG_BRIGHTNESS_FOCUS* pData);
bool parseJsonError(QJsonObject result, TAG_RES_CMD* pResCmd);
bool parseJsonResult(TAG_REQ_CMD* pReqCmd, QJsonObject result, TAG_RES_CMD* pResCmd);


//################################### UTILITIES ##################################//
void writeDebugLog(int type, QString log, int cmd);
bool parseCommonResData(TAG_REQ_CMD* pReqCmd, QJsonObject result, TAG_BRIGHTNESS_FOCUS* pData);

#endif // CLJSONPARSER

