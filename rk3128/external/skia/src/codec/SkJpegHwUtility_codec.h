/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkJpegUtility_codec_DEFINED
#define SkJpegUtility_codec_DEFINED

#include "SkStream.h"

#include <setjmp.h>
// stdio is needed for jpeglib
#include <stdio.h>

extern "C" {
     #include "vpu_mem.h"
     #include "hw_jpegdecapi.h"
//    #include "jpeglib.h"
//    #include "jerror.h"
}

/*
 * Error handling struct
 */
//struct skjpeg_error_mgr : jpeg_error_mgr {
//    jmp_buf fJmpBuf;
//};

/*
 * Error handling function
 */
//void skjpeg_err_exit(HwJpegInputInfo cinfo);

static int sk_fill_thumb(HwJpegInputInfo* hwInfo, void * thumbBuf);
/*
 * Source handling struct for that allows libjpeg to use our stream object
 */
struct sk_hw_jpeg_source_mgr: hw_jpeg_source_mgr {
    sk_hw_jpeg_source_mgr(SkStream* stream,HwJpegInputInfo* hwInfo, int vpuMem);
    ~sk_hw_jpeg_source_mgr();

    SkStream* fStream; // unowned
    enum {
        // TODO (msarett): Experiment with different buffer sizes.
        // This size was chosen because it matches SkImageDecoder.
        kBufferSize = 1024
    };
    uint8_t fBuffer[kBufferSize];
};

class SkJpegVPUMemStream : public SkMemoryStream {
public:
	size_t bytesInStream;//if equal -1 , means the data in this stream is error
	SkStream *baseStream;
	SkJpegVPUMemStream(SkStream* stream, size_t* len);
	virtual ~SkJpegVPUMemStream();
	VPUMemLinear_t* getVpuMemInst();
	void setNewMemory(VPUMemLinear_t* src, size_t size);
private:
	VPUMemLinear_t vpuMem;
};


#endif
