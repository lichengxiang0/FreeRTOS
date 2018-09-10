#include "led.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

//#include "timer.h"
//#include "key.h"

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


 //中断任务
TaskHandle_t INTERRUPTTask_Handler;
#define  INTERRUPT_STK_SIZE 128
#define INTERRUPT_TASK_PRIO 2
 

//函数声明
void start_task(void *pvParameters);
void interrupt_task(void *pvParamenters);


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
//    KEY_Init();                         //按键初始化
//    TIM3_Int_Init(10000-1,7200-1);
//    TIM5_Int_Init(10000-1,7200-1);
    
	xTaskCreate(	(TaskFunction_t 	)start_task,    				 //开始任务函数
								(const char *			) "start_task",					 //任务名称
								(uint16_t					)START_STK_SIZE  ,       //任务堆栈大小
								(void * 					)NULL,									 //传递给任务函数的参数
								(UBaseType_t 			)START_TASK_PRIO,				 //任务优先级
								(TaskHandle_t * 	) &StartTask_Handler );  //任务句柄
							
	 vTaskStartScheduler();          //开启任务调度
}


void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();  //进入临界区
    //创建中断测试任务
    xTaskCreate( (TaskFunction_t    )interrupt_task,
                 (const char *    )"interrupt_task",
                 (uint16_t         )INTERRUPT_STK_SIZE,
                 (void *           )NULL,
                 (UBaseType_t      )INTERRUPT_TASK_PRIO,
                 (TaskHandle_t *)&INTERRUPTTask_Handler
                                                    );
   vTaskDelete(StartTask_Handler);
   taskEXIT_CRITICAL();  //退出临界区                   
    
}

void interrupt_task(void *pvParamenters)
{
        uint8_t key = 0;
        while(1)
        {
            key ++;
            if( key == 5 )
            {
                printf("关闭中断");
                portDISABLE_INTERRUPTS();				//关闭中断
                delay_xms(5000);
                
                printf("打开中断.............\r\n");	//打开中断
                portENABLE_INTERRUPTS();
                
            }
            
//            key=KEY_Scan(0);
//            switch(key)
//            {
//                case KEY0_PRES:
//                      
//                        break;
//                case KEY1_PRES:
//                    printf("打开中断.............\r\n");	//打开中断
//                      portENABLE_INTERRUPTS();   //打开中断
//                        break;
//                case WKUP_PRES:
//                       printf("关闭中断.............\r\n");
//                      portDISABLE_INTERRUPTS();  //关闭中断 
//                       break;
//                default:
//                    break;
//            }
            
            
            LED0 = ~LED0;
            vTaskDelay(1000);
        }
}





