//#ifndef INC_I2C_ER_H_
//#define INC_I2C_ER_H_
//
//#define SCL_PIN     GPIO_PIN_10
//#define SCL_PORT    GPIOB
//
//#define SDA_PIN     GPIO_PIN_11
//#define SDA_PORT    GPIOB
//
//void HAL_I2C_ClearBusyFlagErrata_2_14_7(I2C_HandleTypeDef *hi2c) {
//
//    static uint8_t resetTried = 0;
//    if (resetTried == 1) {
//        return ;
//    }
//    GPIO_InitTypeDef GPIO_InitStruct;
//
//    // 1
//    __HAL_I2C_DISABLE(hi2c);
//
//    // 2
//    GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);
//    HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);
//
//    // 3
//    GPIO_PinState pinState;
//    if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//    if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//
//    // 4
//    GPIO_InitStruct.Pin = SDA_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_TogglePin(GPIOB, SDA_PIN);
//
//    // 5
//    if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_SET) {
//        for(;;){}
//    }
//
//    // 6
//    GPIO_InitStruct.Pin = SCL_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_TogglePin(GPIOB, SCL_PIN);
//
//    // 7
//    if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_SET) {
//        for(;;){}
//    }
//
//    // 8
//    GPIO_InitStruct.Pin = SDA_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);
//
//    // 9
//    if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//
//    // 10
//    GPIO_InitStruct.Pin = SCL_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);
//
//    // 11
//    if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//
//    // 12
//    GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//   // 13
//   hi2c->Instance->CR1 |= I2C_CR1_SWRST;
//
//   // 14
//   hi2c->Instance->CR1 ^= I2C_CR1_SWRST;
//
//   // 15
//   __HAL_I2C_ENABLE(hi2c);
//
//   resetTried = 1;
//}
//
//void HAL_GPIO_WRITE_ODR(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
//{
//  /* Check the parameters */
//  assert_param(IS_GPIO_PIN(GPIO_Pin));
//
//  GPIOx->ODR |= GPIO_Pin;
//} // Call initialization function.
//
//
//
//#endif /* INC_I2C_ER_H_ */
