#ifndef _COMMON_H_
#define _COMMON_H_

#pragma pack(8)

#include "stdafx.h"
#include "ArduCamlib.h"



typedef struct  
{
	Uint32		u32Page;
	Uint32		u32Addr;
	Uint32		u32MinValue;
	Uint32		u32MaxValue;
}GainSet;



#endif