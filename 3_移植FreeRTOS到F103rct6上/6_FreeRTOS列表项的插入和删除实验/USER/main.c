#include "led.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

#include "key.h"

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

//����һ�������õ��б���б���
List_t TestList;   //�������б�
ListItem_t ListItem1;  //�������б���1
ListItem_t ListItem2;  //�������б���2
ListItem_t ListItem3;  //�������б���3


/*************************************
*����˵����start_task�����������ڴ�������
*          task1_task����һ��С�ƣ�������ʾ��������
*          task2_task�����б���б��ʵ��
*
**************************************/

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
	KEY_Init();               //��ʼ������
	
	
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
	while(1)
	{
		LED0 = ~LED0;
		vTaskDelay(500);  //ms
	}
}

//task2������
//�����б���б����ʵ��
void task2_task(void *pvParameters)
{
	//��һ������ʼ���б���б���
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	ListItem1.xItemValue = 40;  //ListItem1�б���ֵΪ40
	ListItem2.xItemValue = 60;  //ListItem2�б���ֵΪ60
	ListItem3.xItemValue = 50;  //ListItem3�б���ֵΪ50 
	
	//�ڶ�������ӡ�б�������б���ĵ�ַ
	printf("/******************�б���б����ַ**************************/\r\n");
	printf("��Ŀ                       ��ַ                     \r\n");
	printf("TestList                   %#x                      \r\n",(int)&TestList);
	printf("TestList->pxIndex         %#x                      \r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd         %#x                      \r\n",(int)(&TestList.xListEnd));
	printf("ListItem1                  %#x                      \r\n",(int)&ListItem1);
	printf("ListItem2                  %#x                      \r\n",(int)&ListItem2);
	printf("ListItem3                  %#x                      \r\n",(int)&ListItem3);
	printf("/**********************����*************************************/\r\n");
	printf("����KEY_UP��������\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES)
	{
		delay_ms(10);  //�ȴ�KEY_UP������
	}
	
	//�����������б�TestList����б���ListItem1,��ͨ�����ڴ�ӡ�����б����г�Ա����pxNext��pxPrevious��ֵ
	//ͨ��������ֵ�۲��б������б��е��������
	vListInsert(&TestList,&ListItem1);  //�����б���ListItem1
	printf("/*****************************����б���ListItem1*******************************/\r\n");
	printf("��Ŀ														��ַ										\r\n");
	printf("TestList->xListEnd->pxNext			%#x                   \r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext								%#x                   \r\n",(int)(ListItem1.pxNext));
	printf("/***************************ǰ�������ӷָ���*************************************/\r\n");
	printf("TestList->xListEnd->pxPrevious  	%#x                     \r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           	%#x                     \r\n",(int)(ListItem1.pxPrevious));
	printf("/*********************************����*****************************************************/\r\n\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES)
	{
		delay_ms(10);  //�ȴ�KEY_UP������
	}
	
	//���Ĳ������б�TestList����б���ListItem2,��ͨ�����ڴ�ӡ�����б����еĳ�Ա����pxNext��pxPrevious��ֵ
	//ͨ��������ֵ�۲��б������б��е��������
	vListInsert(&TestList,&ListItem2);  //�����б���ListItem2
	printf("/***********************************����б���ListItem2***************************/\r\n");
	printf("��Ŀ                              ��ַ				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************����**************************/\r\n");
	printf("����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP������
	
	//���岽�����б�TestList����б���ListItem3����ͨ�����ڴ�ӡ����
	//�б����г�Ա����pxNext��pxPrevious��ֵ��ͨ��������ֵ�۲��б�
	//�����б��е����������
	vListInsert(&TestList,&ListItem3);	//�����б���ListItem3
	printf("/******************����б���ListItem3*****************/\r\n");
	printf("��Ŀ                              ��ַ				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************����**************************/\r\n");
	printf("����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP������
	
	//��������ɾ��ListItem2����ͨ�����ڴ�ӡ�����б����г�Ա����pxNext��
	//pxPrevious��ֵ��ͨ��������ֵ�۲��б������б��е����������
	uxListRemove(&ListItem2);						//ɾ��ListItem2
	printf("/******************ɾ���б���ListItem2*****************/\r\n");
	printf("��Ŀ                              ��ַ				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************����**************************/\r\n");
	printf("����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP������
	
	
	//���߲������ListItem2����ͨ�����ڴ�ӡ�����б����г�Ա����pxNext��
	//pxPrevious��ֵ��ͨ��������ֵ�۲��б������б��е����������
	TestList.pxIndex=TestList.pxIndex->pxNext;			//pxIndex�����һ�����pxIndex�ͻ�ָ��ListItem1��
	vListInsertEnd(&TestList,&ListItem2);				//�б�ĩβ����б���ListItem2
	printf("/***************��ĩβ����б���ListItem2***************/\r\n");
	printf("��Ŀ                              ��ַ				    \r\n");
	printf("TestList->pxIndex                 %#x					\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************����**************************/\r\n\r\n\r\n");
	
	while(1)
	{
		LED1 = ~LED1;
		vTaskDelay(1000);
	}
}







