#include <stdint.h>
#include <stdio.h>
#include "platform.h"

#include "ms.h"
#include "imu.h"
#include "mag.h"

#define AHRS_PERIOD 5 //ms

int err;

void main(void)
{   
	//avvia timer
	ms_init();

	/* Initialize LCD */
	lcd_initialize();
	lcd_clear();
	lcd_display(LCD_LINE8, "no errors");
	char msg[12];

	err = imu_init();
	if (err){
		lcd_display(LCD_LINE7, "imu_init");
		sprintf(msg,"error 0x%x",err);
		lcd_display(LCD_LINE8, (uint8_t*)msg);
		while(1);
	}

	err = mag_init();
	if (err){
		lcd_display(LCD_LINE7, "mag_init");
		sprintf(msg,"error 0x%x",err);
		lcd_display(LCD_LINE8, (uint8_t*)msg);
		while(1);
	}

	Z3 raw;
	R3 scaled;

	err = mag_read_raw(&raw);
	if (err){
		lcd_display(LCD_LINE7, "mag_read_raw");
		sprintf(msg,"error 0x%x",err);
		lcd_display(LCD_LINE8, (uint8_t*)msg);
		while(1);
	}






	//main loop
	while(1){

		err = mag_read_raw(&raw);
		if (err){
			lcd_display(LCD_LINE7, "mag_read_raw");
			sprintf(msg,"error 0x%x",err);
			lcd_display(LCD_LINE8, (uint8_t*)msg);
			while(1);
		}

		err = mag_read(&scaled);
		if (err){
			lcd_display(LCD_LINE7, "mag_read");
			sprintf(msg,"error 0x%x",err);
			lcd_display(LCD_LINE8, (uint8_t*)msg);
			while(1);
		}

		sprintf(msg,"%d", raw.x);
		lcd_display(LCD_LINE2, (uint8_t*)msg);
		sprintf(msg,"%d", raw.y);
		lcd_display(LCD_LINE3, (uint8_t*)msg);
		sprintf(msg,"%d", raw.z);
		lcd_display(LCD_LINE4, (uint8_t*)msg);
		sprintf(msg,"%f", scaled.x);
		lcd_display(LCD_LINE5, (uint8_t*)msg);
		sprintf(msg,"%f", scaled.y);
		lcd_display(LCD_LINE6, (uint8_t*)msg);
		sprintf(msg,"%f", scaled.z);
		lcd_display(LCD_LINE7, (uint8_t*)msg);

		sprintf(msg,"t = %d s", ms()/1000);
		lcd_display(LCD_LINE1, (uint8_t*)msg);

		delay_ms(10);
	}

}

#if 0
	/*configura IMU per leggere accelerometro e giroscopio*/
	result = mpu_set_sensors(INV_XYZ_ACCEL | INV_XYZ_GYRO);
	if (result) {
		lcd_display(LCD_LINE1, "imu config err");
		char msg[10];
		sprintf(msg,"err: %x",result);
		lcd_display(LCD_LINE3, (uint8_t*)msg);

		while(1);
	}



	/*variabili per dati letti*/
	/*terne ordinate xyz*/
	int16_t rawacc[3], rawgyr[3];
	float acc[3],  gyr[3];
	uint16_t acc_sens;
	float gyr_sens;


	mpu_get_accel_sens(&acc_sens);
	mpu_get_gyro_sens(&gyr_sens);

	volatile float yaw,pitch,roll;
	float qa,qb,qc,qd;



	/*variabili di temporizzazione*/
	int32_t now = 0, last = 0;

	/*avvisa l'algoritmo AHRS della frequenza con cui gli arriveranno i dati */
	setAHRSFrequency(1000.0/(float)AHRS_PERIOD);

	while (1)
	{

		do {
			now=read_ms();
		} while (now-last<AHRS_PERIOD);
		last = now;

		sprintf(msg,"t = %ld",now);
		lcd_display(LCD_LINE4, (uint8_t*)msg);

		result = mpu_get_accel_reg(rawacc, NULL);
		result |= mpu_get_gyro_reg(rawgyr, NULL);

		if (result) {
			lcd_display(LCD_LINE1, "imu read err");
			char msg[10];
			sprintf(msg,"err: %x",result);
			lcd_display(LCD_LINE3, (uint8_t*)msg);
			while(1);
		}

		for(int i=0; i<3; i++){
			acc[i]=(float)rawacc[i]/acc_sens;
			gyr[i]=(float)rawgyr[i]/gyr_sens*0.01745329252;
		}

		madgwickFilterUpdate(	gyr[0], gyr[1], gyr[2],
				acc[0], acc[1], acc[2],
				0.0f, 0.0f, 0.0f);



		getAHRS(&pitch, &yaw, &roll);
		/*getQuat(&qa,&qb,&qc,&qd);
		 */
		sprintf(msg,"%f",acc[2]);
		lcd_display(LCD_LINE2, (uint8_t*)msg);
		/*
		sprintf(msg,"%f",qb);
		lcd_display(LCD_LINE3, (uint8_t*)msg);
		 */

		sprintf(msg,"y = %f",yaw);
		lcd_display(LCD_LINE5, (uint8_t*)msg);
		sprintf(msg,"p = %f",pitch);
		lcd_display(LCD_LINE6, (uint8_t*)msg);
		sprintf(msg,"r = %f",roll);
		lcd_display(LCD_LINE7, (uint8_t*)msg);



	}
}  */ /* End of function main() */

#endif

