#ifndef __WS2812B_H
#define	__WS2812B_H

#define TIMING_ONE  50
#define TIMING_ZERO 16

#include "sys.h"

extern uint16_t LED_BYTE_Buffer[300];

void WS2812_Send(uint8_t (*color)[3], uint16_t len);

void WS2812_Random(void); 			//�������
void WS2812_Red_BLN(void);			//��ɫ������
void WS2812_Green_BLN(void);		//��ɫ������
void WS2812_Blue_BLN(void);			//��ɫ������

#endif /* __LED_H */
