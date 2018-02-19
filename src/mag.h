/*
 * mag.h
 *
 *  Created on: 24 gen 2018
 *      Author: Sebastian
 */

#ifndef SRC_MAG_H_
#define SRC_MAG_H_



#ifdef DROTEK_IMU_10DOF_V2
#include "eMPL/inv_mpu.h"
#endif

typedef struct{
  short x,y,z;
} Z3;


typedef struct {
  float x,y,z;
} R3;

int mag_init();

int mag_read_raw(Z3* data);
int mag_read(R3* data);

#endif /* SRC_MAG_H_ */
