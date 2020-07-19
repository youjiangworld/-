#include "bldc.h"
#include "tim.h"

#define MOTOR_DIR_CW               1    // �������: ˳ʱ��
#define MOTOR_DIR_CCW              (-1) // �������: ��ʱ��



__IO uint32_t Lock_Time = 0;  // ��תʱ��

__IO int32_t uwStep = 0;      // 6������״̬
__IO int32_t Dir = MOTOR_DIR_CCW;       // ��ת����
__IO float PWM_Duty = 0.15f;  // ռ�ձ�15%

void TheFirstSequence(void);
void CCW_Sequence(int32_t  HALL_Step);
void CW_Sequence(int32_t  HALL_Step);


/**
  * ��������: ʹ����ˢ���ת��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void Enable_BLDC(void)
{
	 /* ʹ�ܻ����������ӿ� �� PWM������� */
  HAL_TIMEx_HallSensor_Start(&htim3); 
  TheFirstSequence();
  HAL_TIM_GenerateEvent(&htim1,TIM_EVENTSOURCE_COM);
  __HAL_TIM_CLEAR_FLAG(&htim1,TIM_FLAG_COM);
  HAL_TIMEx_CommutationCallback(&htim1);

}

/**
  * ��������: ֹͣ���ת��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void Disable_BLDC(void)
{
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
  
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
  
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
  HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
  
  HAL_TIMEx_HallSensor_Stop(&htim3); 
}


uint32_t HallSensor_GetPinState(void)
{
  __IO static uint32_t State ;
  State  = 0;
  /*����������״̬��ȡ*/
  if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6) != GPIO_PIN_RESET)  
  {
    State |= 0x01U;
  }
  if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7) != GPIO_PIN_RESET) 
  {
    State |= 0x02U;
  }
  if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8) != GPIO_PIN_RESET) 
  {
    State |= 0x04U;
  }
  return State;
}

void TheFirstSequence()
{
	 /* ��ȡ��������������״̬,��Ϊ��������� */
  __IO uint32_t tmp = 0;
  uwStep = HallSensor_GetPinState();
  if(Dir == MOTOR_DIR_CW)
  {
    uwStep = (uint32_t)7 - uwStep; // ����˳���Ĺ��� CW = 7 - CCW;
  }
  /*---- ���嶨ʱ��OC1ΪA(U)�� OC2ΪB(V)�࣬OC3ΪC(W)�� ---- */
  /*---- ����uWStep��3λΪ��������������״̬,IC1(001,U),IC2(010,V),IC3(100,W) ----*/
  switch(uwStep)
  {
    case 1://C+ A-
      /*  Channel2 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); 

      /*  Channe1l configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
      break;
    
    case 2: //A+  B-
      /*  Channel3 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
    
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
      
      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
      break;
    
    case 3:// C+ B-
      /*  Channel1 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
 
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  
      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
      break;
    
    case 4:// B+ C-
      /*  Channel1 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
      
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD +1);    
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
      break;
    
    case 5: // B+ A-
      /*  Channel3 configuration  */       
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
    
      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
      
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
      break;
    
    case 6: // A+ C-
      /*  Channel2 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); 
      
      /*  Channe3l configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD +1);  
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
      break;
  }
  Lock_Time = 0;


}


/* ˳ʱ��˳���*/
void CW_Sequence( int32_t  HALL_Step)
{
  /*---- ���嶨ʱ��OC1ΪA(U)�� OC2ΪB(V)�࣬OC3ΪC(W)�� ---- */
  /*---- ����uWStep��3λΪ��������������״̬,IC1(001,U),IC2(010,V),IC3(100,W) ----*/
	switch(HALL_Step)
  {
		    case 1:// A+ B- ������һ����ͨ������
      /*  Channel3 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
 
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  
      /*  Channe2 configuration   */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
      break;
    
    case 2: // ��һ���� B+ C-
      /*  Channel1 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    
      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); 
      
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD +1);  
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
      break;
    
    case 3: // ��һ����  A+  C-
      /*  Channel2 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
      
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
      break;
    
    case 4: // ��һ���� C+ A-
      /*  Channel2 configuration  */       
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
      
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
      break;
    
    case 5:// ��һ���� C+ B-
      /*  Channel1 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); 

      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
      break;
    
    case 6:// ��һ���� B+ A-
      /*  Channel3 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);

      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
      
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD +1);    
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
      break;
	
	
	
	}


}





/* ��ʱ��˳���*/
void CCW_Sequence( int32_t  HALL_Step)
{
  /*---- ���嶨ʱ��OC1ΪA(U)�� OC2ΪB(V)�࣬OC3ΪC(W)�� ---- */
  /*---- ����uWStep��3λΪ��������������״̬,IC1(001,U),IC2(010,V),IC3(100,W) ----*/
    switch(HALL_Step)
  {
    case 1:// C+ B- ������һ����ͨ������
      /*  Channel1 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
 
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  
      /*  Channe2 configuration   */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
      break;
    
    case 2: // ��һ���� A+ C-
      /*  Channel2 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); 
      
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD +1);  
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
      break;
    
    case 3: // ��һ����  A+  B-
      /*  Channel3 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
    
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
      
      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
      break;
    
    case 4: // ��һ���� B+ A-
      /*  Channel3 configuration  */       
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
    
      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
      
      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
      break;
    
    case 5:// ��һ���� C+ A-
      /*  Channel2 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); 

      /*  Channel1 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,BLDCMOTOR_TIM_PERIOD +1);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
      break;
    
    case 6:// ��һ���� B+ C-
      /*  Channel1 configuration  */ 
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

      /*  Channel2 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,BLDCMOTOR_TIM_PERIOD * PWM_Duty);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
      
      /*  Channel3 configuration  */
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,BLDCMOTOR_TIM_PERIOD +1);    
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
      break;
  } 
}


void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim)
{
  /* ��ȡ��������������״̬,��Ϊ��������� */
  __IO uint32_t tmp = 0;
  uwStep = HallSensor_GetPinState();
  if(Dir == MOTOR_DIR_CW)
  {
    CW_Sequence( uwStep );
  }
  else
  {
    CCW_Sequence( uwStep );
  }
  Lock_Time = 0;
}
/******************* (C) COPYRIGHT 2020 COETIC *****END OF FILE****/
