#include "led.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
/************************************************
 ALIENTEK Mini STM32F103������ FreeRTOSʵ��2-1
 FreeRTOS��ֲʵ��-�⺯���汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//��ʼ����
TaskHandle_t StartTask_Handler;
#define START_STK_SIZE   128
#define START_TASK_PRIO  1

//TASK1������
TaskHandle_t Task1Task_Handler;
#define TASK1_STK_SIZE 128
#define TASK1_TASK_PRIO 2

//TASK2������
TaskHandle_t Task2Task_Handler;
#define TASK2_STK_SIZE 128
#define TASK2_TASK_PRIO 3
 

//��������
void start_task(void *pvParameters);
void task1_task(void *pvParameters);
void task2_task(void *pvParameters);


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED

	xTaskCreate(	(TaskFunction_t 	)start_task,    				 //��ʼ������
								(const char *			) "start_task",					 //��������
								(uint16_t					)START_STK_SIZE  ,       //�����ջ��С
								(void * 					)NULL,									 //���ݸ��������Ĳ���
								(UBaseType_t 			)START_TASK_PRIO,				 //�������ȼ�
								(TaskHandle_t * 	) &StartTask_Handler );  //������
							
	 vTaskStartScheduler();          //�����������
}

//��ʼ������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //�����ٽ���
	//����TASK1����
	xTaskCreate(	(TaskFunction_t 	)task1_task,    				 //��ʼ������
								(const char *			) "task1_task",					 //��������
								(uint16_t					)TASK1_STK_SIZE  ,       //�����ջ��С
								(void * 					)NULL,									 //���ݸ��������Ĳ���
								(UBaseType_t 			)TASK1_TASK_PRIO,				 //�������ȼ�
								(TaskHandle_t * 	) &Task1Task_Handler );  //������
								
	//����TASK2����
	xTaskCreate(	(TaskFunction_t 	)task2_task,    				 //��ʼ������
								(const char *			) "task2_task",					 //��������
								(uint16_t					)TASK2_STK_SIZE  ,       //�����ջ��С
								(void * 					)NULL,									 //���ݸ��������Ĳ���
								(UBaseType_t 			)TASK2_TASK_PRIO,				 //�������ȼ�
								(TaskHandle_t * 	) &Task2Task_Handler );  //������
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
void task1_task(void *pvParameters)
{
	uint8_t cnt = 0;
	while(1)
	{
		cnt++;
		LED0 = ~LED0;
		
		if ( Task2Task_Handler != NULL)  //����2�Ƿ����
		{
			if (cnt == 5)
			{
				
				vTaskDelete(Task2Task_Handler);  //����1ִ��5��ɾ������2
				Task2Task_Handler = NULL;    		//����������
			}
			
		}
		vTaskDelay(1000);
	}
}

//task2������
void task2_task(void *pvParameters)
{
	while(1)
	{
		LED1 = ~LED1;
		vTaskDelay(500);
	}

}






