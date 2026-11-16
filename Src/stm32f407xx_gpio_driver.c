


#include"stm32f407xx_gpio_driver.h"



/*************************************************************************
 * @fn                  - GPIO_PeriClockControl
 *
 * @brief				-This function enables or disables peripheral clock for the given GPIO port
 *
 * @param[in]			-base address of the gpio peripheral
 * @param[in]			-Enable or Disable macros.
 *
 * @return				-none
 *
 * @note                -none
 *
 */

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx,uint8_t EnorDi)
{
	if(EnorDi== ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		}
		else if (pGPIOx== GPIOB)
		{
			GPIOB_PCLK_EN();
		}
		else if (pGPIOx== GPIOC)
		{
		    GPIOC_PCLK_EN();
		}
		else if (pGPIOx== GPIOD)
		{
		    GPIOD_PCLK_EN();
		}
		else if (pGPIOx== GPIOE)
		{
			GPIOE_PCLK_EN();
		}
		else if (pGPIOx== GPIOF)
		{
    	    GPIOF_PCLK_EN();
		}
		else if (pGPIOx== GPIOG)
		{
			GPIOG_PCLK_EN();
		}
		else if (pGPIOx== GPIOH)
		{
	        GPIOH_PCLK_EN();
		}
		else if (pGPIOx== GPIOI)
		{
			GPIOI_PCLK_EN();
		}
	}
	else
	{
		if(pGPIOx == GPIOA)
		{
		    GPIOA_PCLK_DI();
		}
		else if (pGPIOx== GPIOB)
		{
			GPIOB_PCLK_DI();
		}
		else if (pGPIOx== GPIOC)
		{
			GPIOC_PCLK_DI();
		}
		else if (pGPIOx== GPIOD)
		{
		    GPIOD_PCLK_DI();
		}
		else if (pGPIOx== GPIOE)
		{
			GPIOE_PCLK_DI();
		}
		else if (pGPIOx== GPIOF)
		{
		    GPIOF_PCLK_DI();
		}
		else if (pGPIOx== GPIOG)
		{
			GPIOG_PCLK_DI();
		}
		else if (pGPIOx== GPIOH)
		{
	        GPIOH_PCLK_DI();
		}
		else if (pGPIOx== GPIOI)
		{
			GPIOI_PCLK_DI();
		}
	}
}
/*************************************************************************
 * @fn                  - GPIO_Init
 *
 * @brief				-Initializes the GPIO peripheral according to the configuration provided in the GPIO handle.
 *
 * @param[in]			- pGPIOHandle - Pointer to GPIO handle structure.

 *
 * @return				-none
 *
 * @note                -none
 *
 */

void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
   	uint32_t temp=0; //temp register

   	GPIO_PeriClockControl(pGPIOHandle->pGPIOx,ENABLE);



    //1.CONFIGURE the mode of gpio pin

     if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN)
     {
	    //NON interrupt mode
    	 temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    	 pGPIOHandle->pGPIOx->MODER &=~(0x3 << 2* pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	 pGPIOHandle->pGPIOx->MODER |=temp;

     }
     else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT)
	 {
    	 //NON interrupt mode
    	    	 temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode<< (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    	    	 pGPIOHandle->pGPIOx->MODER &=~(0x3 << 2* pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	    	 pGPIOHandle->pGPIOx->MODER |=temp;
	 }
     else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	 {
    	  //NON interrupt mode
    	    	 temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode<< (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    	    	 pGPIOHandle->pGPIOx->MODER &=~(0x3 << 2* pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	    	 pGPIOHandle->pGPIOx->MODER |=temp;
	 }
     else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ANALOG)
	 {
    	 //NON interrupt mode
    	     	 temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode<< (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    	     	 pGPIOHandle->pGPIOx->MODER &=~(0x3 << 2* pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	     	 pGPIOHandle->pGPIOx->MODER |=temp;
	 }
     else
     {
    	 temp =(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

    	 EXTI->FTSR &= ~temp;
    	 EXTI->RTSR &= ~temp;
       // Interrupt mode
    	 if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode ==GPIO_MODE_IT_FT)
    	 {
    		//1.Configure the FTSR
    		 EXTI->FTSR |=(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    		 EXTI->RTSR &=~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	 }
    	 else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode ==GPIO_MODE_IT_RT)
    	 {
    		//1.Configure the RTSR
    		 EXTI->RTSR |=(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    		 EXTI->FTSR &=~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	 }
    	 else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode ==GPIO_MODE_IT_RFT)
    	 {
    		//1.Configure both FTSR and RTSR
    		 EXTI->RTSR |=(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    		 EXTI->FTSR |=(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	 }
    	  //2.Configure the gpio port selection in SYSCFG_EXTICR
          uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber /4;
          uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber %4;
          uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
          SYSCFG_PCLK_EN();
          SYSCFG->EXTICR[temp1]&=~(0xF << (temp2*4));
          SYSCFG->EXTICR[temp1]|=(portcode << (temp2*4));

    	 //3. enable the exti interrupt delivery using IMR
    	 EXTI->IMR |=(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
     }
     temp=0;

     //2.Configure the speed

     temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
     pGPIOHandle->pGPIOx->OSPEEDR &=~(0x3 <<2* pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
     pGPIOHandle->pGPIOx->OSPEEDR |=temp;

     temp=0;

     //3. configure the PUPD settings

     temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
     pGPIOHandle->pGPIOx->PUPDR &=~(0x3 <<2* pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
     pGPIOHandle->pGPIOx->PUPDR |=temp;

     temp=0;

     //3. configure the optype

     temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType<< (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
     pGPIOHandle->pGPIOx->OTYPER &=~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
     pGPIOHandle->pGPIOx->OTYPER |=temp;

     temp=0;

     //4.Configure the alt functionity

     if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
     {
    	 //Configure the alt function register.
    	 uint32_t temp1 ,temp2;
    	 temp1=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;  //this will help us separate the AF[0] low alternate function register from 0 to 7 and AF[1] high alternate function register from 8 to 15
         temp2=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

         pGPIOHandle->pGPIOx->AFR[temp1] &=~(0xF << 4* temp2); // CLEARING BIT
         pGPIOHandle->pGPIOx->AFR[temp1] |= pGPIOHandle->GPIO_PinConfig.GPIO_PinALTFunMode << (4*temp2); //SETTING BIT
      }
      temp=0;
}

/*************************************************************************
 * @fn                  - GPIO_DEInit
 *
 * @brief				-
 *
 * @param[in]			-
 *
 * @return				-none
 *
 * @note                -none
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();
	}
	else if (pGPIOx== GPIOB)
	{
	    GPIOB_REG_RESET();
	}
	else if (pGPIOx== GPIOC)
	{
		GPIOC_REG_RESET();
	}
	else if (pGPIOx== GPIOD)
	{
		GPIOD_REG_RESET();
	}
	else if (pGPIOx== GPIOE)
	{
		GPIOE_REG_RESET();
	}
	else if (pGPIOx== GPIOF)
	{
		GPIOF_REG_RESET();
	}
	else if (pGPIOx== GPIOG)
	{
		GPIOG_REG_RESET();
	}
	else if (pGPIOx== GPIOH)
	{
		GPIOH_REG_RESET();
	}
	else if (pGPIOx== GPIOI)
	{
		GPIOI_REG_RESET();
	}
}



/*************************************************************************
 * @fn                  - GPIO_ReadFromInputPin
 *
 * @brief				-
 *
 * @param[in]			-.
 *
 * @return				-none
 *
 * @note                -none
 *
 */

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber)
{
   uint8_t value;
   value=(uint8_t)((pGPIOx->IDR  >> PinNumber) &  0x1);
   return value;
}

/*************************************************************************
 * @fn                  - GPIO_ReadFromInputPort
 *
 * @brief				-This function enables or disables peripheral clock for the given GPIO port
 *
 * @param[in]			-
 *
 * @return				-none
 *
 * @note                -none
 *
 */

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	   uint16_t value;
	   value=(uint16_t) pGPIOx->IDR ;
	   return value;
}


/*************************************************************************
 * @fn                  - GPIO_WriteFromOututPin
 *
 * @brief				-
 *
 * @param[in]			-
 *
 * @return				-none
 *
 * @note                -none
 *
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber,uint8_t Value)
{
 if(Value == GPIO_PIN_SET)
 {
	 //write 1 to the output data register at the bit field corresponding to the pin number
  pGPIOx->ODR|=(1 << PinNumber)	 ;

 }
 else
 {
	 // write 0
	  pGPIOx->ODR&=~(1 << PinNumber)	 ;
 }
}

/*************************************************************************
 * @fn                  - GPIO_WriteFromOutputPort
 *
 * @brief				-
 *
 * @param[in]			-
 *
 * @return				-none
 *
 * @note                -none
 *
 */

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,uint8_t Value)
{
 pGPIOx->ODR=Value;
}

/*************************************************************************
 * @fn                  - GPIO_ToggleOutputPin
 *
 * @brief				-This function enables or disables peripheral clock for the given GPIO port
 *
 * @param[in]			-base address of the gpio peripheral
 * @param[in]			-Enable or Disable macros.
 *
 * @return				-none
 *
 * @note                -none
 *
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber)
{
 pGPIOx->ODR =pGPIOx->ODR ^(1<<PinNumber);
}


/*************************************************************************
 * @fn                  - GPIO_IRQConfig
 *
 * @brief				-
 *
 * @param[in]			-
 *
 *
 * @return				-none
 *
 * @note                -none
 *
 */

void GPIO_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
{
  if(EnorDi ==ENABLE)
  {
	  if (IRQNumber <= 31)
	  {
		  // program ISERO Register
		  *NVIC_ISER0|=(1 << IRQNumber);

	  }
	  else if (IRQNumber > 31 && IRQNumber < 64)
	  {
		  // program ISER1 register
		  *NVIC_ISER1|=(1 << IRQNumber%32);
	  }
	  else if(IRQNumber >=64 && IRQNumber < 96)
	  {
		  // program ISER2 register
		  *NVIC_ISER2|=(1 << IRQNumber%64);
	  }
  }
  else
  {

	  if (IRQNumber <= 31)
	  {
		  // program ICER0 register
		  *NVIC_ICER0|=(1 << IRQNumber);
	  }
	  else if (IRQNumber > 31 && IRQNumber < 64)
	  {
		  // program ICER1 register
		  *NVIC_ICER1|=(1 << IRQNumber%32);
	  }
	  else if(IRQNumber >=64 && IRQNumber < 96)
	  {
		  // program ICERW Register
		  *NVIC_ICER2|=(1 << IRQNumber%64);
	  }
  }
}

void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)

{
	//1. first find out ipr register
	uint8_t iprx=IRQNumber / 4;
	uint8_t iprx_section =IRQNumber %4;

	uint8_t shift_amount =(8* iprx_section) +(8-NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR +iprx)|=( IRQPriority << shift_amount );
}

/*************************************************************************
 * @fn                  - GPIO_IRQHanding.
 *
 * @brief				-
 *
 * @param[in]			-
 *
 *
 * @return				-none
 *
 * @note                -none
 *
 */
void GPIO_IRQHandling(uint8_t PinNumber)
{
// clear the EXTI pr register corresponding to the pin number
	if(EXTI->PR &(1<< PinNumber))
	{
		//clear
		EXTI->PR |=(1<<PinNumber);
	}
}
