#include "imu.h"

#include "eMPL/inv_mpu.h" //libreria Invensense adattata per renesas rx
#include "i2c.h"

int imu_init(){
	if (i2c_init())
		return 0x1;
	if (mpu_init(0))
		return 0x2;
	if (mpu_set_sensors(INV_XYZ_ACCEL | INV_XYZ_GYRO))
		return 0x3;
	else
		return 0x0;
}
