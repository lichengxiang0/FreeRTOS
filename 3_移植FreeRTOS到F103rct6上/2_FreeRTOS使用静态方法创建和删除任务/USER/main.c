#include "led.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
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

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED

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
	int num = 0;
	while(1)
	{
		num++;
		LED0 = ~LED0;
		vTaskDelay(2000);  //ms
		if (num > 3)
		{
			if ( Task2Task_Handler!= NULL )  //任务2是否存在
			{
				vTaskDelete(Task2Task_Handler);  //任务1执行3次删除任务2
				Task2Task_Handler = NULL;  //任务句柄清零
			}
		}
	}
}

//task2任务函数
void task2_task(void *pvParameters)
{
	while(1)
	{
		LED1 = ~LED1;
		vTaskDelay(500);
	}
}







