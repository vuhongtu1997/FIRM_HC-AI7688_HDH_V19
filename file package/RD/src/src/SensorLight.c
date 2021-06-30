/*
 * SensorLight.c
 */

#include "SensorLight.h"
#include "Provision.h"
#include "Light.h"

lightsensorRsp * vrts_LighSensor_Rsp;
pirsensorRsp * vrts_PirSensor_Rsp;
pmsensorRsp * vrts_PMSensor_Rsp;
//bool flag_sensor_light_rsp = false;
uint16_t  value_Lux = 0;
static uint16_t luxReg;

unsigned int CalculateLux(unsigned int rsp_lux)
{
	unsigned int lux_LSB = 0;
	unsigned char lux_MSB = 0;
	unsigned int lux_Value = 0;
	unsigned int pow = 1;
	unsigned char i;
	lux_LSB = rsp_lux & 0x0FFF;
	lux_MSB = ((rsp_lux>>12) & 0x0F);
	//Lux_Value = 0.01 * pow(2,Lux_MSB) * Lux_LSB; //don't use
	for(i=0;i<lux_MSB;i++){
		pow=pow*2;
	}
	lux_Value=0.01 * pow * lux_LSB;
	return lux_Value;
}
void ProcessLightSensor(lightsensorRsp *rsp)
{
	luxReg=rsp->luxValue[1] | (rsp->luxValue[0]<<8);
	value_Lux = CalculateLux(luxReg);
	//printf ("Lux= %d\n",value_Lux);
}

