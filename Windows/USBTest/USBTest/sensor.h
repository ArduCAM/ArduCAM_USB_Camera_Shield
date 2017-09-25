#ifndef _SENSOR_H_
#define _SENSOR_H_

//Sensor type definition
#define OV7670		0
#define OV7725		1
#define MT9V034		2
#define AR0134		3
#define MT9J001		4
#define MT9F002		5
#define MT9N001		6


typedef struct
{
	Uint32 sensor_addr;
	Uint32 sensor_value;
}sensor_reg;

typedef struct
{
	Uint16 u16Type;
	Uint16 u16Para1;
	Uint16 u16Para2;
	Uint16 u16Bak;
}SensorReg;

#endif