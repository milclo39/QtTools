//--------------------------------------------------------------------------------//
/*!
    @file	DocumentCameraCgiDefines.h
    @brief  Document Camera CGI definitions
    @author Le Thanh Trung
*/
//--------------------------------------------------------------------------------//
#ifndef DOCUMENTCAMERACGIDEFINES
#define DOCUMENTCAMERACGIDEFINES


// request key name
#define DC_REQ_KEY_ID                   "id"
#define DC_REQ_KEY_JSON_RPC             "jsonrpc"
#define DC_REQ_KEY_METHOD               "method"
#define DC_REQ_KEY_PARAMS               "params"

// response key name
#define DC_RES_KEY_ID                   "id"
#define DC_RES_KEY_RESULT               "result"
#define DC_RES_KEY_ERROR                "error"

// id number
#define DC_ID_NUMBER                    1

// json-rpc version
#define DC_JSON_RPC_VERSION             "2.0"

// method type name
#define DC_METHOD_FRAMERATE             "FRAMERATE"
#define DC_METHOD_VERSION               "VERSION"
#define DC_METHOD_RTCP_CODEC            "RTCPCODEC"
#define DC_METHOD_LIGHT                 "LIGHT"
#define DC_METHOD_POWER                 "POWER"
#define DC_METHOD_USB_MODE              "USBMODE"
#define DC_METHOD_MICRO_SCOPE           "MICROSCOPE"
#define DC_METHOD_IMAGE_MODE            "IMAGEMODE"
#define DC_METHOD_AF_MODE               "AFMODE"
#define DC_METHOD_WHITE_BALANCE         "WHITEBALANCE"
#define DC_METHOD_FLICKERLESS           "FLICKERLESS"
#define DC_METHOD_DIGITAL_ZOOM          "DIGITALZOOM"
#define DC_METHOD_REM_ZOOM_POS          "REMZOOMPOS"
#define DC_METHOD_QUALITY               "QUALITY"
#define DC_METHOD_NETWORK               "NETWORK"
#define DC_METHOD_NETWORK_AP            "NETWORK_AP"
#define DC_METHOD_NETWORK_ST            "NETWORK_ST"
#define DC_METHOD_RESET                 "RESET"
#define DC_METHOD_ZOOM                  "ZOOM"
#define DC_METHOD_BRIGHTNESS            "BRIGHTNESS"
#define DC_METHOD_FOCUS                 "FOCUS"
#define DC_METHOD_ROTATE                "ROTATE"
#define DC_METHOD_FREEZE                "FREEZE"
#define DC_METHOD_RESOLUTION            "RESOLUTION"
#define DC_METHOD_CUSTOM                "CUSTOM"

// param type key
#define DC_PARAM_KEY_GET                "GET"
#define DC_PARAM_KEY_SET                "SET"
#define DC_PARAM_KEY_STATE              "STATE"
#define DC_PARAM_KEY_ACTION             "ACTION"
#define DC_PARAM_KEY_MODE               "MODE"
#define DC_PARAM_KEY_HOST               "HOST"
#define DC_PARAM_KEY_ESSID              "ESSID"
#define DC_PARAM_KEY_PSKKEY             "PSKKEY"
#define DC_PARAM_KEY_AUTH               "AUTH"
#define DC_PARAM_KEY_STATION            "STATION"
#define DC_PARAM_KEY_TCPIP              "TCPIP"
#define DC_PARAM_KEY_DHCP               "DHCP"
#define DC_PARAM_KEY_IP                 "IP"
#define DC_PARAM_KEY_MASK               "MASK"
#define DC_PARAM_KEY_GATEWAY            "GATEWAY"
#define DC_PARAM_KEY_TARGET             "TARGET"
#define DC_PARAM_KEY_AP                 "AP"
#define DC_PARAM_KEY_DEGREE             "DEGREE"
#define DC_PARAM_KEY_FPS                "FPS"
#define DC_PARAM_KEY_QUALITY            "QUALITY"
#define DC_PARAM_KEY_AUDIO_DUMMY        "AUDIO_DUMMY"
#define DC_PARAM_KEY_PRIMARYDNS         "PRIMARYDNS"
#define DC_PARAM_KEY_SECONDARYDNS       "SECONDARYDNS"
#define DC_PARAM_KEY_CH                 "CH"
#define DC_PARAM_KEY_TYPE               "TYPE"

// param type value
#define DC_PARAM_VALUE_LIST             "LIST"
#define DC_PARAM_VALUE_CURRENT          "CURRENT"
#define DC_PARAM_VALUE_ON               "ON"
#define DC_PARAM_VALUE_OFF              "OFF"
#define DC_PARAM_VALUE_RESTART          "RESTART"
#define DC_PARAM_VALUE_UVC              "UVC"
#define DC_PARAM_VALUE_EOU              "EOU"
#define DC_PARAM_VALUE_BASIC            "BASIC"
#define DC_PARAM_VALUE_NETWORK          "NETWORK"
#define DC_PARAM_VALUE_ALL              "ALL"
#define DC_PARAM_VALUE_ESSID            "Essid"
#define DC_PARAM_VALUE_PSKKEY           "PskKey"
#define DC_PARAM_VALUE_WEP              "WEP"
#define DC_PARAM_VALUE_WPA_WPA2_PSK     "WPA/WPA2-PSK"
#define DC_PARAM_VALUE_AP               "AP"
#define DC_PARAM_VALUE_STATION          "STATION"
#define DC_PARAM_VALUE_STOP             "STOP"
#define DC_PARAM_VALUE_TELE             "TELE"
#define DC_PARAM_VALUE_WIDE             "WIDE"
#define DC_PARAM_VALUE_DEFAULT          "DEFAULT"
#define DC_PARAM_VALUE_MAX              "MAX"
#define DC_PARAM_VALUE_MIN              "MIN"
#define DC_PARAM_VALUE_STEP             "STEP"
#define DC_PARAM_VALUE_MAGNIFICATION    "MAGNIFICATION"
#define DC_PARAM_VALUE_SPEED            "SPEED"
#define DC_PARAM_VALUE_OPEN             "OPEN"
#define DC_PARAM_VALUE_CLOSE            "CLOSE"
#define DC_PARAM_VALUE_NEAR             "NEAR"
#define DC_PARAM_VALUE_FAR              "FAR"
#define DC_PARAM_VALUE_AF               "AF"
#define DC_PARAM_VALUE_TRUE             "true"
#define DC_PARAM_VALUE_FALSE            "false"
#define DC_PARAM_VALUE_RTSP             "RTSP"

// result type key
#define DC_RESULT_KEY_LIST              "LIST"
#define DC_RESULT_KEY_CURRENT           "CURRENT"
#define DC_RESULT_KEY_MICON             "MICON"
#define DC_RESULT_KEY_MAIN_FPGA         "MAINFPGA"
#define DC_RESULT_KEY_HEAD_FPGA         "HEADFPGA"
#define DC_RESULT_KEY_STATE             "STATE"
#define DC_RESULT_KEY_HOST              "HOST"
#define DC_RESULT_KEY_ESSID             "ESSID"
#define DC_RESULT_KEY_PSKKEY            "PSKKEY"
#define DC_RESULT_KEY_AUTH              "AUTH"
#define DC_RESULT_KEY_TCPIP             "TCPIP"
#define DC_RESULT_KEY_DHCP              "DHCP"
#define DC_RESULT_KEY_IP                "IP"
#define DC_RESULT_KEY_MASK              "MASK"
#define DC_RESULT_KEY_GATEWAY           "GATEWAY"
#define DC_RESULT_KEY_ESSID             "ESSID"
#define DC_RESULT_KEY_PSKKEY            "PSKKEY"
#define DC_RESULT_KEY_MAX               "MAX"
#define DC_RESULT_KEY_MIN               "MIN"
#define DC_RESULT_KEY_DEFAULT           "DEFAULT"
#define DC_RESULT_KEY_STEP              "STEP"
#define DC_RESULT_KEY_MAGNIFICATION     "MAGNIFICATION"
#define DC_RESULT_KEY_SPEED             "SPEED"
#define DC_RESULT_KEY_KERNEL            "KERNEL"
#define DC_RESULT_KEY_BUILDTIME         "BUILDTIME"
#define DC_RESULT_KEY_PRIMARYDNS        "PRIMARYDNS"
#define DC_RESULT_KEY_SECONDARYDNS      "SECONDARYDNS"

// error type key
#define DC_ERROR_KEY_CODE               "CODE"
#define DC_ERROR_KEY_MESSAGE            "MESSAGE"

// command type id
#define DC_ID_CMD_NG                    -1
#define DC_ID_CMD_GET_FRAMERATE         0
#define DC_ID_CMD_SET_FRAMERATE         1
#define DC_ID_CMD_VERSION               2
#define DC_ID_CMD_GET_RTCP_CODEC        3
#define DC_ID_CMD_SET_RTCP_CODEC        4
#define DC_ID_CMD_STATE_LIGHT           5
#define DC_ID_CMD_LIGHT                 6
#define DC_ID_CMD_POWER                 7
#define DC_ID_CMD_USB_MODE              8
#define DC_ID_CMD_MICRO_SCOPE           9
#define DC_ID_CMD_GET_IMAGE_MODE        10
#define DC_ID_CMD_SET_IMAGE_MODE        11
#define DC_ID_CMD_GET_AF_MODE           12
#define DC_ID_CMD_SET_AF_MODE           13
#define DC_ID_CMD_STATE_WHITE_BALANCE   14
#define DC_ID_CMD_WHITE_BALANCE         15
#define DC_ID_CMD_GET_FLICKERLESS       16
#define DC_ID_CMD_SET_FLICKERLESS       17
#define DC_ID_CMD_STATE_DIGITAL_ZOOM    18
#define DC_ID_CMD_DIGITAL_ZOOM          19
#define DC_ID_CMD_STATE_REM_ZOOM_POS    20
#define DC_ID_CMD_REM_ZOOM_POS          21
#define DC_ID_CMD_GET_QUALITY           22
#define DC_ID_CMD_SET_QUALITY           23
#define DC_ID_CMD_NETWORK               24
#define DC_ID_CMD_NETWORK_AP_EMPTY      25
#define DC_ID_CMD_NETWORK_AP            26
#define DC_ID_CMD_NETWORK_ST_EMPTY      27
#define DC_ID_CMD_NETWORK_ST            28
#define DC_ID_CMD_RESET                 29
#define DC_ID_CMD_ACTION_ZOOM           30
#define DC_ID_CMD_GET_ZOOM              31
#define DC_ID_CMD_SET_ZOOM              32
#define DC_ID_CMD_ACTION_BRIGHTNESS     33
#define DC_ID_CMD_GET_BRIGHTNESS        34
#define DC_ID_CMD_SET_BRIGHTNESS        35
#define DC_ID_CMD_ACTION_FOCUS          36
#define DC_ID_CMD_GET_FOCUS             37
#define DC_ID_CMD_DEGREE_ROTATE         38
#define DC_ID_CMD_ROTATE                39
#define DC_ID_CMD_STATE_FREEZE          40
#define DC_ID_CMD_FREEZE                41
#define DC_ID_CMD_GET_RESOLUTION        42
#define DC_ID_CMD_SET_RESOLUTION        43
#define DC_ID_CMD_CUSTOM                44

// error type id
#define DC_ID_ERROR_FAILED              -1
#define DC_ID_ERROR_PARSE_JSON_RESULT   -2
#define DC_ID_ERROR_PARSE_JSON_ERROR    -3
#define DC_ID_ERROR_RESULT_INVALID      -4

// constant value
#define DC_VAL_REQUEST_TIMEOUT          10
#define DC_VAL_RESPONSE_OK              0
#define DC_VAL_EXPECTED_GET             2
#define DC_VAL_EXPECTED_VERSION         5
#define DC_VAL_EXPECTED_NETWORK_AP      4
#define DC_VAL_EXPECTED_COMMON          5
#define DC_VAL_EXPECTED_NETWORK_ST      8
#define DC_VAL_EXPECTED_ZOOM            7

// log type id
#define DC_ID_LOG_JSON_REQ              0
#define DC_ID_LOG_JSON_RECV             1
#define DC_ID_LOG_ERROR                 2

// structure of Network Ap (Req/Res)
typedef struct _tag_network_ap
{
    QString         host;           // host name
    QString         essid;          // essid
    QString         pskkey;         // pskkey
    QString         auth;           // authenticate
    qint32          ch;             // channel

    _tag_network_ap()
    {
        host = QString();
        essid = QString();
        pskkey = QString();
        auth = QString();
        ch = -1;
    }

} TAG_NETWORK_AP;

// structure of Network St (Req/Res)
typedef struct _tag_network_st
{
    QString         tcpip;          // tcp/ip
    bool            dhcp;           // dhcp client
    QString         ip;             // ip address
    QString         mask;           // sub-net mask address
    QString         gateway;        // default gateway address
    QString         essid;          // essid
    QString         pskkey;         // pskkey
    QString         primarydns;     // primarydns
    QString         secondarydns;   // secondarydns

    _tag_network_st()
    {
        tcpip = QString();
        dhcp = false;
        ip = QString();
        mask = QString();
        gateway = QString();
        essid = QString();
        pskkey = QString();
        primarydns = QString();
        secondarydns = QString();
    }

} TAG_NETWORK_ST;

// structure of Zoom (Res)
typedef struct _tag_zoom
{
    qint32              current;        // current
    qint32              max;            // max
    qint32              min;            // min
    qint32              def;            // default
    qint32              step;           // step
    double              magnification;  // magnification
    qint32              speed;          // speed

    _tag_zoom()
    {
        current = -1;
        max = -1;
        min = -1;
        def = -1;
        step = -1;
        magnification = -1.f;
        speed = -1;
    }

} TAG_ZOOM;

// structure of Brightness & Focus (Res)
typedef struct _tag_brightness_focus
{
    qint32              current;        // current
    qint32              max;            // max
    qint32              min;            // min
    qint32              def;            // default
    qint32              step;           // step

    _tag_brightness_focus()
    {
        current = -1;
        max = -1;
        min = -1;
        def = -1;
        step = -1;
    }
} TAG_BRIGHTNESS_FOCUS;

// structure of Custom (Req)
typedef struct _tag_custom
{
    QString             fps;            // fps
    QString             quality;        // quality
    QString             audio_dummy;    // audio dummy

    _tag_custom()
    {
        fps = QString();
        quality = QString();
        audio_dummy = QString();
    }

} TAG_CUSTOM;

// structure of result
typedef struct _tag_res_get
{
    QString         list;           // list of info
    QString         current;        // current info

    _tag_res_get()
    {
        list = QString();
        current = QString();
    }

} TAG_RES_RESULT;

// structure of Version result
typedef struct _tag_res_version
{
    QString         micon;          // micon version info
    QString         mainfpga;       // main fpga version info
    QString         headfpga;       // head fpga version info
    QString         kernel;         // kernel version info
    QString         buildtime;      // buildtime version info

    _tag_res_version()
    {
        micon = QString();
        mainfpga = QString();
        headfpga = QString();
        kernel = QString();
        buildtime = QString();
    }

} TAG_RES_VERSION;

// structure of Resolution (Req)
typedef struct _tag_resolution
{
    QString             set;            // set
    QString             type;           // type

    _tag_resolution()
    {
        set = QString();
        type = QString();
    }

} TAG_RESOLUTION;

// structure of error code
typedef struct _tag_res_error
{
    qint32          code;           // error code
    QString         message;        // message info

    _tag_res_error()
    {
        code = -1;
        message = QString();
    }

} TAG_RES_ERROR;

// structure of Request Command object
typedef struct _tag_req_cmd
{
    qint32          cmd;                // request command id
    qint32          id;                 // id parameter
    QString         method;             // method parameter
    QString         paramStr;           // string parameter
    bool            paramState;         // state parameter
    qint32          paramNum;           // number parameter
    TAG_NETWORK_AP* pParamAp;           // network ap parameter
    TAG_NETWORK_ST* pParamSt;           // network st parameter
    TAG_CUSTOM*     pParamCustom;       // custom parameter
    TAG_RESOLUTION* pParamResolution;   // resolution parameter

    _tag_req_cmd()
    {
        cmd = DC_ID_CMD_NG;
        id = -1;
        method = QString();
        paramStr = QString();
        paramState = false;
        paramNum = -1;
        pParamAp = NULL;
        pParamSt = NULL;
        pParamCustom = NULL;
        pParamResolution = NULL;
    }

} TAG_REQ_CMD;

// structure of Response Command object
typedef struct _tag_res_cmd
{
    qint32                  cmd;            // response command id
    qint32                  id;             // id response
    TAG_RES_ERROR*          pErr;           // error response
    TAG_RES_RESULT*         pGet;           // get result response
    TAG_RES_VERSION*        pVer;           // version result response
    bool                    state;          // state response
    qint32                  number;         // number response
    TAG_NETWORK_AP*         pResAp;         // network ap response
    TAG_NETWORK_ST*         pResSt;         // network st response
    TAG_ZOOM*               pResZoom;       // zoom response
    TAG_BRIGHTNESS_FOCUS*   pResCommon;     // brightness or focus response

    _tag_res_cmd()
    {
        cmd = DC_ID_CMD_NG;
        id = -1;
        pErr = NULL;
        pGet = NULL;
        pVer = NULL;
        state = false;
        number = -1;
        pResAp = NULL;
        pResSt = NULL;
        pResZoom = NULL;
        pResCommon = NULL;
    }

} TAG_RES_CMD;

#endif // DOCUMENTCAMERACGIDEFINES

