#ifndef _SENSOR_H_
#define _SENSOR_H_
struct sensor_reg
{
	unsigned int sensor_addr;
	unsigned int sensor_value;
};

//Sensor type definition
#define OV7670		0
#define OV7725		1
#define MT9V034		2
#define AR0134		3
#define MT9J001		4
#define MT9F002		5
#define MT9N001		6
#define OV2640		7
#define OV5642		8
#define OV5640		9
#define MT9M001		10
#define MT9M112		11
#define MT9D111		12
#endif
