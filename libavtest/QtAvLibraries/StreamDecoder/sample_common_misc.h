/**
@file: sample_common_misc.h
@brief Auxinfo handling and other shared stuff

@verbatim
File: sample_common_misc.h
Desc: Auxinfo handling and other shared stuff
 
 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
@endverbatim
 **/
#ifndef SAMPLE_COMMON_MISC_H_INCLUDED
#define SAMPLE_COMMON_MISC_H_INCLUDED

#include "auxinfo.h"
#include "mcdefs.h"
#include "bufstrm.h"

/**
 * @name WAV header structure structure
 * @{
 **/
 
 /**
 *@brief WAV header parameters structure used to configure WAV header
 */
typedef struct wav_hdr_param
{
    uint16_t num_channels;               /**<@brief Number of chanels */
    uint32_t sample_rate;                /**<@brief Sample rate */
    uint32_t bytes_per_sec;              /**<@brief Bytes per second */
    uint16_t bits_per_sample;            /**<@brief Bits per sample */
    uint16_t block_align;                /**<@brief Block align */
    int32_t data_size;                   /**<@brief Size of data */
} wav_hdr_param_t;
/**@}*/
#ifdef __cplusplus 
extern "C" {
#endif
 
// Standard output functions.
void info_printf(const char * fmt, ...);
void warn_printf(const char * fmt, ...);
void error_printf(const char * fmt, ...);
void stat_printf(const char * fmt, ...);
void progress_printf(int32_t percent, const char * fmt, ...);

// resource functions dispatcher
void * MC_EXPORT_API get_rc(const char* name);

/**
 * @brief Returns size of video frame
 * @param[in] width                - Width of frame
 * @param[in] height               - Heigth of frame
 * @param[in] cs_fourcc            - Color space (FOURCC)
 * @param[in] line_size            - Line size (stride)
 * @param[in] img_start            - Pointer to start of image    
 * @return     Size of frame in bytes
 */
int32_t get_video_frame_size(int32_t width, int32_t height, uint32_t cs_fourcc, int32_t * line_size, int32_t * img_start);


/**
 * @brief Initializes frame structure
 * @param[in] frame                - Pointer to frame_tt structure
 * @param[in] four_cc              - Colorspace (Fourcc)
 * @param[in] width                - Width of frame
 * @param[in] height               - Height of frame
 * @param[in] p                    - Pointer to data (plane[0])    
 * @param[in] bottomup             - Flipping flag
 * @param[in] stride               - Stride of frame
 * @param[in] plane_height         - Height of frame by plane, pointer to data (int32_t[4])
 * @return     0 or 1
 */
int32_t init_frame(frame_tt *frame, uint32_t four_cc, int32_t width, int32_t height, uint8_t *p, int32_t bottomup, int32_t stride, int32_t *plane_height = NULL);

/**
* @brief AUXinfo handler type.
**/
typedef uint32_t (*auxinfo_handler_t)(bufstream_tt * bs, uint32_t offs, uint32_t info_id, void * info_ptr, uint32_t info_size);

/**
* @brief Extern original AUXinfo function.
**/
extern auxinfo_handler_t org_auxinfo;

/**
* @brief Extern picture info structure.
**/
extern struct pic_start_info pic_info;

/**
 * @brief Encoder auxinfo handler
 * @param[in] bs                  - Pointer to bufstream
 * @param[in] offs                - offsets
 * @param[in] info_id             - Information ID (TIME_STAMP_INFO,STATISTIC_INFO,VIDEO_AU_CODE,ID_PICTURE_START_CODE ...)
 * @param[in] info_ptr            - Pointer to information data
 * @param[in] info_size           - Size of information data
 * @return     result of original auxinfo function
 */
uint32_t auxinfo(bufstream_tt * bs, uint32_t offs, uint32_t info_id, void * info_ptr, uint32_t info_size);

/**
 * @brief Cathes keyboard hit
 * @param[in] text                - Pointer to bufstream
 * @return     1 if key has been pressed, 0 if not
 */
int has_pressed_key( const char* text );

/**
 * @brief Reads WAV header
 * @param[in] f                    - Pointer to file
 * @param[in|out] param            - Pointer to WAV header parameters structure
 * @return     0 if there are errors, else returns size of header
 */
int32_t wav_header_read(FILE *f, struct wav_hdr_param* param);

/**
 * @brief Reads WAV data
 * @param[in] f                    - Pointer to file
 * @param[in|out] param            - Pointer to WAV header parameters structure
 * @param[in|out] buf              - Pointer to buffer of data
 * @param[in] buf_size             - Size of buffer
 * @return     0 if size of data <=0,else returns length of readed data
 */
int32_t wav_data_read(FILE *f, struct wav_hdr_param* param, uint8_t *buf, uint32_t buf_size);

/**
 * @brief Writes WAV header
 * @param[in] f                    - Pointer to file
 * @param[in] param                - Pointer to WAV header parameters structure
 * @return     28 + fmt_size
 */
int32_t wav_header_write(FILE *f, struct wav_hdr_param* param);

/**
 * @brief Reads WAV data
 * @param[in] f                    - Pointer to file
 * @param[in|out] param            - Pointer to WAV header parameters structure
 * @param[in] buf                  - Pointer to buffer of data
 * @param[in] buf_size             - Size of buffer
 * @return     size of written data
 */
int32_t wav_data_write(FILE *f, struct wav_hdr_param* param, uint8_t *buf, uint32_t buf_size);

typedef struct _context_md5_t {
  uint32_t buf[4];
  uint32_t bits[2];
  union {
    uint8_t b8[64];
    uint32_t b32[16];
  } in;
} context_md5_t;

void MD5Init(context_md5_t *ctx);
void MD5Update(context_md5_t *ctx, uint8_t *buf, size_t len);
void MD5Final(uint8_t digest[16], context_md5_t *ctx, uint32_t invert);
#ifdef __cplusplus 
}
#endif

#endif //SAMPLE_COMMON_MISC_H_INCLUDED