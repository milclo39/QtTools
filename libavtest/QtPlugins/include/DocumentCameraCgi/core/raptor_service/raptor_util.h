#ifndef raptor_util_h
#define raptor_util_h

#include <sstream>
#include "raptor_service/picojson.h"
#include "raptor_service/json_util.h"
#include "raptor_service/raptor_defs.h"

typedef struct
{
	char* data;   // response data from server
	size_t size;  // response size of data
} MEMFILE;

inline MEMFILE* memfopen()
{
	MEMFILE* mf = (MEMFILE*) malloc(sizeof(MEMFILE));
	mf->data = NULL;
	mf->size = 0;
	return mf;
}

inline void memfclose(MEMFILE* mf)
{
	if (mf->data) free(mf->data);
	free(mf);
}

inline size_t memfwrite(char* ptr, size_t size, size_t nmemb, void* stream)
{
	MEMFILE* mf = (MEMFILE*) stream;
	int block = size * nmemb;
	if (!mf->data)
		mf->data = (char*) malloc(block);
	else
		mf->data = (char*) realloc(mf->data, mf->size + block);
    if (mf->data)
    {
		memcpy(mf->data + mf->size, ptr, block);
		mf->size += block;
	}
	return block;
}

inline char* memfstrdup(MEMFILE* mf)
{
	char* buf = (char*)malloc(mf->size + 1);
	memcpy(buf, mf->data, mf->size);
	buf[mf->size] = 0;
	return buf;
}

#endif

