#ifndef RAPTOR_DEFS_H
#define RAPTOR_DEFS_H

#define WIFI_CAM_IP                     "192.168.42.1"
#define PORT_NO                         8787

#define RESPONSE_OK    					0
#define RESPONSE_UNKNOWN_ERROR			-1
#define RESPONSE_TOKEN_UNLOCKED			-2
#define RESPONSE_SESSION_START_FAIL		-3
#define RESPONSE_INVALID_TOKEN			-4
#define RESPONSE_CAMERA_MANUAL_OVERRIDE	-5
#define RESPONSE_FOUND_NO_MATCH_CLNT	-6
#define RESPONSE_JSON_PACKAGE_ERROR		-7
#define RESPONSE_JSON_PACKAGE_TIMEOUT	-8
#define RESPONSE_JSON_SYNTAX_ERROR		-9
#define RESPONSE_MQ_SENDING_ERROR		-10
#define RESPONSE_MQ_RECEIVING_ERROR		-11
#define RESPONSE_CONFIG_NOT_EXIST 		-12
#define RESPONSE_INVALID_OPTION_VALUE	-13
#define RESPONSE_INVALID_OPERATION		-14
#define RESPONSE_OPERATION_MISMATCH		-15
#define RESPONSE_HDMI_INSERTED			-16
#define RESPONSE_NO_MORE_SPACE			-17
#define RESPONSE_CARD_PROTECTED			-18
#define RESPONSE_NO_MORE_MEMORY			-19
#define RESPONSE_PIV_NOT_ALLOWED		-20
#define RESPONSE_SYSTEM_BUSY			-21

#define MSGID_AMBA_START_SESSION    	257
#define MSGID_AMBA_STOP_SESSION    		258
#define MSGID_AMBA_TAKE_PHOTO      		769
#define MSGID_AMBA_AMBA_LS				1282
#define MSGID_AMBA_GET_FILE				1285

#define ERR_TIME_OUT                    -1000
#define ERR_CONNECTION                  -1001
#define ERR_JSON                        -1002
#define ERR_HTTP                        -1003
#define ERR_OUT_MEM                     -1004

#define KEY_RVAL		 				"rval"
#define KEY_MSG_ID 						"msg_id"
#define KEY_PARAM 						"param"
#define KEY_TYPE 						"type"
#define KEY_TOKEN 						"token"
#define KEY_JSON_RPC                    "json_rpc"

#define PARAM_START_TOKEN				0

#define STRING_EMPTY                    ""
#define STRING_HTTP_PROXY_PORT          "proxy.isb.co.jp:8080"

#endif

