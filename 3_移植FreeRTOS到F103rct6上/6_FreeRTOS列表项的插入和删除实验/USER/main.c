#include "led.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

#include "key.h"

/************************************************
 ALIENTEK Mini STM32F103开发板 FreeRTOS实验2-1
 FreeRTOS移植实验-库函数版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

/********************************************
**使用静态方法创建任务 
**第一步：在FreeRTOSConfig.h中添加定义  #define configSUPPORT_STATIC_ALLOCATION          1            //使能静态创建方法  
**第二步：自定义vApplicationGetIdleTaskMemory和vApplicationGetTimerTaskMemory函数
**第三步：创建任务
****
***********************************************/

//开始任务
TaskHandle_t StartTask_Handler;
#define START_STK_SIZE 		128
#define START_TASK_PRIO		1
StackType_t StartTaskStack[START_STK_SIZE];
StaticTask_t StartTaskTCB;

//创建TASK1任务
TaskHandle_t Task1Task_Handler;
#define TASK1_STK_SIZE 		128
#define TASK1_TASK_PRIO		1
StackType_t Task1TaskStack[START_STK_SIZE];
StaticTask_t Task1TaskTCB;

//创建TASK2任务
TaskHandle_t Task2Task_Handler;
#define TASK2_STK_SIZE 		128
#define TASK2_TASK_PRIO		1
StackType_t Task2TaskStack[START_STK_SIZE];
StaticTask_t Task2TaskTCB;

//空闲任务所需要的变量
static StaticTask_t IdleTaskTCB;  //控制块
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];  //控制堆栈

//定时器任务所需要的变量
static StaticTask_t TimerTaskTCB;
static StackType_t  TimerTaskStack[configTIMER_TASK_STACK_DEPTH];


//定时器任务所需内存
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &TimerTaskTCB;
	*ppxTimerTaskStackBuffer = TimerTaskStack;
	*pulTimerTaskStackSize = configMINIMAL_STACK_SIZE;  //软件定时器任务堆栈大小
}


//空闲任务所需内存
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, 
									StackType_t **ppxIdleTaskStackBuffer,
									uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

//函数声明
void start_task(void *pvParameters);
void task1_task(void *pvParameters);
void task2_task(void *pvParameters);

//定义一个测试用的列表和列表项
List_t TestList;   //测试用列表
ListItem_t ListItem1;  //测试用列表项1
ListItem_t ListItem2;  //测试用列表项2
ListItem_t ListItem3;  //测试用列表项3


/*************************************
*测试说明：start_task是主任务，用于创建任务
*          task1_task点亮一个小灯，用于显示任务运行
*          task2_task用于列表和列表的实验
*
**************************************/

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	KEY_Init();               //初始化按键
	
	
	StartTask_Handler = xTaskCreateStatic(	(TaskFunction_t		) start_task,    	//任务函数
											(const char *		)  "start_task",	//任务名称
											(uint32_t			) START_STK_SIZE,	//任务堆栈大小
											(void * 			)NULL,				//传递给任务函数的参数
											(UBaseType_t		) START_TASK_PRIO,	//任务优先级
											(StackType_t *		) StartTaskStack ,	//任务堆栈
											(StaticTask_t *		) &StartTaskTCB );	//任务控制块
							
	 vTaskStartScheduler();          //开启任务调度

	
	
		//用系统写不会用到while循环
//	while(1)
//	{
//	
//	}		
	
//    vTaskStartScheduler();          //开启任务调度
}


void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区
	//创建TASK1任务
	Task1Task_Handler=xTaskCreateStatic((TaskFunction_t	)task1_task,
										(const char* 	)"task1_task",
										(uint32_t 		)TASK1_STK_SIZE,
										(void* 		  	)NULL,
										(UBaseType_t 	)TASK1_TASK_PRIO,
										(StackType_t*   )Task1TaskStack,
										(StaticTask_t*  )&Task1TaskTCB);
	//创建TASK2任务
	Task2Task_Handler=xTaskCreateStatic((TaskFunction_t	)task2_task,
										(const char* 	)"task2_task",
										(uint32_t 		)TASK2_STK_SIZE,
										(void* 		  	)NULL,
										(UBaseType_t 	)TASK2_TASK_PRIO,
										(StackType_t*   )Task2TaskStack,
										(StaticTask_t*  )&Task2TaskTCB);
	vTaskDelete(StartTask_Handler); //删除开始任务
	taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	while(1)
	{
		LED0 = ~LED0;
		vTaskDelay(500);  //ms
	}
}

//task2任务函数
//用于列表和列表项的实验
void task2_task(void *pvParameters)
{
	//第一步：初始化列表和列表项
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	ListItem1.xItemValue = 40;  //ListItem1列表项值为40
	ListItem2.xItemValue = 60;  //ListItem2列表项值为60
	ListItem3.xItemValue = 50;  //ListItem3列表项值为50 
	
	//第二步：打印列表和其它列表项的地址
	printf("/******************列表和列表项地址**************************/\r\n");
	printf("项目                       地址                     \r\n");
	printf("TestList                   %#x                      \r\n",(int)&TestList);
	printf("TestList->pxIndex         %#x                      \r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd         %#x                      \r\n",(int)(&TestList.xListEnd));
	printf("ListItem1                  %#x                      \r\n",(int)&ListItem1);
	printf("ListItem2                  %#x                      \r\n",(int)&ListItem2);
	printf("ListItem3                  %#x                      \r\n",(int)&ListItem3);
	printf("/**********************结束*************************************/\r\n");
	printf("按下KEY_UP键继续！\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES)
	{
		delay_ms(10);  //等待KEY_UP键按下
	}
	
	//第三步：向列表TestList添加列表项ListItem1,并通过串口打印所有列表项中成员变量pxNext和pxPrevious的值
	//通过这两个值观察列表项在列表中的连接情况
	vListInsert(&TestList,&ListItem1);  //插入列表项ListItem1
	printf("/*****************************添加列表项ListItem1*******************************/\r\n");
	printf("项目														地址										\r\n");
	printf("TestList->xListEnd->pxNext			%#x                   \r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext								%#x                   \r\n",(int)(ListItem1.pxNext));
	printf("/***************************前后向连接分割线*************************************/\r\n");
	printf("TestList->xListEnd->pxPrevious  	%#x                     \r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           	%#x                     \r\n",(int)(ListItem1.pxPrevious));
	printf("/*********************************结束*****************************************************/\r\n\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES)
	{
		delay_ms(10);  //等待KEY_UP键按下
	}
	
	//第四步：向列表TestList添加列表项ListItem2,并通过串口打印所有列表项中的成员变量pxNext和pxPrevious的值
	//通过这两个值观察列表项在列表中的连接情况
	vListInsert(&TestList,&ListItem2);  //插入列表项ListItem2
	printf("/***********************************添加列表项ListItem2***************************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第五步：向列表TestList添加列表项ListItem3，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem3);	//插入列表项ListItem3
	printf("/******************添加列表项ListItem3*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第六步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	uxListRemove(&ListItem2);						//删除ListItem2
	printf("/******************删除列表项ListItem2*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	
	//第七步：添加ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	TestList.pxIndex=TestList.pxIndex->pxNext;			//pxIndex向后移一项，这样pxIndex就会指向ListItem1。
	vListInsertEnd(&TestList,&ListItem2);				//列表末尾添加列表项ListItem2
	printf("/***************在末尾添加列表项ListItem2***************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->pxIndex                 %#x					\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************结束**************************/\r\n\r\n\r\n");
	
	while(1)
	{
		LED1 = ~LED1;
		vTaskDelay(1000);
	}
}







