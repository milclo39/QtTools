/********************************************************************
created:    2009/07/20 - 12:00
file name:  sample_common_misc.cpp
purpose:    Auxinfo handling and other shared stuff

 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#if !defined(__linux__) && !defined(__native_client__) && !defined(__APPLE__)
#include <conio.h> // _kbhit()
#endif

#include "sample_common_misc.h"
#include "mctypes.h"
#include "mcfourcc.h"
#include "mccolorspace.h"

#if defined _MSC_VER
#define strcmp_lower_case _stricmp
#else
#define strcmp_lower_case strcasecmp
#endif

auxinfo_handler_t org_auxinfo;

pic_start_info pic_info;
FILE * enc_stat_file = NULL;

void info_printf(const char * fmt, ...)
{
    char lst[1024];
    va_list marker;

    va_start(marker, fmt);
    vsnprintf(lst, sizeof(lst), fmt, marker);
    va_end(marker);

    printf("%s\n", lst);
}


void warn_printf(const char * fmt, ...)
{
    char lst[2048];
    va_list marker;

    va_start(marker, fmt);
    vsnprintf(lst, sizeof(lst), fmt, marker);
    va_end(marker);

    printf("%s\n", lst);
}


void error_printf(const char * fmt, ...)
{
    char lst[256];
    va_list marker;

    va_start(marker,fmt);
    vsnprintf(lst, sizeof(lst), fmt, marker);
    va_end(marker);

    printf("%s\n", lst);
}


void progress_printf(int32_t percent, const char * fmt, ...)
{
    char lst[256];
    va_list marker;

    va_start(marker,fmt);
    vsnprintf(lst, sizeof(lst), fmt, marker);
    va_end(marker);

    printf(" %d - %s\n", percent, lst);
}


int32_t yield()
{
    if (has_pressed_key(NULL))
        return 1;

    return 0;
}

void statistics_printf(char *fmt, ...) {
    va_list va;

    if(enc_stat_file)
    {
        fseek(enc_stat_file, 0, SEEK_END);

        va_start(va, fmt);
        vfprintf(enc_stat_file, fmt, va);
        va_end(va);

        fprintf(enc_stat_file, "\n");
        fflush(enc_stat_file);
    }
}

// resource functions dispatcher
void * MC_EXPORT_API get_rc(const char* name)
{
    if (!strcmp(name, "err_printf"))
        return (void*) error_printf;
    else if (!strcmp(name, "prg_printf"))
        return (void*) progress_printf;
    else if (!strcmp(name, "wrn_printf"))
        return (void*) warn_printf;
    else if (!strcmp(name, "inf_printf"))
        return (void*) info_printf;
    else if (!strcmp(name, "yield"))
        return (void*) yield;
    else if (!strcmp(name, "stat_printf"))
        return (void*) statistics_printf;

    return NULL;
}


#define V210_LINE_ALIGN     (128 - 1)

// This functions is deprecated. Use functions from mccolorspace.h
int32_t get_video_frame_size(int32_t width, int32_t height, uint32_t cs_fourcc, int32_t *line_size, int32_t *img_start)
{
    frame_colorspace_info_tt cs_info;
    int32_t error = get_frame_colorspace_info(&cs_info, width, height, cs_fourcc, 0);
    if(error)
        return 0;

    if(get_cs_type(cs_fourcc) == CS_FORMAT_RGB)
        flip_colorspace(&cs_info);

    *line_size = cs_info.stride[0];
    *img_start = cs_info.plane_offset[0];
    return cs_info.frame_size;
}

// This functions is deprecated. Use functions from mccolorspace.h
int32_t init_frame(frame_tt *frame, uint32_t four_cc, int32_t width, int32_t height, uint8_t *p, int32_t bottomup, int32_t stride, int32_t *plane_height)
{
    int32_t plane_count = 0;
    int32_t plane_height_local[4];
    if (!plane_height)
        plane_height = plane_height_local;

    frame_colorspace_info_tt cs_info;
    int32_t stride_by_bottomup = stride;
    if( (stride > 0 && bottomup != 0) ||
        (stride < 0 && bottomup == 0) )
    {
        stride_by_bottomup = -stride;
    }

    if(get_frame_colorspace_info(&cs_info, width, height, four_cc, stride_by_bottomup))
        return 0;

    uint32_t i;
    if(bottomup != 0)
    {
        for(i = 0; i < cs_info.planes; i++)
        {
            cs_info.plane_offset[i] += (cs_info.frame_size - abs(cs_info.stride[i]));
        }
    }

    if(fill_frame_from_colorspace_info(&cs_info, p, frame))
        return 0;
    
    int32_t chroma_format = get_chroma_format(four_cc);

    int32_t height_divider = 1;
    if(chroma_format == CHROMA420 || chroma_format == CHROMA410)
        height_divider = 2;
    else if(chroma_format == CHROMA810)
        height_divider = 4;
    
    plane_height[0] = height;
    for(i = 1; i < cs_info.planes; i++)
        plane_height[i] = height / height_divider;

    return 1;
}



// encoder auxinfo handler
uint32_t auxinfo(bufstream_tt * bs, uint32_t offs, uint32_t info_id, void * info_ptr, uint32_t info_size)
{
    sample_info_struct * sample_info;
    encode_stat_struct * encode_stat;
    v_au_struct * vau;
    pic_start_info *ref_pic_info;

    switch (info_id)
    {
    case TIME_STAMP_INFO:
        sample_info = (sample_info_struct *) info_ptr;
        break;

    case STATISTIC_INFO:
        encode_stat = (encode_stat_struct *) info_ptr;

        printf("\r %d frames encoded @ %.2f fps", encode_stat->frames_encoded, encode_stat->average_speed);
        break;

    case VIDEO_AU_CODE:
        vau = (v_au_struct *)info_ptr;
        break;

   case ID_PICTURE_START_CODE:
        ref_pic_info = (pic_start_info*)info_ptr;
        pic_info = *ref_pic_info;
        break;
    }

    // call original auxinfo handler
    return org_auxinfo(bs, offs, info_id, info_ptr, info_size);
}

int has_pressed_key( const char* text )
{
#if !defined(__linux__) && !defined(__native_client__) && !defined(__APPLE__)
    if( _kbhit() )
    {
        _getch();
        return 1;
    }
#endif
    return 0;
}



// WAVE PCM soundfile format

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT_ 0x20746d66
#define ID_DATA 0x61746164


int32_t wav_header_read(FILE *fp, wav_hdr_param* param)
{
    memset(param,0,sizeof(wav_hdr_param));
    uint32_t id;
    int32_t offset = 0;

    offset += (int32_t)fread(&id, 1, sizeof(uint32_t), fp);
    if (id != ID_RIFF)
    {
        fseek(fp, 0, 0);
        return 0;
    }

    uint32_t data_size;
    offset += (int32_t)fread(&data_size, 1, sizeof(uint32_t), fp);

    offset += (int32_t)fread(&id, 1, sizeof(uint32_t), fp);
    if (id != ID_WAVE)
    {
        fseek(fp, 0, 0);
        return 0;
    }

    while (!feof(fp))
    {
        offset += (int32_t)fread(&id, 1, sizeof(uint32_t), fp);
        switch (id)
        {
            case ID_FMT_:
            {
                uint32_t fmt_size;
                offset += (int32_t)fread(&fmt_size, 1, sizeof(uint32_t), fp);
                offset += fmt_size;

                int16_t audio_format;
                uint8_t wavx_flag = 0;
                fread(&audio_format, sizeof(uint16_t), 1, fp);
                if (audio_format == 1)
                    wavx_flag = 0;
                else if(audio_format == -2)
                    wavx_flag = 1;
                else
                {
                    fseek(fp, 0, 0);
                    return 0;
                }

                fread(&param->num_channels, sizeof(uint16_t), 1, fp);
                fread(&param->sample_rate, sizeof(uint32_t), 1, fp);
                fread(&param->bytes_per_sec, sizeof(uint32_t), 1, fp);
                fread(&param->block_align, sizeof(uint16_t), 1, fp);
                fread(&param->bits_per_sample, sizeof(uint16_t), 1, fp);

                if (wavx_flag)
                {
                    // These parameters are necessary for wav-x header type
                    uint16_t cbsize, valid_bits_per_sample;
                    uint32_t channel_mapping;
                    uint16_t subformat_guid[8];

                    if (fread(&cbsize, 2, 1, fp) != 1) 
                        return 1;
                    if (cbsize != 22) 
                        return 1;
                    if (fread(&valid_bits_per_sample, 2, 1, fp) != 1) 
                        return 1;
                    if (fread(&channel_mapping, 4, 1, fp) != 1) 
                        return 1;
                    if (fread(subformat_guid, 2, 8, fp) != 8) 
                        return 1;
                    if (fmt_size > 40)
                    {
                        if (fseek(fp, fmt_size - 40, SEEK_CUR)) 
                            return 1;
                    }
                }
                else
                {
                    // Consume any remaining bytes 
                    if (fmt_size > 16)
                    {
                        if (fseek(fp, fmt_size - 16, SEEK_CUR)) 
                            return 1;
                    }
                    //fseek(fp, offset, SEEK_SET);    //to skip extra format bytes
                }
                break;
            }
            case ID_DATA:
            {
                offset += (int32_t)fread(&param->data_size, 1, sizeof(uint32_t), fp);
                return offset;
            }
            default:
            {   //skip all not supported ids
                uint32_t chunk_id = 0;
                uint32_t chunk_size = 0;
                size_t read = 0;

                offset += (int32_t)(read = fread(&chunk_size, 1, sizeof(chunk_size), fp));
                if (read != sizeof(chunk_size))
                    break;

                offset += chunk_size;
                fseek(fp, chunk_size, SEEK_CUR);
                break;
            }
        }
    }

    //no DATA header found
    fseek(fp, 0, 0);
    return 0;
}


int32_t wav_data_read(FILE *fp, wav_hdr_param* param, uint8_t *buf, uint32_t buf_size)
{
    if (param->data_size <= 0)
        return 0;

    uint32_t len = (uint32_t) fread(buf, 1, (param->data_size < (int32_t)buf_size) ? param->data_size : buf_size, fp);

    param->data_size -= len;

    return len;
}


int32_t wav_header_write(FILE *fp, wav_hdr_param* param)
{
    fseek(fp, 0, 0);

    uint32_t id = ID_RIFF;
    fwrite(&id, sizeof(uint32_t), 1, fp);

    uint32_t data_size = param->data_size + 36;
    fwrite(&data_size, sizeof(uint32_t), 1, fp);

    id = ID_WAVE;
    fwrite(&id, sizeof(uint32_t), 1, fp);

    id = ID_FMT_;
    fwrite(&id, sizeof(uint32_t), 1, fp);

    uint32_t fmt_size = 16;
    fwrite(&fmt_size, sizeof(uint32_t), 1, fp);

    int16_t audio_format = 1;
    fwrite(&audio_format, sizeof(uint16_t), 1, fp);

    fwrite(&param->num_channels, sizeof(uint16_t), 1, fp);
    fwrite(&param->sample_rate, sizeof(uint32_t), 1, fp);
    fwrite(&param->bytes_per_sec, sizeof(uint32_t), 1, fp);
    fwrite(&param->block_align, sizeof(uint16_t), 1, fp);
    fwrite(&param->bits_per_sample, sizeof(uint16_t), 1, fp);

    id = ID_DATA;
    fwrite(&id, sizeof(uint32_t), 1, fp);

    fwrite(&param->data_size, sizeof(uint32_t), 1, fp);

    return 28 + fmt_size;
}

int32_t wav_data_write(FILE *fp, wav_hdr_param* param, uint8_t *buf, uint32_t size)
{
    int32_t len = (uint32_t) fwrite(buf, 1, size, fp);
    param->data_size += len;
    return len;
}

static void MD5Transform(uint32_t buf[4], uint32_t const in[16]);

#ifndef __BIG_ENDIAN__
# define byteReverse(buf, len)    /* Nothing */
#else
/*
 * Note: this code is harmless on little-endian machines.
 */
void byteReverse(uint32_t *buf, unsigned len)
{
  uint32_t t;
  do {
    char* bytes = (char *) buf;
    t = ((unsigned) bytes[3] << 8 | bytes[2]) << 16 |
        ((unsigned) bytes[1] << 8 | bytes[0]);
    *buf = t;
    buf++;
  } while (--len);
}
#endif

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
void MD5Init(context_md5_t *ctx)
{
  ctx->buf[0] = 0x67452301;
  ctx->buf[1] = 0xefcdab89;
  ctx->buf[2] = 0x98badcfe;
  ctx->buf[3] = 0x10325476;

  ctx->bits[0] = 0;
  ctx->bits[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void MD5Update(context_md5_t *ctx, uint8_t *buf, size_t len)
{
  uint32_t t;

  /* Update bitcount */

  t = ctx->bits[0];
  if ((ctx->bits[0] = t + ((uint32_t) len << 3)) < t)
    ctx->bits[1]++;        /* Carry from low to high */
  ctx->bits[1] += (uint32_t)len >> 29;

  t = (t >> 3) & 0x3f;    /* Bytes already in shsInfo->data */

  /* Handle any leading odd-sized chunks */

  if (t) {
    uint8_t *p = ctx->in.b8 + t;

    t = 64 - t;
    if (len < t) {
      memcpy(p, buf, len);
      return;
    }
    memcpy(p, buf, t);
    byteReverse(ctx->in.b32, 16);
    MD5Transform(ctx->buf, ctx->in.b32);
    buf += t;
    len -= t;
  }
  /* Process data in 64-byte chunks */

  while (len >= 64) {
    memcpy(ctx->in.b8, buf, 64);
    byteReverse(ctx->in.b32, 16);
    MD5Transform(ctx->buf, ctx->in.b32);
    buf += 64;
    len -= 64;
  }

    /* Handle any remaining bytes of data. */

  memcpy(ctx->in.b8, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void MD5Final(uint8_t digest[16], context_md5_t *ctx, uint32_t invert)
{
  size_t count;
  uint8_t *p;

  /* Compute number of bytes mod 64 */
  count = (ctx->bits[0] >> 3) & 0x3F;

  /* Set the first char of padding to 0x80.  This is safe since there is
     always at least one byte free */
  p = ctx->in.b8 + count;
  *p++ = 0x80;

  /* Bytes of padding needed to make 64 bytes */
  count = 64 - 1 - count;

  /* Pad out to 56 mod 64 */
  if (count < 8) {
    /* Two lots of padding:  Pad the first block to 64 bytes */
    memset(p, 0, count);
    byteReverse(ctx->in.b32, 16);
    MD5Transform(ctx->buf, ctx->in.b32);

    /* Now fill the next block with 56 bytes */
    memset(ctx->in.b8, 0, 56);
  } else {
    /* Pad block to 56 bytes */
    memset(p, 0, count - 8);
  }
  byteReverse(ctx->in.b32, 14);

  /* Append length in bits and transform */
  ctx->in.b32[14] = ctx->bits[0];
  ctx->in.b32[15] = ctx->bits[1];

  MD5Transform(ctx->buf, ctx->in.b32);
  byteReverse((uint32_t *) ctx->buf, 4);
  if (invert) {
    uint8_t *p = (uint8_t*)(ctx->buf);
    for (int32_t i = 15; i >= 0; i--)
      digest[i] = *p++;
  } else
    memcpy(digest, ctx->buf, 16);

  memset(ctx, 0, sizeof(* ctx));    /* In case it's sensitive */
  /* The original version of this code omitted the asterisk. In
     effect, only the first part of ctx was wiped with zeros, not
     the whole thing. Bug found by Derek Jones. Original line: */
  // memset(ctx, 0, sizeof(ctx));    /* In case it's sensitive */
}

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
    ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void MD5Transform(uint32_t buf[4], uint32_t const in[16])
{
  register uint32_t a, b, c, d;

  a = buf[0];
  b = buf[1];
  c = buf[2];
  d = buf[3];

  MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
  MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
  MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
  MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
  MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
  MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
  MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
  MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
  MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
  MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
  MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
  MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
  MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
  MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
  MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
  MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

  MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
  MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
  MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
  MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
  MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
  MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
  MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
  MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
  MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
  MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
  MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
  MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
  MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
  MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
  MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
  MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

  MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
  MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
  MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
  MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
  MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
  MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
  MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
  MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
  MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
  MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
  MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
  MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
  MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
  MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
  MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
  MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

  MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
  MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
  MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
  MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
  MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
  MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
  MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
  MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
  MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
  MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
  MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
  MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
  MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
  MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
  MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
  MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}