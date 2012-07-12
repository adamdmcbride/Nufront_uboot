//------------------------------------------------------------------------------
// Module Name:  AMF_Lib Header
// Module Description:  Header File for Auto Mode Firmware Library
//
//------------------------------------------------------------------------------
// Copyright © 2009, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------
#ifndef __AMF_LIB_H__
#define __AMF_LIB_H__


//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define COLOR_DEPTH_8BIT    0
#define COLOR_DEPTH_10BIT   1
#define COLOR_DEPTH_12BIT   2

#define PICTURE_ASPECT_RATIO_MASK	0x30
#define	PICTURE_ASPECT_RATIO_4x3	0x10
#define PICTURE_ASPECT_RATIO_16x9	0x20

//------------------------------------------------------------------------------
// Video Information Structure
//------------------------------------------------------------------------------
typedef struct
{
    unsigned short vertical_lines_total;
    unsigned short horizontal_pixels_total;
    unsigned short vertical_lines_active;
    unsigned short horizontal_pixels_active;
    unsigned char  color_depth;
} VideoFormat_t;

//------------------------------------------------------------------------------
// AVI Info Frame Structure
//------------------------------------------------------------------------------
typedef struct
{
    unsigned char  type_code;
    unsigned char  version_number;
    unsigned char  length;
	unsigned char  checksum;
    unsigned char  byte_1;
    unsigned char  byte_2;
    unsigned char  byte_3;
    unsigned char  byte_4;
    unsigned char  byte_5;
    unsigned char  byte_6;
    unsigned char  byte_7;
    unsigned char  byte_8;
    unsigned char  byte_9;
    unsigned char  byte_10;
    unsigned char  byte_11;
    unsigned char  byte_12;
    unsigned char  byte_13;
	unsigned char  byte_14;
	unsigned char  byte_15;
} AVIInfoFormat_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
void AMF_Init(byte dbg);
unsigned char AMF_Poll(byte dbg);
void AMF_GetVideoInformation(VideoFormat_t* video_format);
unsigned char AMF_GetAviInfoFrame(AVIInfoFormat_t * avi_info);

//------------------------------------------------------------------------------
// Obsolete Functions
//------------------------------------------------------------------------------
//unsigned short AMF_GetVerticalResolution(void);


#endif  // __AMF_LIB_H__

