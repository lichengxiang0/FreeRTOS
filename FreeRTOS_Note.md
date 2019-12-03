# FreeRTOS学习笔记  

## test

## 1.官方网站  

https://www.freertos.org/  

用途：蓝牙协议、wifi协议栈基本都用FreeRTOS  

## 2.动态创建  

动态创建的优先级有两个不能用（范围0--31），一个是空闲任务的优先级0，另外一个是软件定时器的优先级31，其它优先级都可以用  

如果删除自己，可以使用 vTaskDelete(NULL)  

## 3.挂起和恢复  

相关API函数：

挂起：vTaskSuspend()  

恢复：vTaskResume()  

从中断中恢复：xTaskResumeFromISR()  



## 4.中断管理  

使用的API函数：

关闭中断：portDISABLE_INTERRUPTS()

打开中断：portENABLE_INTERRUPTS()

![qizhong ](image\three_representation_priorities.png)

其中STM32采用的是***四位表达优先级组***

![](image\grouping_relationship.png)

STM32只有5组分组：3、4、 5、 6、 7，其中FreeRTOS采用分组位置3，四位全是抢占优先级，没有亚优先级

| 分组位置 | FreeRTOS中断分组 |
| :------: | :--------------: |
|    3     |        4         |
|    4     |        3         |
|    5     |        2         |
|    6     |        1         |
|    7     |        0         |

总共是2^4=16个优先级，全是抢占优先级。在使用16个优先级时注意：FreeRTOS一般使用中断分组4

```
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY	15                      //中断最低优先级 

#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5                       //系统可管理的最高中断优先级 

#define configKERNEL_INTERRUPT_PRIORITY ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) 

#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) 
```



系统优先级配置时，0-4一般时不能调用API函数的，中断优先级5-15才是操作系统管理的范围 

关闭中断上面 configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 设置的优先级才会被关闭，0-4之间的优先级不会被关闭 



## 5.ARM架构 

片上系统：Soc 

ARM处理器架构进化史：

![](image\ARM_architecture.png)



Cortex-M3处理器拥有R0-R15的寄存器组。其中R13作为堆栈指针SP，SP有两个，但同一时刻只能有一个可以看到，这就是所谓的“banked”寄存器。

![寄存器](image\Cortex-M3_register.png)



特殊功能寄存器：

![](image\M3_register_team.png)

![](image\special_function_register_group.png)

![](image\program_status_register.png)

![](image\consolidated_program_status_register.png)

![](image\control_register.png)

![](image\shielding_register.png)

![](image\stack_pointer_R13.png)



存储器映射：总体来说，Cortex-M3支持4GB存储空间

![](image\M3_storage_mapping.png)



中断异常：

![](image\M3_abnormal_type1.png)

![](image\M3_abnormal_type2.png)



向量表：

![](image\vector_table1.png)

![](image\vector_table2.png)







