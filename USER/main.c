#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timer.h"
#include "semphr.h"
#include "key.h"
#include "lcd.h"
#include "adc.h"
#include "string.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 FreeRTOS实验2-1
 FreeRTOS移植实验-库函数版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		1024  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		128  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define KEY_TASK_PRIO		3
//任务堆栈大小	
#define KEY_STK_SIZE 		128  
//任务句柄
TaskHandle_t KEYTask_Handler;
//任务函数
void KEY_task(void *pvParameters);

//任务优先级
#define ADC_TASK_PRIO		2
//任务堆栈大小	
#define ADC_STK_SIZE 		512  
//任务句柄
TaskHandle_t ADCask_Handler;
//任务函数
void ADC_task(void *pvParameters);

//任务优先级
#define LCD_TASK_PRIO		5
//任务堆栈大小	
#define LCD_STK_SIZE 		512  
//任务句柄
TaskHandle_t LCDask_Handler;
//任务函数
void LCD_task(void *pvParameters);

//任务优先级
#define CPU_TASK_PRIO		6
//任务堆栈大小	
#define CPU_STK_SIZE 		512  
//任务句柄
TaskHandle_t CPUask_Handler;
//任务函数
void CPU_task(void *pvParameters);

//按键消息队列的数量
#define MESSAGE_Q_NUM    1  		//按键消息队列的数量  
QueueHandle_t  Message_Queue;	//信息队列句柄

//二值信号量句柄
SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	KEY_Init();
	
	LCD_Init();         //初始化LCD接口
	Adc_Init();         //初始化ADC
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,130,200,16,16,"ADC1_CH5_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH5_VOL:0.000V");	//先在固定位置显示小数点      
	TIM3_Int_Init(50-1,84-1);
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}
u8 flag = 0;
//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
	 Message_Queue=xQueueCreate(MESSAGE_Q_NUM,4); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	 if(Message_Queue!=NULL)
		 flag = 1;
	 	//创建二值信号量
	BinarySemaphore=xSemaphoreCreateBinary();	
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建KEY任务
    xTaskCreate((TaskFunction_t )KEY_task,     
                (const char*    )"KEY_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);        
								
								    //创建ADC任务
    xTaskCreate((TaskFunction_t )ADC_task,     
                (const char*    )"ADC_task",   
                (uint16_t       )ADC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )ADC_TASK_PRIO,
                (TaskHandle_t*  )&ADCask_Handler);  
								
								    //创建LCD任务
    xTaskCreate((TaskFunction_t )LCD_task,     
                (const char*    )"LCD_task",   
                (uint16_t       )LCD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LCD_TASK_PRIO,
                (TaskHandle_t*  )&LCDask_Handler);  

										//创建CPU任务
    xTaskCreate((TaskFunction_t )CPU_task,     
                (const char*    )"CPU_task",   
                (uint16_t       )CPU_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CPU_TASK_PRIO,
                (TaskHandle_t*  )&CPUask_Handler); 
								
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
			 if(xSemaphoreTake(BinarySemaphore,portMAX_DELAY))
			 { 
        LED0=~LED0;
			 }
       vTaskDelay(10);
    }
}   

//LED1任务函数
void KEY_task(void *pvParameters)
{
	u8 key ;

    while(1)
    {
		   		key=KEY_Scan(0);
          if(BinarySemaphore!=NULL&&key==1)
					{
							xSemaphoreGive(BinarySemaphore);
					}
        vTaskDelay(10);
    }
}
	 	u16 adcx;
	float temp;
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
    printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//信息缓冲区清零
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("任务名       运行计数         使用率\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(1000);   /* 延时500个tick */		
		
	}
	
	
	
}





