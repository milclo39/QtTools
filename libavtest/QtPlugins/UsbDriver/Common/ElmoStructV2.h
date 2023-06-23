#ifndef ELMOSTRUCTV2_H
#define ELMOSTRUCTV2_H

/* struct for a packet header */
typedef struct _ELMO_PACKET_HEADER_V2
{
	unsigned char			HeaderVer;		/* Header Version */
	unsigned char			TransID;		/* transaction ID */
	unsigned short			OpCode;			/* operation code */
	unsigned long			DataSize;		/* the size of data that dose not include this header */
} ELMO_PACKET_HEADER_V2, *PELMO_PACKET_HEADER_V2; /* 12 bytes */

#define CAST_HEADER_V2(x)	((PELMO_PACKET_HEADER_V2)(x))

/* struct for a packet */
typedef struct _ELMO_PACKET_V2
{
	ELMO_PACKET_HEADER_V2	Header;			/* a packet header */
	unsigned long			Data[1];		/* a place holder for data
											 * ,a struct for a command
											 * or a struct for a response
											 */
} ELMO_PACKET_V2, *PELMO_PACKET_V2;

/* struct for a command */
typedef struct _ELMO_COMMAND_V2
{
	unsigned long	Param0;					/* parameter 0 */
	unsigned long	Param1;					/* parameter 1 */
	unsigned long	Param2;					/* parameter 2 */
	unsigned long	Param3;					/* parameter 3 */
	unsigned long	Param4;					/* parameter 4 */
	unsigned long	Param5;					/* parameter 5 */
} ELMO_COMMAND_V2, *PELMO_COMMAND_V2; /* 24 bytes */

#define CAST_COMMAND_V2(x)	((PELMO_COMMAND_V2)((PELMO_PACKET_V2)(x))->Data)

/* struct for a continuous command */
typedef struct _ELMO_CONTINUOUS_V2
{
	unsigned long	SessionId;				/* Session ID */
	unsigned long	TotalDtSize;			/* Total data size */
	unsigned long	Param2;					/* parameter 2 */
	unsigned long	Param3;					/* parameter 3 */
	unsigned long	Param4;					/* parameter 4 */
	unsigned long	Param5;					/* parameter 5 */
} ELMO_CONTINUOUS_V2, *PELMO_CONTINUOUS_V2; /* 24 bytes */

#define CAST_CONTINUOUS_V2(x)	((PELMO_CONTINUOUS_V2)((PELMO_PACKET_V2)(x))->Data)

/* struct for a file transfer command */
typedef struct _ELMO_FILETRANS_V2
{
    unsigned long	SessionId;				/* Session ID */
    unsigned long	Param1;                 /* parameter 1 */
    unsigned long	Param2;					/* parameter 2 */
    unsigned long	Param3;					/* parameter 3 */
    unsigned long	Param4;					/* parameter 4 */
    unsigned long	Param5;					/* parameter 5 */
    unsigned long	FileData[1];			/* File Data */
} ELMO_FILETRANS_V2, *PELMO_FILETRANS_V2;

#define CAST_FILETRANS_V2(x)	((PELMO_FILETRANS_V2)((PELMO_PACKET_V2)(x))->Data)

/* struct for a fw update command */
typedef struct _ELMO_FWUPDATE_V2
{
	unsigned long	SessionId;				/* Session ID */
	unsigned short	TotalSectorSize;		/* Total sector size */
	unsigned short	WroteSectorSize;		/* Wrote sector size */
	unsigned long	Status;					/* Status */
	unsigned long	Param3;					/* parameter 3 */
	unsigned long	Param4;					/* parameter 4 */
	unsigned long	Param5;					/* parameter 5 */
} ELMO_FWUPDATE_V2, *PELMO_FWUPDATE_V2;		/* 24 bytes */

#define CAST_FWUPDATE_V2(x)	((PELMO_FWUPDATE_V2)((PELMO_PACKET_V2)(x))->Data)

/* union for a function bit */
typedef union _ELMO_FUNC_BIT
{
	unsigned char BYTE;		/* Header Version */
	struct{
		unsigned char _RESERVED:2;			/* Reserved */
		unsigned char _FINISH:1;			/* Session finish flag */
		unsigned char _SESSION:1;			/* Session flag */
		unsigned char _HVER:4;				/* Header Version */
	}BIT;
}ELMO_FUNC_BIT, *PELMO_FUNC_BIT;

#define CAST_HEADER_FLAG(x)	(((PELMO_FUNC_BIT)&(CAST_HEADER_V2(x)->HeaderVer))->BIT)

/* struct for a continuous command */
typedef struct _ELMO_ERROR_V2
{
	unsigned long	ErrorDetail;			/* Error Detail */
	unsigned long	Param1;					/* parameter 1 */
	unsigned long	Param2;					/* parameter 2 */
	unsigned long	Param3;					/* parameter 3 */
	unsigned long	Param4;					/* parameter 4 */
	unsigned long	Param5;					/* parameter 5 */
} ELMO_ERROR_V2, *PELMO_ERROR_V2; 			/* 24 bytes */

#define CAST_ERROR_V2(x)	((CAST_ERROR_V2)((PELMO_PACKET_V2)(x))->Data)


#define ELMO_MAXPACKETSIZE_V2	(1024 * 256) /* maximum packet size */
#define ELMO_CMD_PACKET_SIZE_V2 (sizeof(ELMO_PACKET_HEADER_V2) + sizeof(ELMO_COMMAND_V2))

/* Header Version */
#define ELMO_HEAD_VER_V2	(1)
#define GET_HEAD_VER(hcv)	((unsigned char)((hcv >> 4) & 0x0f))


typedef struct _FILE_TIME
{
	unsigned long ulLowDateTime;
	unsigned long ulHighDateTime;
} FILE_TIME, *PFILE_TIME;

typedef struct _FILE_INFO
{
	unsigned long ulFileAttributes;
	FILE_TIME stCreationTime;
	FILE_TIME stLastAccessTime;
	FILE_TIME stLastWriteTime;
	unsigned long ulFileSizeHigh;
	unsigned long ulFileSizeLow;
	unsigned char *pucFilename;
} FILE_INFO, *PFILE_INFO;

#define ELMO_CMD_SESSION_SIZE	(8)
#define ELMO_FILEUP_DUMMY		(24)

//for Stream
#define ELMO_STREAM_INFO			(24)
#define ELMO_STREAM_OTHER_IMG		(16)
#define ELMO_STREAM_TYPE_JPG		(0)
#define ELMO_STREAM_TYPE_H264		(1)
#define ELMO_STREAM_TYPE_SOUND_PCM	(2)
#define ELMO_STREAM_TYPE_SOUND_AAC	(3)


#endif // ELMOSTRUCTV2_H
