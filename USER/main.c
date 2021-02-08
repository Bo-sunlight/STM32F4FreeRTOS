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
 ALIENTEK ̽����STM32F407������ FreeRTOSʵ��2-1
 FreeRTOS��ֲʵ��-�⺯���汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		1024  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		128  
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

//�������ȼ�
#define KEY_TASK_PRIO		3
//�����ջ��С	
#define KEY_STK_SIZE 		128  
//������
TaskHandle_t KEYTask_Handler;
//������
void KEY_task(void *pvParameters);

//�������ȼ�
#define ADC_TASK_PRIO		2
//�����ջ��С	
#define ADC_STK_SIZE 		512  
//������
TaskHandle_t ADCask_Handler;
//������
void ADC_task(void *pvParameters);

//�������ȼ�
#define LCD_TASK_PRIO		5
//�����ջ��С	
#define LCD_STK_SIZE 		512  
//������
TaskHandle_t LCDask_Handler;
//������
void LCD_task(void *pvParameters);

//�������ȼ�
#define CPU_TASK_PRIO		6
//�����ջ��С	
#define CPU_STK_SIZE 		512  
//������
TaskHandle_t CPUask_Handler;
//������
void CPU_task(void *pvParameters);

//������Ϣ���е�����
#define MESSAGE_Q_NUM    1  		//������Ϣ���е�����  
QueueHandle_t  Message_Queue;	//��Ϣ���о��

//��ֵ�ź������
SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	uart_init(115200);     	//��ʼ������
	LED_Init();		        //��ʼ��LED�˿�
	KEY_Init();
	
	LCD_Init();         //��ʼ��LCD�ӿ�
	Adc_Init();         //��ʼ��ADC
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(30,130,200,16,16,"ADC1_CH5_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH5_VOL:0.000V");	//���ڹ̶�λ����ʾС����      
	TIM3_Int_Init(50-1,84-1);
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}
u8 flag = 0;
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
	 Message_Queue=xQueueCreate(MESSAGE_Q_NUM,4); //������ϢMessage_Queue,��������Ǵ��ڽ��ջ���������
	 if(Message_Queue!=NULL)
		 flag = 1;
	 	//������ֵ�ź���
	BinarySemaphore=xSemaphoreCreateBinary();	
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //����KEY����
    xTaskCreate((TaskFunction_t )KEY_task,     
                (const char*    )"KEY_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);        
								
								    //����ADC����
    xTaskCreate((TaskFunction_t )ADC_task,     
                (const char*    )"ADC_task",   
                (uint16_t       )ADC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )ADC_TASK_PRIO,
                (TaskHandle_t*  )&ADCask_Handler);  
								
								    //����LCD����
    xTaskCreate((TaskFunction_t )LCD_task,     
                (const char*    )"LCD_task",   
                (uint16_t       )LCD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LCD_TASK_PRIO,
                (TaskHandle_t*  )&LCDask_Handler);  

										//����CPU����
    xTaskCreate((TaskFunction_t )CPU_task,     
                (const char*    )"CPU_task",   
                (uint16_t       )CPU_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CPU_TASK_PRIO,
                (TaskHandle_t*  )&CPUask_Handler); 
								
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED0������ 
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

//LED1������
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
			LCD_ShowxNum(134,130,adcx,4,16,0);    //��ʾADCC�������ԭʼֵ
			LCD_ShowxNum(134,150,adcx,1,16,0);    //��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3
			LCD_ShowxNum(150,150,temp,3,16,0X80); //��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111.
		 vTaskDelay(400);
	}
	
}

void ADC_task(void *pvParameters)
{
	while(1)
	{
		adcx=Get_Adc_Average(ADC_Channel_5,20);//��ȡͨ��5��ת��ֵ��20��ȡƽ��
		temp=(float)adcx*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
		adcx=temp;                            //��ֵ�������ָ�adcx��������ΪadcxΪu16����
		temp-=adcx;                           //���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111
		temp*=1000;                           //С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС����
		 vTaskDelay(100);
	}
	
}
void CPU_task(void*pvParameters)
{
	uint8_t CPU_RunInfo[400]; //������������ʱ����Ϣ
	while(1)
	{
		memset(CPU_RunInfo,0,400); //��Ϣ����������
		 vTaskList((char *)&CPU_RunInfo);  //��ȡ��������ʱ����Ϣ
    
    printf("---------------------------------------------\r\n");
    printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//��Ϣ����������
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("������       ���м���         ʹ����\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(1000);   /* ��ʱ500��tick */		
		
	}
	
	
	
}





