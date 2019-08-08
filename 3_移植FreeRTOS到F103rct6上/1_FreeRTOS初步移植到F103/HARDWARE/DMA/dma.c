#include "dma.h"

uint16_t LED_BYTE_Buffer[300];

void TIM2_DMA_Init(void){ //DMA��ʼ������
	
	/* configure DMA */
	DMA_InitTypeDef DMA_InitStructure;//����DMA��ʼ���ṹ��

	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	//ʹ��DMAʱ�ӣ�����SPI�����ݴ��䣩
	
	/* DMA1 Channel7 Config for PWM4 by TIM2_CH4*/
	DMA_DeInit(DMA1_Channel7);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM2_CCR4_Address;	// physical address of Timer 3 CCR1
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED_BYTE_Buffer;		// this is the buffer memory 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						// data shifted from memory to peripheral
	DMA_InitStructure.DMA_BufferSize = 300;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// stop DMA feed after buffer size is reached
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);

	/* TIM2 DMA Request enable */
	TIM_DMACmd(TIM2, TIM_DMA_CC4, ENABLE);
	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
	
	
}




