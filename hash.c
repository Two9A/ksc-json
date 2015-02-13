#include "hash.h"

unsigned short crc16(const char *buf, int len)
{
	unsigned short crc = 0, i;
	while(len--)
	{
		crc ^= (*buf++)<<8;
		for(i=0; i<8; i++)
		{
			if(crc & 0x8000) crc = (crc<<1)^0x8005;
			else crc <<= 1;
		}
	}
	return crc;
}

