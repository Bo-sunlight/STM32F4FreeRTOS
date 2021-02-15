/*                                                             *
*                           STM32F407ZGT6                      *            
****************************************************************/
#include "includes.h"
//任务句柄
TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t KEYTask_Handler;
TaskHandle_t ADCask_Handler;
TaskHandle_t LCDask_Handler;
TaskHandle_t CPUask_Handler;
EventGroupHandle_t xCreatedEventGroup = NULL;

//任务函数
void start_task(void *pvParameters);
void led0_task(void *pvParameters);
void KEY_task(void *pvParameters);
void ADC_task(void *pvParameters);
void LCD_task(void *pvParameters);
void CPU_task(void *pvParameters);

static void AppTaskCreate (void);
static void AppObjCreate (void);

u16 adcx;
float temp;

int main(void)
{ 
		__set_PRIMASK(1);  
	
		/* 硬件初始化 */
	bsp_Init(); 
	
		/* 创建任务 */
	AppTaskCreate();
	
		/* 创建任务通信机制 */
	  AppObjCreate();
	
 /* 启动调度，开始执行任务 */
   vTaskStartScheduler();
/* 
 如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或者空闲任务的
 heap 空间不足造成创建失败，此要加大 FreeRTOSConfig.h 文件中定义的 heap 大小：
 #define configTOTAL_HEAP_SIZE ( ( size_t ) ( 17 * 1024 ) )
*/
   while(1);      
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    taskENTER_CRITICAL();           //进入临界区

    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )512, 
                (void*          )NULL,				
                (UBaseType_t    )1,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建KEY任务
    xTaskCreate((TaskFunction_t )KEY_task,     
                (const char*    )"KEY_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )2,
                (TaskHandle_t*  )&KEYTask_Handler);        
								
								    //创建ADC任务
    xTaskCreate((TaskFunction_t )ADC_task,     
                (const char*    )"ADC_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )&ADCask_Handler);  
								
								    //创建LCD任务
    xTaskCreate((TaskFunction_t )LCD_task,     
                (const char*    )"LCD_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )4,
                (TaskHandle_t*  )&LCDask_Handler);  

										//创建CPU任务
    xTaskCreate((TaskFunction_t )CPU_task,     
                (const char*    )"CPU_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )&CPUask_Handler); 
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void led0_task(void *pvParameters)
{
		EventBits_t uxBits;
	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* 最大延迟100ms */
    while(1)
    {
			 	uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
							          (1 << 0) ,            /* 等待bit0和bit1被设置 */
							         pdTRUE,             /* 退出前bit0和bit1被清除 */
							         pdTRUE,             /* 设置为pdTRUE表示等待bit1和bit0都被设置*/
							         xTicksToWait); 	 /* 等待延迟时间 */
		
		if((uxBits &  (1 << 0) ) ==  (1 << 0) )
		{
			/* 接收到bit1和bit0都被设置的消息 */
			printf("接收到bit0和bit1都被设置的消息\r\n");
		}
    }
}   

//key任务函数
void KEY_task(void *pvParameters)
{
	  u8 key ;
    while(1)
    {
		   	key=KEY_Scan(0);
        vTaskDelay(10);
    }
}


void LCD_task(void *pvParameters)
{
	while(1)
	{
			LCD_ShowxNum(134,130,adcx,4,16,0);    //显示ADCC采样后的原始值
			LCD_ShowxNum(134,150,adcx,1,16,0);    //显示电压值的整数部分，3.1111的话，这里就是显示3
			LCD_ShowxNum(150,150,temp,3,16,0X80); //显示小数部分（前面转换为了整形显示），这里显示的就是111.
		  vTaskDelay(400);
	}
}

void ADC_task(void *pvParameters)
{
	while(1)
	{
		adcx=Get_Adc_Average(ADC_Channel_5,20);//获取通道5的转换值，20次取平均
		temp=(float)adcx*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
		adcx=temp;                            //赋值整数部分给adcx变量，因为adcx为u16整形
		temp-=adcx;                           //把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111
		temp*=1000;                           //小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。
		 vTaskDelay(100);
	}
}

void CPU_task(void*pvParameters)
{
	uint8_t CPU_RunInfo[400]; //保存任务运行时间信息
	while(1)
	{
		memset(CPU_RunInfo,0,400); //信息缓冲区清零
		 vTaskList((char *)&CPU_RunInfo);  //获取任务运行时间信息
    
    printf("---------------------------------------------\r\n");
    printf("任务名                         任务状态           优先级            剩余栈         任务序号\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//信息缓冲区清零
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("任务名                             运行计数                             使用率\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(1000);   /* 延时500个tick */				
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
		/* 创建事件标志组 */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
	
}




