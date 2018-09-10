#include "led.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

//#include "timer.h"
//#include "key.h"

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


 //�ж�����
TaskHandle_t INTERRUPTTask_Handler;
#define  INTERRUPT_STK_SIZE 128
#define INTERRUPT_TASK_PRIO 2
 

//��������
void start_task(void *pvParameters);
void interrupt_task(void *pvParamenters);


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
//    KEY_Init();                         //������ʼ��
//    TIM3_Int_Init(10000-1,7200-1);
//    TIM5_Int_Init(10000-1,7200-1);
    
	xTaskCreate(	(TaskFunction_t 	)start_task,    				 //��ʼ������
								(const char *			) "start_task",					 //��������
								(uint16_t					)START_STK_SIZE  ,       //�����ջ��С
								(void * 					)NULL,									 //���ݸ��������Ĳ���
								(UBaseType_t 			)START_TASK_PRIO,				 //�������ȼ�
								(TaskHandle_t * 	) &StartTask_Handler );  //������
							
	 vTaskStartScheduler();          //�����������
}


void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();  //�����ٽ���
    //�����жϲ�������
    xTaskCreate( (TaskFunction_t    )interrupt_task,
                 (const char *    )"interrupt_task",
                 (uint16_t         )INTERRUPT_STK_SIZE,
                 (void *           )NULL,
                 (UBaseType_t      )INTERRUPT_TASK_PRIO,
                 (TaskHandle_t *)&INTERRUPTTask_Handler
                                                    );
   vTaskDelete(StartTask_Handler);
   taskEXIT_CRITICAL();  //�˳��ٽ���                   
    
}

void interrupt_task(void *pvParamenters)
{
        uint8_t key = 0;
        while(1)
        {
            key ++;
            if( key == 5 )
            {
                printf("�ر��ж�");
                portDISABLE_INTERRUPTS();				//�ر��ж�
                delay_xms(5000);
                
                printf("���ж�.............\r\n");	//���ж�
                portENABLE_INTERRUPTS();
                
            }
            
//            key=KEY_Scan(0);
//            switch(key)
//            {
//                case KEY0_PRES:
//                      
//                        break;
//                case KEY1_PRES:
//                    printf("���ж�.............\r\n");	//���ж�
//                      portENABLE_INTERRUPTS();   //���ж�
//                        break;
//                case WKUP_PRES:
//                       printf("�ر��ж�.............\r\n");
//                      portDISABLE_INTERRUPTS();  //�ر��ж� 
//                       break;
//                default:
//                    break;
//            }
            
            
            LED0 = ~LED0;
            vTaskDelay(1000);
        }
}





