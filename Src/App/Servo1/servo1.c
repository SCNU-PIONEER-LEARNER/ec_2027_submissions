#include "servo1.h"
#include "main.h" 
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
extern TIM_HandleTypeDef htim2;


static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) { Error_Handler(); }
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) { Error_Handler(); }
  
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) { Error_Handler(); }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) { Error_Handler(); }
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500; 
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) { Error_Handler(); }

 
}

void Servo_Task_Init(void) {
    MX_TIM2_Init();
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500); 
}

void Servo_Task_Loop(void) {
    static uint32_t last_time = 0;
    static uint8_t direction = 0; 
    uint32_t now = HAL_GetTick();

    if (now - last_time >= 2000) {
        if (direction == 0) {
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500); 
            direction = 1;
        } else {
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);  
            direction = 0;
        }
        last_time = now;
    }
}