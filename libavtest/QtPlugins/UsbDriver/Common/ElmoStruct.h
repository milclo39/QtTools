/* 
 * ElmoStruct.h
 *
 *  by Yutaka Hibino
 *
 * Copyright (c) 2004 ELMO CO., LTD.
 */

#ifndef __ELMOSTRUCT_H__
#define __ELMOSTRUCT_H__

#ifdef Q_OS_MAC
#define WORD unsigned short
//#define DWORD unsigned long //elmo del
#define DWORD unsigned int //elmo add
#endif

/* struct for a packet header */
typedef struct _ELMO_PACKET_HEADER
{
	WORD	PacketType;	/* packet type (command, response, data) */
	WORD	TransID;		/* transaction ID */
	DWORD	DataSize;		/* the size of data that dose not include this header */
} ELMO_PACKET_HEADER, *PELMO_PACKET_HEADER; /* 8 bytes */


/* struct for a packet */
typedef struct _ELMO_PACKET
{
	ELMO_PACKET_HEADER	Header;	/* a packet header */
	DWORD	Data[1];			/* a place holder for data
								 * ,a struct for a command
								 * or a struct for a response
								 */
} ELMO_PACKET, *PELMO_PACKET;


/* struct for a command */
typedef struct _ELMO_COMMAND
{
	WORD	OpCode;	/* operation code */
	WORD	Reserved;	/* reserved */
	DWORD	Param1;	/* parameter 1 */
	DWORD	Param2;	/* parameter 2 */
	DWORD	Param3;	/* parameter 3 */
	DWORD	Param4;	/* parameter 4 */
	DWORD	Param5;	/* parameter 5 */
} ELMO_COMMAND, *PELMO_COMMAND; /* 24 bytes */


/* struct for a response */
typedef struct _ELMO_RESPONSE
{
	WORD	ResponseCode;	/* response code */
	WORD	Reserved;		/* reserved */
	DWORD	Param1;			/* parameter 1 */
	DWORD	Param2;	/* parameter 2 */
	DWORD	Param3;	/* parameter 3 */
	DWORD	Param4;	/* parameter 4 */
	DWORD	Param5;	/* parameter 5 */
} ELMO_RESPONSE, *PELMO_RESPONSE; /* 24 bytes */


/* packet size */
#define ELMO_MAXPACKETSIZE	(4096 * 16) /* maximum packet size */

#define ELMO_CMD_PACKET_SIZE (sizeof(ELMO_PACKET) - sizeof(DWORD) + sizeof(ELMO_COMMAND))
#define ELMO_RES_PACKET_SIZE (sizeof(ELMO_PACKET) - sizeof(DWORD) + sizeof(ELMO_RESPONSE))

/* packet type */
#define ELMO_PKT_CMD	0		/* for command packet */
#define ELMO_PKT_RES	1		/* for response packet */
#define ELMO_PKT_DAT	2		/* for data packet */

/******************************************************************************
 * file access
 */

// ********************************************************
// for EL_GET_DIR_LIST
//
typedef struct _ELMO_FILE_DATA_HEADER
{
	DWORD	Status;
	DWORD	DataSize;
	DWORD	Attribute;
} ELMO_FILE_DATA_HEADER, *PELMO_FILE_DATA_HEADER;


typedef struct _ELMO_FILE_DATA
{
	ELMO_FILE_DATA_HEADER Header;
	DWORD	Data[1];
} ELMO_FILE_DATA, *PELMO_FILE_DATA;


#define ELMO_FULLPATH_MAXLEN	32

typedef struct _ELMO_PATH_INFO
{
	char	Path[ELMO_FULLPATH_MAXLEN];
	DWORD	Attribute;
} ELMO_PATH_INFO, *PELMO_PATH_INFO;


// ********************************************************
// for EL_GET_JPG_LIST_INDIR
//
typedef struct _ELMO_LIST_DATA_HEADER
{
	DWORD	Status;
	DWORD	ListSize;
} ELMO_LIST_DATA_HEADER, *PELMO_LIST_DATA_HEADER;


typedef struct _ELMO_LIST_DATA
{
	ELMO_LIST_DATA_HEADER Header;
	DWORD	Data[1];
} ELMO_LIST_DATA, *PELMO_LIST_DATA;


typedef struct _ELMO_PATHPARAM_INFO
{
	ELMO_PATH_INFO 	PathInfo;
	DWORD			Param1;
	DWORD			Param2;
} ELMO_PATHPARAM_INFO, *PELMO_PATHPARAM_INFO;

#endif
