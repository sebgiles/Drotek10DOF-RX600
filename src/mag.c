#include "mag.h"
#include "i2c.h"
#include "ms.h"
#include "r_riic_rx600_master.h"

#define HMC5983_ADDRESS 0x1E
#define HMC5983_WRITE	0x3C
#define HMC5983_READ 	0x3D

// MEMORY MAPPING
/*
Address Location	Name 		Access
---------------------------------------------------
0x00	Configuration Register A	Read/Write
0x01	Configuration Register B	Read/Write
0x02	Mode Register			Read/Write
0x03	Data Output X MSB Register	Read
0x04	Data Output X LSB Register	Read
0x05	Data Output Z MSB Register	Read
0x06	Data Output Z LSB Register	Read
0x07	Data Output Y MSB Register	Read
0x08	Data Output Y LSB Register	Read
0x09	Status Register			Read
0x0A	Identification Register A	Read
0x0B	Identification Register B	Read
0x0C	Identification Register C	Read
0x31	Temperature Output MSB Register	Read
0x32	Temperature Output LSB Register Read
 */

#define HMC5983_CONF_A		0x00
#define HMC5983_CONF_B		0x01
#define HMC5983_MODE		0x02
#define HMC5983_OUT_X_MSB	0x03
#define HMC5983_OUT_X_LSB	0x04
#define HMC5983_OUT_Z_MSB	0x05
#define HMC5983_OUT_Z_LSB	0x06
#define HMC5983_OUT_Y_MSB	0x07
#define HMC5983_OUT_Y_LSB	0x08
#define HMC5983_STATUS		0x09
#define HMC5983_ID_A		0x0A
#define HMC5983_ID_B		0x0B
#define HMC5983_ID_C		0x0C
#define HMC5983_TEMP_OUT_MSB	0x31
#define HMC5983_TEMP_OUT_LSB	0x32

#define HMC5983_ID_A_VAL    0x48


/*

CONF_A ========================================================

|BIT1|BIT0| MEASUREMENT MODE  |
| 0  | 0  | normal (default)  |
| 0  | 1  | positive bias	    |
| 1  | 0  | negative bias	    |
| 1  | 1  | temp only			    |

|BIT4|BIT3|BIT2| MEASUREMENT RATE [Hz] (in continuous mode)
| 0  | 0  | 0  | 0.75			    |
| 0  | 0  | 1  | 1.5 			    |
| 0  | 1  | 0  | 3				    |
| 0  | 1  | 1  | 7.5				  |
| 1  | 0  | 0  | 15	(default) |
| 1  | 0  | 1  | 30					  |
| 1  | 1  | 0  | 75					  |
| 1  | 1  | 1  | 220				  |

|BIT6|BIT5| AVERAGING WINDOW [samples]  |
| 0  | 0  | 1 (default)				          |
| 0  | 1  | 2           	              |
| 1  | 0  | 4           	              |
| 1  | 1  | 8       			              |

BIT7 = enable temperature sensor

CONF_B =========================================================

|BIT7|BIT6|BIT5| RESOLUTION [mG/LSb]  | Campo max (Gauss)
| 0  | 0  | 0  | 0.73			            | 0.88
| 0  | 0  | 1  | 0.92 (default)			  | 1.3
| 0  | 1  | 0  | 1.22				          | 1.9
| 0  | 1  | 1  | 1.52				          | 2.5
| 1  | 0  | 0  | 2.27                 | 4.0
| 1  | 0  | 1  | 2.56 			          | 4.7
| 1  | 1  | 0  | 3.03				          | 5.6
| 1  | 1  | 1  | 4.35 			          | 8.1

MODE ===========================================================

|BIT0| OPERATING MODE   |
| 0  | continuous		    |
| 1  | single (default)	|

BIT1 = enable idle mode
BIT7 = enable 3400khz i2c mode
BIT5 = enter low power mode
 */


float sens = 0.92;

int mag_init(){

	uint8_t tmp;

	// se stiamo usando il modulo drotek chiediamo ad MPU6050 di
	// farci accedere al magnetometro che è suo slave
	// altrimenti basta avviare l'i2c
#ifdef DROTEK_IMU_10DOF_V2
	if(mpu_set_bypass(1))
		return 0x1;
#else
	if(i2c_init())
		return 0x1;
#endif

	// leggiamo il registro identificativo e verifichiamo che abbia
	// il valore costante come da datasheet
	if(i2c_read(HMC5983_ADDRESS, HMC5983_ID_A, 1, &tmp))
		return 0x2;
	if(tmp != HMC5983_ID_A_VAL)
		return 0x3;


	// abilitiamo la continuous mode
	tmp = 0;
	if(i2c_write(HMC5983_ADDRESS, HMC5983_MODE, 1, &tmp))
		return 0x4;
  //scarico registro CONF A
  if(i2c_read(HMC5983_ADDRESS, HMC5983_CONF_A, 1, &tmp))
  	return 0x4;
  //imposto frequenza a 220 Hz
  tmp |= 0x1C;
  //carico il registro modificato
	if(i2c_write(HMC5983_ADDRESS, HMC5983_CONF_A, 1, &tmp))
		return 0x5;
  //scarico registro CONF B
  if(i2c_read(HMC5983_ADDRESS, HMC5983_CONF_B, 1, &tmp))
    return 0x6;
  tmp = tmp >> 5;
  switch(tmp) {
    case 0: sens = 0.73; break;
    case 1: sens = 0.92; break;
    case 2: sens = 1.22; break;
    case 3: sens = 1.52; break;
    case 4: sens = 2.27; break;
    case 5: sens = 2.56; break;
    case 6: sens = 3.03; break;
    case 7: sens = 4.35; break;
    default: break;
  }
	return 0x0;

}

int mag_read_raw(Z3* data){
  uint8_t tmp[6];

  if(i2c_read(HMC5983_ADDRESS, HMC5983_OUT_X_MSB, 6, tmp))
    return 0x1;
    
  data->x = (tmp[0] << 8) | tmp[1];
  data->y = (tmp[2] << 8) | tmp[3];
  data->z = (tmp[4] << 8) | tmp[5];

  return 0x0;
}

int mag_read(R3* data){
  Z3 temp;

  if (mag_read_raw(&temp))
    return 0x1;

  data->x = temp.x * sens;
  data->y = temp.y * sens;
  data->z = temp.z * sens;

  return 0x0;
}
