#include "sys.h"
#include "delay.h"
#include "includes.h"

static GPIO_InitTypeDef		GPIO_InitStructure;


void dht11_init(void)
{
	//ʹ�ܶ˿�G��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	

	
	//����PG9Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOG,&GPIO_InitStructure);	


	//PG9��ʼ��ƽΪ�ߵ�ƽ
	PGout(9)=1;
}


static void dht11_pin_mode(GPIOMode_TypeDef gpio_mode)
{
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9������
	GPIO_InitStructure.GPIO_Mode=gpio_mode;			//���/����ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOG,&GPIO_InitStructure);	

}



int32_t dht11_read(uint8_t *pdht_data)
{
	uint32_t t=0;
	
	uint8_t d;
	
	int32_t i=0;
	int32_t j=0;
	uint32_t check_sum=0;
	
	//��֤����Ϊ���ģʽ
	dht11_pin_mode(GPIO_Mode_OUT);	
	
	PGout(9)=0;
	
	delay_ms(20);
	
	PGout(9)=1;	
	
	delay_us(30);
	
	//��֤����Ϊ����ģʽ
	dht11_pin_mode(GPIO_Mode_IN);

	
	//�ȴ�DHT11��Ӧ���ȴ��͵�ƽ����
	t=0;
	while(PGin(9))
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 4000)
			return -1;
	}
	
	//���͵�ƽ����100us
	t=0;
	while(PGin(9)==0)
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 100)
			return -2;
	}
	
	
	//���ߵ�ƽ����100us
	t=0;
	while(PGin(9))
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 100)
			return -3;
	}
	
	//��������5���ֽ�
	for(j=0; j<5; j++)
	{
		d = 0;
		//���8��bit���ݵĽ��գ���λ����
		for(i=7; i>=0; i--)
		{
			//�ȴ��͵�ƽ�������
			t=0;
			while(PGin(9)==0)
			{
			
				t++;
				
				delay_us(1);
				
				if(t >= 100)
					return -4;
			}	
			
			
			delay_us(40);
			
			if(PGin(9))
			{
				d|=1<<i;
				
				//�ȴ�����1�ĸߵ�ƽʱ��������
				t=0;
				while(PGin(9))
				{
				
					t++;
					
					delay_us(1);
					
					if(t >= 100)
						return -5;
				}			
			
			}
		}	
	
		pdht_data[j] = d;
	}
	
	
	//ͨ�ŵĽ���
	delay_us(100);
	
	//����У���
	check_sum=pdht_data[0]+pdht_data[1]+pdht_data[2]+pdht_data[3];
	
	
	
	check_sum = check_sum & 0xFF;
	
	if(check_sum != pdht_data[4])
		return -6;
	
	return 0;
}
