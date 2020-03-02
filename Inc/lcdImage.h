/*
 * lcdImage.h
 *
 *  Created on: 2019年8月21日
 *      Author: Administrator
 */

#ifndef LCDIMAGE_H_
#define LCDIMAGE_H_
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"
#define info(x) {sizeof(x),x}
struct imageInfo{
	int length;
	const uint8_t * p;
};
extern const struct imageInfo bigNumList[];
extern const struct imageInfo smallNumList[];
extern const struct imageInfo unit[];
extern const unsigned char gImage_locationMesg[672];
extern const struct imageInfo circleList[];
extern const struct imageInfo arraowList[];
extern const struct imageInfo setPageList[];
extern const unsigned char passWordInputPage[77440];
extern const unsigned char passwordWord[264];
extern const unsigned char otherSetPage[77440];
extern const struct imageInfo managerList[];
extern struct imageInfo detectPageList[];
extern struct imageInfo demarcatePage[];
extern struct imageInfo gImageErrorList[];
extern const unsigned char measureRange[];
extern const unsigned char segmentationSymbols[328];
#endif /* LCDIMAGE_H_ */
