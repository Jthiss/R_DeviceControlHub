/**
  * @brief  Torgue control A1 command
  * @param  uint8_t Motor_ID 
  * @param  int32_t iqControl
  */

void CAN_TorgueControl(uint8_t Motor_ID, int32_t iqControl)
{
  uint8_t TxData[8] = {0};
  
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;            
  TxHeader.StdId = std_id + Motor_ID;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = 8;
    
  TxData[0] = 0xA1;
  TxData[1] = 0;
  TxData[2] = 0;
  TxData[3] = 0;
  TxData[4] = *(uint8_t *)(&iqControl); 
  TxData[5] = *((uint8_t *)(&iqControl)+1);
  TxData[6] = 0;
  TxData[7] = 0;
        
 if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
 {
   /* Transmission request Error */
   Error_Handler();
 }
}

/**
  * @brief  speeed control A2 command
  * @param  uint8_t Motor_ID 
  * @param  int32_t speedControl
  */
void speedControl(uint8_t Motor_ID, int32_t speedControl)
{
  uint8_t TxData[8] = {0};
  
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;            
  TxHeader.StdId = std_id + Motor_ID;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = 8;
    
  TxData[0] = 0xA2;
  TxData[1] = 0;
  TxData[2] = 0;
  TxData[3] = 0;
  TxData[4] = *(uint8_t *)(&speedControl); 
  TxData[5] = *((uint8_t *)(&speedControl)+1);
  TxData[6] = *((uint8_t *)(&speedControl)+2);
  TxData[7] = *((uint8_t *)(&speedControl)+3);
        
 if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
 {
   /* Transmission request Error */
   Error_Handler();
 }

}

/**
  * @brief  Multi_angleControl_1 A3 command
  * @param  uint8_t Motor_ID 
  * @param  int32_t angleControl
  */
void Multi_angleControl_1(uint8_t Motor_ID, int32_t angleControl)
{
  uint8_t TxData[8] = {0};
  
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;            
  TxHeader.StdId = std_id + Motor_ID;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = 8;
    
  TxData[0] = 0xA3;
  TxData[1] = 0;
  TxData[2] = 0;
  TxData[3] = 0;
  TxData[4] = *(uint8_t *)(&angleControl); 
  TxData[5] = *((uint8_t *)(&angleControl)+1);
  TxData[6] = *((uint8_t *)(&angleControl)+2);
  TxData[7] = *((uint8_t *)(&angleControl)+3);
        
 if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
 {
   /* Transmission request Error */
   Error_Handler();
 }

}
/**
  * @brief  Multi_angleControl_1 A4 command
  * @param  uint8_t Motor_ID 
  * @param  uint16_t maxSpeed
  * @param  int32_t angleControl
  */
void Multi_angleControl_2(uint8_t Motor_ID, uint16_t maxSpeed, int32_t angleControl)
{

  uint8_t TxData[8] = {0};
  
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;            
  TxHeader.StdId = std_id + Motor_ID;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = 8;
    
  TxData[0] = 0xA4;
  TxData[1] = 0;
  TxData[2] = *(uint8_t *)(&maxSpeed);
  TxData[3] = *(uint8_t *)((&maxSpeed)+1);
  TxData[4] = *(uint8_t *)(&angleControl); 
  TxData[5] = *((uint8_t *)(&angleControl)+1);
  TxData[6] = *((uint8_t *)(&angleControl)+2);
  TxData[7] = *((uint8_t *)(&angleControl)+3);
  
  //HAL_UART_Transmit(&huart1, TxData, 8, 10);
  
  if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
  {
    /* Transmission request Error */
    Error_Handler();
  }

}
/**
  * @brief  Single_loop_angleControl_1 A5 command
  * @param  uint8_t Motor_ID 
  * @param  uint8_t spinDirection
  * @param  int16_t angleControl
  */
void Single_loop_angleControl_1(uint8_t Motor_ID, uint8_t spinDirection, uint16_t angleControl)
{

  uint8_t TxData[8] = {0};
  
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;            
  TxHeader.StdId = std_id + Motor_ID;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = 8;
    
  TxData[0] = 0xA5;
  TxData[1] = spinDirection;
  TxData[2] = 0;
  TxData[3] = 0;
  TxData[4] = *(uint8_t *)(&angleControl); 
  TxData[5] = *((uint8_t *)(&angleControl)+1);
  TxData[6] = 0;
  TxData[7] = 0;
        
 if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
 {
   /* Transmission request Error */
   Error_Handler();
 }

}
/**
  * @brief  Single_loop_angleControl_2 A6 command
  * @param  uint8_t Motor_ID 
  * @param  uint8_t spinDirection
  * @param  uint16_t maxSpeed
  * @param  int16_t angleControl
  */
void Single_loop_angleControl_2(uint8_t Motor_ID, uint8_t spinDirection, uint16_t maxSpeed, uint16_t angleControl)
{

  uint8_t TxData[8] = {0};
  
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;            
  TxHeader.StdId = std_id + Motor_ID;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = 8;
    
  TxData[0] = 0xA6;
  TxData[1] = spinDirection;
  TxData[2] = *(uint8_t *)(&maxSpeed);
  TxData[3] = *((uint8_t *)(&maxSpeed)+1);
  TxData[4] = *(uint8_t *)(&angleControl); 
  TxData[5] = *((uint8_t *)(&angleControl)+1);
  TxData[6] = 0;
  TxData[7] = 0;
        
 if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
 {
   /* Transmission request Error */
   Error_Handler();
 }

}

/**
  * @brief  Motor_Off command
  * @param  uint8_t Motor_ID 
  */
void Motor_Off(uint8_t Motor_ID)
{

  uint8_t TxData[8] = {0};
  
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;            
  TxHeader.StdId= std_id + Motor_ID;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = 8;
    
  TxData[0] = 0x81;
  TxData[1] = 0;
  TxData[2] = 0;
  TxData[3] = 0;
  TxData[4] = 0;
  TxData[4] = 0;
  TxData[5] = 0;
  TxData[6] = 0;
  TxData[7] = 0;
        
 if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
 {
   /* Transmission request Error */
   Error_Handler();
 }

}

