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

/********************************************
**ʹ�þ�̬������������ 
**��һ������FreeRTOSConfig.h����Ӷ���  #define configSUPPORT_STATIC_ALLOCATION          1            //ʹ�ܾ�̬��������  
**�ڶ������Զ���vApplicationGetIdleTaskMemory��vApplicationGetTimerTaskMemory����
**����������������
****
***********************************************/

//��ʼ����
TaskHandle_t StartTask_Handler;
#define START_STK_SIZE 		128
#define START_TASK_PRIO		1
StackType_t StartTaskStack[START_STK_SIZE];
StaticTask_t StartTaskTCB;

//����TASK1����
TaskHandle_t Task1Task_Handler;
#define TASK1_STK_SIZE 		128
#define TASK1_TASK_PRIO		1
StackType_t Task1TaskStack[START_STK_SIZE];
StaticTask_t Task1TaskTCB;

//����TASK2����
TaskHandle_t Task2Task_Handler;
#define TASK2_STK_SIZE 		128
#define TASK2_TASK_PRIO		1
StackType_t Task2TaskStack[START_STK_SIZE];
StaticTask_t Task2TaskTCB;

//������������Ҫ�ı���
static StaticTask_t IdleTaskTCB;  //���ƿ�
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];  //���ƶ�ջ

//��ʱ����������Ҫ�ı���
static StaticTask_t TimerTaskTCB;
static StackType_t  TimerTaskStack[configTIMER_TASK_STACK_DEPTH];


//��ʱ�����������ڴ�
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &TimerTaskTCB;
	*ppxTimerTaskStackBuffer = TimerTaskStack;
	*pulTimerTaskStackSize = configMINIMAL_STACK_SIZE;  //�����ʱ�������ջ��С
}


//�������������ڴ�
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, 
									StackType_t **ppxIdleTaskStackBuffer,
									uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

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

	StartTask_Handler = xTaskCreateStatic(	(TaskFunction_t		) start_task,    	//������
											(const char *		)  "start_task",	//��������
											(uint32_t			) START_STK_SIZE,	//�����ջ��С
											(void * 			)NULL,				//���ݸ��������Ĳ���
											(UBaseType_t		) START_TASK_PRIO,	//�������ȼ�
											(StackType_t *		) StartTaskStack ,	//�����ջ
											(StaticTask_t *		) &StartTaskTCB );	//������ƿ�
							
	 vTaskStartScheduler();          //�����������

	
	
		//��ϵͳд�����õ�whileѭ��
//	while(1)
//	{
//	
//	}		
	
//    vTaskStartScheduler();          //�����������
}


void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //�����ٽ���
	//����TASK1����
	Task1Task_Handler=xTaskCreateStatic((TaskFunction_t	)task1_task,
										(const char* 	)"task1_task",
										(uint32_t 		)TASK1_STK_SIZE,
										(void* 		  	)NULL,
										(UBaseType_t 	)TASK1_TASK_PRIO,
										(StackType_t*   )Task1TaskStack,
										(StaticTask_t*  )&Task1TaskTCB);
	//����TASK2����
	Task2Task_Handler=xTaskCreateStatic((TaskFunction_t	)task2_task,
										(const char* 	)"task2_task",
										(uint32_t 		)TASK2_STK_SIZE,
										(void* 		  	)NULL,
										(UBaseType_t 	)TASK2_TASK_PRIO,
										(StackType_t*   )Task2TaskStack,
										(StaticTask_t*  )&Task2TaskTCB);
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
	taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
void task1_task(void *pvParameters)
{
	int num = 0;
	while(1)
	{
		num++;
		LED0 = ~LED0;
		vTaskDelay(2000);  //ms
		if (num > 3)
		{
			if ( Task2Task_Handler!= NULL )  //����2�Ƿ����
			{
				vTaskDelete(Task2Task_Handler);  //����1ִ��3��ɾ������2
				Task2Task_Handler = NULL;  //����������
			}
		}
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







