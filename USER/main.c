/*                                                             *
*                           STM32F407ZGT6                      *            
****************************************************************/
#include "includes.h"
//������
TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t KEYTask_Handler;
TaskHandle_t ADCask_Handler;
TaskHandle_t LCDask_Handler;
TaskHandle_t CPUask_Handler;

//������
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
		/* Ӳ����ʼ�� */
	bsp_Init(); 
		/* �������� */
	AppTaskCreate();
		/* ��������ͨ�Ż��� */
	  AppObjCreate();
	  /*�����������*/
   vTaskStartScheduler();          
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    taskENTER_CRITICAL();           //�����ٽ���

    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )512, 
                (void*          )NULL,				
                (UBaseType_t    )1,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //����KEY����
    xTaskCreate((TaskFunction_t )KEY_task,     
                (const char*    )"KEY_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )2,
                (TaskHandle_t*  )&KEYTask_Handler);        
								
								    //����ADC����
    xTaskCreate((TaskFunction_t )ADC_task,     
                (const char*    )"ADC_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )&ADCask_Handler);  
								
								    //����LCD����
    xTaskCreate((TaskFunction_t )LCD_task,     
                (const char*    )"LCD_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )4,
                (TaskHandle_t*  )&LCDask_Handler);  

										//����CPU����
    xTaskCreate((TaskFunction_t )CPU_task,     
                (const char*    )"CPU_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )&CPUask_Handler); 
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED0������ 
void led0_task(void *pvParameters)
{
	double sum=0;
    while(1)
    {
			sum = sinf(91.2)+cosf(22.3);
			printf("\n%lf\n",sum);
       vTaskDelay(1000);
    }
}   

//key������
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
    printf("������                         ����״̬           ���ȼ�            ʣ��ջ         �������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//��Ϣ����������
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("������                             ���м���                             ʹ����\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(1000);   /* ��ʱ500��tick */				
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	
	
}




