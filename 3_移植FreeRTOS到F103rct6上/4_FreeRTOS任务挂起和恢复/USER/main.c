#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
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




//开始任务
TaskHandle_t StartTask_Handler;
#define START_STK_SIZE   128
#define START_TASK_PRIO  1

//TASK1的声明
TaskHandle_t Task1Task_Handler;
#define TASK1_STK_SIZE 128
#define TASK1_TASK_PRIO 2

//TASK2的声明
TaskHandle_t Task2Task_Handler;
#define TASK2_STK_SIZE 128
#define TASK2_TASK_PRIO 3
 
//TASK3的声明
TaskHandle_t Task3Task_Handler;
#define TASK3_STK_SIZE 128
#define TASK3_TASK_PRIO 4

//函数声明
void start_task(void *pvParameters);
void task1_task(void *pvParameters);
void task2_task(void *pvParameters);
void task3_task(void *pvParameters);


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
    KEY_Init();                         //按键初始化
    EXTIX_Init();						//初始化外部中断
    
	xTaskCreate(	(TaskFunction_t 	)start_task,    				 //开始任务函数
								(const char *			) "start_task",					 //任务名称
								(uint16_t					)START_STK_SIZE  ,       //任务堆栈大小
								(void * 					)NULL,									 //传递给任务函数的参数
								(UBaseType_t 			)START_TASK_PRIO,				 //任务优先级
								(TaskHandle_t * 	) &StartTask_Handler );  //任务句柄
							
	 vTaskStartScheduler();          //开启任务调度
}

//开始任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区
	//创建TASK1任务
	xTaskCreate(	(TaskFunction_t 	)task1_task,    				 //开始任务函数
								(const char *			) "task1_task",					 //任务名称
								(uint16_t					)TASK1_STK_SIZE  ,       //任务堆栈大小
								(void * 					)NULL,									 //传递给任务函数的参数
								(UBaseType_t 			)TASK1_TASK_PRIO,				 //任务优先级
								(TaskHandle_t * 	) &Task1Task_Handler );  //任务句柄
								
	//创建TASK2任务
	xTaskCreate(	(TaskFunction_t 	)task2_task,    				 //开始任务函数
								(const char *			) "task2_task",					 //任务名称
								(uint16_t					)TASK2_STK_SIZE  ,       //任务堆栈大小
								(void * 					)NULL,									 //传递给任务函数的参数
								(UBaseType_t 			)TASK2_TASK_PRIO,				 //任务优先级
								(TaskHandle_t * 	) &Task2Task_Handler );  //任务句柄
   //创建TASK3任务
	xTaskCreate(	(TaskFunction_t 	)task3_task,    				 //开始任务函数
								(const char *			) "task3_task",					 //任务名称
								(uint16_t					)TASK3_STK_SIZE  ,       //任务堆栈大小
								(void * 					)NULL,									 //传递给任务函数的参数
								(UBaseType_t 			)TASK3_TASK_PRIO,				 //任务优先级
								(TaskHandle_t * 	) &Task3Task_Handler );  //任务句柄
                                
	vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	uint8_t key = 0;
	while(1)
	{
        key = KEY_Scan(0);
        switch(key)
        {
            case KEY0_PRES:
                
                    break;
            case KEY1_PRES:
                    vTaskSuspend(Task2Task_Handler);  //挂起任务 2
                    printf("挂起任务2的运行!\r\n");
                    break;
            case WKUP_PRES:
                     vTaskResume(Task2Task_Handler);  //恢复任务2
                       printf("恢复任务2的运行!\r\n");
                   break;
            default:
                break;
        }
        

	}
}

//task2任务函数
void task2_task(void *pvParameters)
{
	while(1)
	{
		LED0 = ~LED0;
		vTaskDelay(500);
	}

}

//task3任务函数
void task3_task(void *pvParameters)
{
    while(1)
    {
        LED1 = ~LED1;
        vTaskDelay(500);
    }
}




