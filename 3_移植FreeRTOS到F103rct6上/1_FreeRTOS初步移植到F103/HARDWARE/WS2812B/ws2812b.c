#include "ws2812b.h"
#include "stdlib.h"
#include "delay.h"

void WS2812_Send(uint8_t (*color)[3], uint16_t len)
{

	uint8_t i;
	uint16_t memaddr;
	uint16_t buffersize;
	buffersize = (len*24)+43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
	memaddr = 0;				// reset buffer memory index

	while (len)
	{	
		
		for(i=0; i<8; i++) // RED
		{
			LED_BYTE_Buffer[memaddr] = ((color[len-1][1]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
			memaddr++;
		}
		
		for(i=0; i<8; i++) // GREEN data
		{
			LED_BYTE_Buffer[memaddr] = ((color[len-1][0]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
			memaddr++;
		}
		
		for(i=0; i<8; i++) // BLUE
		{
			LED_BYTE_Buffer[memaddr] = ((color[len-1][2]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
			memaddr++;
		}
		len--;
	}
//===================================================================//	
//bug：最后一个周期波形是高电平，故增加一个低电平的波形
		LED_BYTE_Buffer[memaddr] = 0;
//===================================================================//	
	  memaddr++;	
		while(memaddr < buffersize)
		{
			LED_BYTE_Buffer[memaddr] = 0;
			memaddr++;
		}

		DMA_SetCurrDataCounter(DMA1_Channel7, buffersize); 	// load number of bytes to be transferred
		DMA_Cmd(DMA1_Channel7, ENABLE); 			// enable DMA channel 7
		TIM_Cmd(TIM2, ENABLE); 						// enable Timer 2
		while(!DMA_GetFlagStatus(DMA1_FLAG_TC7)) ; 	// wait until transfer complete
		DMA_Cmd(DMA1_Channel7, DISABLE); 			// disable DMA channel 7
		DMA_ClearFlag(DMA1_FLAG_TC7); 				// clear DMA1 Channel 7 transfer complete flag
		TIM_Cmd(TIM2, DISABLE); 	// disable Timer 2
}
void WS2812_Random()	//随机闪灯
{
		uint8_t rgb0[1][3];
	  rgb0[0][0] = rand()%256;
		rgb0[0][1] = rand()%256;
		rgb0[0][2] = rand()%256;
		WS2812_Send(&rgb0[0],1);
		delay_s(1);
}

void WS2812_Red_BLN()	//红色呼吸灯
{
	u16 i;
	uint8_t rgb0[256][3] ={0,0,0};
	for(i=0;i<=255;i++)
	{
		rgb0[i][0] = i;
	}
	for(i=0;i<=255;i++)
	{
		WS2812_Send(&rgb0[i],1);
		delay_ms(5);
	}
	for(i=0;i<=255;i++)
	{
		rgb0[i][0] = (255 - i);
	}
	for(i=0;i<=255;i++)
	{
		WS2812_Send(&rgb0[i],1);
		delay_ms(5);
	}
	delay_ms(500);
	
}

void WS2812_Green_BLN()	//绿色呼吸灯
{
	u16 i;
	uint8_t rgb0[256][3] ={0,0,0};
	for(i=0;i<=255;i++)
	{
		rgb0[i][1] = i;
	}
	for(i=0;i<=255;i++)
	{
		WS2812_Send(&rgb0[i],1);
		delay_ms(5);
	}
	for(i=0;i<=255;i++)
	{
		rgb0[i][1] = (255 - i);
	}
	for(i=0;i<=255;i++)
	{
		WS2812_Send(&rgb0[i],1);
		delay_ms(5);
	}
	delay_ms(500);
}
void WS2812_Blue_BLN()	//蓝色呼吸灯
{
	u16 i;
	uint8_t rgb0[256][3] ={0,0,0};
	for(i=0;i<=255;i++)
	{
		rgb0[i][2] = i;
	}
	for(i=0;i<=255;i++)
	{
		WS2812_Send(&rgb0[i],1);
		delay_ms(5);
	}
	for(i=0;i<=255;i++)
	{
		rgb0[i][2] = (255 - i);
	}
	for(i=0;i<=255;i++)
	{
		WS2812_Send(&rgb0[i],1);
		delay_ms(5);
	}
	delay_ms(500);
}	




