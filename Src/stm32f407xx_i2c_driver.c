/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: 15-Oct-2026
 *      Author: negis
 */
#include"stm32f407xx_i2c_driver.h"

uint16_t AHB_PreScaler[8] ={2,4,8,16,64,128,256,512};
uint16_t APB1_PreScaler[4]={2,4,8,16};



static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);








static void I2C_GenerateStartCondition(I2C_RegDef_t*pI2Cx)
{
	 pI2Cx->CR1|=(1 << I2C_CR1_START);
}


static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |=(1<< I2C_CR1_STOP);
}



static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr)
{
	SlaveAddr=(SlaveAddr << 1); //as slave addr is 7 bit now we have 0th bit for read write bit ,

	SlaveAddr&=~(1); //7th Bit set to 0(WRITE)

    pI2Cx->DR=SlaveAddr;
}



static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr)
{
	SlaveAddr=(SlaveAddr << 1); //as slave addr is 7 bit now we have 0th bit for read write bit ,

	SlaveAddr|=(1); //7th Bit set to 1(READ)

    pI2Cx->DR=SlaveAddr;
}



static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;
	// check for device master or slave mode
	if(pI2CHandle->pI2Cx->SR2 & (1<<I2C_SR2_MSL))
	{
		//Master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize ==1 )
			{
				// first disable the ack
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//Clear the Addr flag
				dummy_read=pI2CHandle->pI2Cx->SR1;
				dummy_read=pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}
		}
		else
		{
			//Clear the Addr flag
				dummy_read=pI2CHandle->pI2Cx->SR1;
				dummy_read=pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
		}
	}
	else
	{
		//Slave
		//Clear the Addr flag
		       dummy_read=pI2CHandle->pI2Cx->SR1;
		       dummy_read=pI2CHandle->pI2Cx->SR2;
		    	(void)dummy_read;
	}

}




/*************************************************************************
 * @fn                  -I2C_PeripheralControl
 *
 * @brief				-
 *
 * @param[in]			-
 * @param[in]			-
 *
 * @return				-none
 *
 * @note                -none
 *
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx,uint8_t EnOrDi)
{
	 if(EnOrDi ==ENABLE)
	 {
		 pI2Cx->CR1|=(1<<I2C_CR1_PE);
	 }
	 else
	 {
		 pI2Cx->CR1 &=~(1<<I2C_CR1_PE);
	 }
}
/*************************************************************************************************************8
 * @fn                  - I2C_PeriClockControl
 *
 * @brief				-for enabling peripheral clock
 *
 * @param[in]			-
 * @param[in]			-
 *
 * @return				-none
 *
 * @note                -none
 *
 */


 void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi)
 {
		if (EnorDi==ENABLE)
		{
			 if(pI2Cx==I2C1)
			 {
				 I2C1_PCLK_EN();
			 }
			 else if(pI2Cx==I2C2)
			 {
				 I2C2_PCLK_EN();
			 }
			 else if(pI2Cx==I2C3)
			 {
				 I2C3_PCLK_EN();
			 }
		}
		if (EnorDi==DISABLE)
		{
			 if(pI2Cx==I2C1)
			 {
				 I2C1_PCLK_DI();
			 }
			 else if(pI2Cx==I2C2)
			 {
				 I2C2_PCLK_DI();
			 }
			 else if(pI2Cx==I2C3)
			 {
				 I2C3_PCLK_DI();
			 }
		}
 }

uint32_t RCC_GetPLLOutputClock()
 {
	 return 0;
 }

 uint32_t RCC_GetPCLK1Value(void)
 {
	 uint32_t pclk1,SystemCLK;
	 uint8_t clksrc,temp;
	 uint16_t ahbp,apb1p;
	 clksrc=0;
	 temp=0;

	 clksrc|=(RCC->CFGR & 0x3);
	 if(clksrc == 0)
	 {
		 SystemCLK =16000000; // HSI
	 }
	 else if(clksrc == 1)
	 {
		 SystemCLK =8000000; //HSE
	 }
	 else if(clksrc == 2)
	 {
		 SystemCLK =RCC_GetPLLOutputClock();
	 }

	 temp = (RCC->CFGR>>4) & 0xF;//shifts bit 4 to 7 to 0 to 4
	  if(temp<8)
	  {
		  ahbp=1;
	  }
	  else
	  {
		  ahbp=AHB_PreScaler[temp-8];
	  }


	  temp |= (RCC->CFGR>>10) & 0x7;
	  if(temp<4)
	  {
	      apb1p=1;
	  }
	  else
	  {
		  apb1p=APB1_PreScaler[temp-4];
	  }

	  pclk1=(SystemCLK/ahbp)/apb1p;
	 return pclk1;
 }
 /*************************************************************************
  * @fn                  - I2C_Init
  *
  * @brief				-for enabling peripheral clock
  *
  * @param[in]			-
  * @param[in]			-
  *
  * @return				-none
  *
  * @note                -none
  *
  */
 void I2C_Init(I2C_Handle_t *pI2CHandle)
 {

	 I2C_PeriClockControl(pI2CHandle->pI2Cx,ENABLE);

   uint32_t tempreg=0;
  //ack control bit
   tempreg|=(pI2CHandle->I2CConfig.I2C_ACKControl << I2C_CR1_ACK  );
   pI2CHandle->pI2Cx->CR1=tempreg;

  //configure the freq field of Cr2
   tempreg=0;
   tempreg=RCC_GetPCLK1Value()/1000000U;
   pI2CHandle->pI2Cx->CR2=(tempreg & 0x3F);//except first 5 bits all bits will be zero

   //program device own address

   tempreg=0;
   tempreg=pI2CHandle->I2CConfig.I2C_DeviceAddress << 1;
   tempreg|=(1<<14); // must be set written in reference manual
   pI2CHandle->pI2Cx->OAR1=tempreg;

   //CCR calculations
   uint16_t ccr_value=0;
   tempreg=0;
   if(pI2CHandle->I2CConfig.I2C_SCLSpeed<=I2C_SCL_SPEED_SM)
   {
	   //this mode is standard mode
	   ccr_value=RCC_GetPCLK1Value()/(2*(pI2CHandle->I2CConfig.I2C_SCLSpeed));
	   tempreg|=(ccr_value & 0xFFF);
   }
   else
   {
	   //else this mode is fast mode
	   tempreg|=(1<<15);
	   tempreg|=(pI2CHandle->I2CConfig.I2C_FMDutyCycle<<14);
	   if(pI2CHandle->I2CConfig.I2C_FMDutyCycle==I2C_FM_DUTY2 )
	   {
		ccr_value=RCC_GetPCLK1Value()/(3*(pI2CHandle->I2CConfig.I2C_SCLSpeed));
	   }
	   else
	   {
		ccr_value=RCC_GetPCLK1Value()/(25*(pI2CHandle->I2CConfig.I2C_SCLSpeed));
	   }
	   tempreg|=(ccr_value & 0xFFF);
   }
   pI2CHandle->pI2Cx->CCR=tempreg;


    //TRISE CONFIGURATION
   if(pI2CHandle->I2CConfig.I2C_SCLSpeed<=I2C_SCL_SPEED_SM)
    {
 	   //this mode is standard mode

 	   tempreg=(RCC_GetPCLK1Value()/1000000U )+1;


    }
    else
    {
 	   //else this mode is fast mode

       tempreg=(RCC_GetPCLK1Value()*300/1000000000U )+1;

    }

    pI2CHandle->pI2Cx->TRISE=(tempreg & 0x3F); //0x3f is used so only 6 bits are used

 }

 /*************************************************************************
  * @fn                  - I2C_DeInit
  *
  * @brief				-
  *
  * @param[in]			-
  * @param[in]			-
  *
  * @return				-none
  *
  * @note                -none
  *
  */
 void I2C_DeInit(I2C_RegDef_t *pI2Cx)
 {
	    if(pI2Cx == I2C1)
	 	{
	 		I2C1_REG_RESET();
	 	}
	 	else if (pI2Cx == I2C2)
	 	{
	 	    I2C2_REG_RESET();
	 	}
	 	else if (pI2Cx == I2C3)
	 	{
	 		I2C3_REG_RESET();
	 	}
 }

 /*************************************************************************
   * @fn                  - I2C_MasterSendData
   *
   * @brief				-
   *
   * @param[in]			-
   * @param[in]			-SR=repeated Start
   *
   * @return				-none
   *
   * @note                -none
   *
   */

 void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer,uint8_t Len,uint8_t SlaveAddr,uint8_t Sr)
 {

	 //1.Generate the start Condition
	 I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	 //2. Confirm that the start generation is complete by checking the sb flag in the SR1
	 //NOTE- UNtil SB is cleared SCL will be stretched (pulled to low)
	 while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB));

     //3.SEND the ADDRESS of the slave with the r/nw bit set to W(0) (total 8 bits)
	 I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	 //4. Confirm that address phase is complete by checking the Addr flag in the sr1
	 while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR));

	 //5. clear the ADDr flag according to its software sequence
	 I2C_ClearADDRFlag(pI2CHandle); //we need to clear this Addr bit or the Scl will be streched until

     //6 send data until len become 0
	 while(Len>0)
	 {
		 while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_TXE)); //Wait till TXE is set
		 pI2CHandle->pI2Cx->DR=*pTxBuffer;
		 pTxBuffer++;
		 Len--;

	 }

	 //7. when len becomes zero wait for txe=1 and BTF=1 before generating the stop condition
	 //NOTE-TXE=1 and BTF=1 ,means that both SR and DR are empty and next transmission should begin
	 // when BTF=1 SCLwill be stretched

	 while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));

	 while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));


	 //8. Generate STOP condition and master need not to wait for the completion of stop condition.
	 // NOTE-Generating stop , automatically clears the BTF
	 if(Sr == I2C_DISABLE_SR)
	 I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

 }

 /*************************************************************************
   * @fn                  - I2C_MasterReceiveData
   *
   * @brief				-
   *
   * @param[in]			-
   * @param[in]			-
   *
   * @return				-none
   *
   * @note                -none
   *
   */
 void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer,uint8_t Len,uint8_t SlaveAddr,uint8_t Sr)
 {

	 //1.Generate the start Condition
	 I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	 //2. Confirm that the start generation is complete by checking the sb flag in the SR1
	 //NOTE- UNtil SB is cleared SCL will be stretched (pulled to low)

	 while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB));

     //3.SEND the ADDRESS of the slave with the r/nw bit set to R(1) (total 8 bits)

	 I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	 //4. Confirm that address phase is complete by checking the Addr flag in the sr1

	 while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR));

	//procedure to read only 1 byte from the slave
	 if(Len ==1)
	 {
		 //Disable Acking
		 I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

		 //Clear the ADDR FLag
		 I2C_ClearADDRFlag(pI2CHandle);

		 //Wait until RXNE becomes 1 (RXNE =Data register not empty (receivers))
		 while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		 //Generate stop condition
		 if(Sr == I2C_DISABLE_SR)
		 {
		 I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		 }

		 //read data into buffer
		 *pRxBuffer= pI2CHandle->pI2Cx->DR;

		 if(pI2CHandle->I2CConfig.I2C_ACKControl==I2C_ACK_ENABLE)
		 {
			 I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
		 }
		 return;

	 }

	 //procedure to read data from slave when Len >1
	 else if(Len>1)

	 {
		 //Clear the Addr Flag
		 I2C_ClearADDRFlag(pI2CHandle);

		 //read the data until len becomes zero
		 for(uint32_t i=Len ; i>0 ; i--)
		 {
			 //wait until RXNE becomes 1
			 while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_RXNE));

			 if(i==2) //if last 2 bytes are remaining
			 {

			     //Disable acking
			      I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

			    // generate STOP condition
			     if(Sr == I2C_DISABLE_SR)
			     I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		     }

		     //read the data from data register in to the buffer
		      *pRxBuffer= pI2CHandle->pI2Cx->DR;

		     //increment the buffer adddress
		       pRxBuffer++;
	     }
     }

     //re-enable ACking
	 if(pI2CHandle->I2CConfig.I2C_ACKControl==I2C_ACK_ENABLE)
	 {
	 I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
	 }

 }



 /*************************************************************************
   * @fn                  - I2C_IRQInterruptConfig
   *
   * @brief				-
   *
   * @param[in]			-
   * @param[in]			-
   *
   * @return				-none
   *
   * @note                -none
   *
   */


 void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
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

 /*************************************************************************
  * @fn                  -
  *
  * @brief				-
  *
  * @param[in]			-
  * @param[in]			-
  *
  * @return				-none
  *
  * @note                -none
  *
  */
 void I2C_IRQPriorityConfig(uint8_t IRQNumber ,uint32_t IRQPriority)

 {
		//1. first find out ipr register
		uint8_t iprx=IRQNumber / 4;
		uint8_t iprx_section =IRQNumber %4;

		uint8_t shift_amount =(8* iprx_section) +(8-NO_PR_BITS_IMPLEMENTED );

		*(NVIC_PR_BASE_ADDR +iprx)|=( IRQPriority << shift_amount );
 }





 /*************************************************************************
   * @fn                  - I2C_MasterSendDataIT
   *
   * @brief				-
   *
   * @param[in]			-
   * @param[in]			-
   *
   * @return				-none
   *
   * @note                -none
   *
   */
 uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer,uint8_t Len,uint8_t SlaveAddr,uint8_t Sr)//SR=Repeated start
 {
 uint8_t busystate=pI2CHandle->TxRxState;

 if((busystate !=I2C_BUSY_IN_TX) &&(busystate !=I2C_BUSY_IN_RX))
 {
	 pI2CHandle->pTxBuffer=pTxBuffer;
	 pI2CHandle->Txlen=Len;
	 pI2CHandle->TxRxState=I2C_BUSY_IN_TX;
	 pI2CHandle->DevAddr=SlaveAddr;
	 pI2CHandle->Sr=Sr;

 // Generate Start condition

 I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

 //Enable the ITBUFEN bit
pI2CHandle->pI2Cx->CR2=(1<<I2C_CR2_ITBUFEN);
 //Enable the ITERREN bit
pI2CHandle->pI2Cx->CR2=(1<<I2C_CR2_ITERREN);
 //Enable the ITEVTEN bit
pI2CHandle->pI2Cx->CR2=(1<<I2C_CR2_ITEVTEN);

 }
 return busystate;

 }
 /*************************************************************************
   * @fn                  - I2C_MasterReceiveDataIT
   *
   * @brief				-
   *
   * @param[in]			-
   * @param[in]			-
   *
   * @return				-none
   *
   * @note                -none
   *
   */
 uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer,uint8_t Len,uint8_t SlaveAddr,uint8_t Sr) //SR=Repeated start
 {
	 uint8_t busystate=pI2CHandle->TxRxState;

	 if((busystate !=I2C_BUSY_IN_TX) &&(busystate !=I2C_BUSY_IN_RX))
	 {
		 pI2CHandle->pRxBuffer=pRxBuffer;
		 pI2CHandle->Rxlen=Len;
		 pI2CHandle->TxRxState=I2C_BUSY_IN_RX;
		 pI2CHandle->RxSize=
		 pI2CHandle->DevAddr=SlaveAddr;
		 pI2CHandle->Sr=Sr;

	 // Generate Start condition

	 I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	 //Enable the ITBUFEN bit
	pI2CHandle->pI2Cx->CR2=(1<<I2C_CR2_ITBUFEN);
	 //Enable the ITERREN bit
	pI2CHandle->pI2Cx->CR2=(1<<I2C_CR2_ITERREN);
	 //Enable the ITEVTEN bit
	pI2CHandle->pI2Cx->CR2=(1<<I2C_CR2_ITEVTEN);

	 }
	 return busystate;
 }

 uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx,uint32_t FlagName)
 {
	 if(pI2Cx->SR1 & FlagName)
	 {
		return FLAG_SET;
	 }
	 else
	 {
		 return FLAG_RESET;
	 }
 }

 void I2C_ManageAcking(I2C_RegDef_t *pI2Cx,uint8_t EnorDi)
 {
	 if(EnorDi == I2C_ACK_ENABLE)
	 {
		 //enable the ack
		  pI2Cx->CR1|=(1<<I2C_CR1_ACK );
	 }
	 else
	 {
		 //disable the ack
		  pI2Cx->CR1&=~(1<<I2C_CR1_ACK );
	 }
 }
 /*************************************************************************
   * @fn                  - I2C_ER_IRQHandling
   *
   * @brief				-
   *
   * @param[in]			-
   * @param[in]			-
   *
   * @return				-none
   *
   * @note                -none
   *
   */

 void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
 {
		// Implement the Code to disable ItBUFN Control bit
		 pI2CHandle->pI2Cx->CR2 &=~(1<<I2C_CR2_ITBUFEN);

		 // Implement the Code to disable Itevten Control bit
		 pI2CHandle->pI2Cx->CR2 &=~(1<<I2C_CR2_ITEVTEN);

		 pI2CHandle->TxRxState=I2C_READY;
		 pI2CHandle->pTxBuffer=NULL;
		 pI2CHandle->Txlen=0;

		 if(pI2CHandle->I2CConfig.I2C_ACKControl==I2C_ACK_ENABLE)
		 {
		 I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
		 }
 }

 void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
 {
	// Implement the Code to disable ItBUFN Control bit
	 pI2CHandle->pI2Cx->CR2 &=~(1<<I2C_CR2_ITBUFEN);

	 // Implement the Code to disable Itevten Control bit
	 pI2CHandle->pI2Cx->CR2 &=~(1<<I2C_CR2_ITEVTEN);

	 pI2CHandle->TxRxState=I2C_READY;
	 pI2CHandle->pRxBuffer=NULL;
	 pI2CHandle->RxSize=0;
	 pI2CHandle->Rxlen=0;

	 if(pI2CHandle->I2CConfig.I2C_ACKControl==I2C_ACK_ENABLE)
	 {
	 I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
	 }

 }

 void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
 {
	 // Interrupt handling for both master and slave mode of a device
	  uint8_t temp1,temp2,temp3;

     temp1=pI2CHandle->pI2Cx->CR2 &(1<<I2C_CR2_ITEVTEN);
	 temp2= pI2CHandle->pI2Cx->SR1 &(1<<I2C_CR2_ITBUFEN);


	 //1.Handle For interrupt generated by the sb event
	 //note Sb flag is only implemented in master mode
	 temp3=pI2CHandle->pI2Cx->SR1 &(1<<I2C_SR1_SB);
	 if(temp1 && temp3) //SB flag is set it confirms
	  {
		// this interrupt is generated because of Sb Flag
		//this mode will not execute in slave mode for slave mode sb=0;
		//lets execute the Addr phase
		 if(pI2CHandle->TxRxState==I2C_BUSY_IN_TX)
		 {
			 I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		 }
		 else
		 {
			 I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		 }

	  }

	 //2.HandleFor interrupt generated by the Addr event
	 //NOTE- when master mode -Address is sent
	 //       when slave mode -Address matched with own address

	 temp3=pI2CHandle->pI2Cx->SR1 &(1<<I2C_SR1_ADDR);
	 if(temp1 && temp3) //ADDR flag is set it confirms
	 {
		//INterrupt is generated because of Addr Event
		 I2C_ClearADDRFlag(pI2CHandle);
	 }

	 //3.Handled for interrupt generatred by BTF (BYTE TRANSFERED FINISH) EVENT
	 temp3=pI2CHandle->pI2Cx->SR1 &(1<<I2C_SR1_BTF);
	 if(temp1 && temp3) //BTF flag is set it confirms
	 {
		 if(temp1 && temp3) //BTF  flag is set it confirms
		 {
			 //make sure TXE IS SET
			 if(pI2CHandle->TxRxState==I2C_BUSY_IN_TX)
			 {
				 // ENtering this loop means btf and txe =1 which means sr(shift register) and Dr(data register) are empty

				 if(pI2CHandle->Txlen ==0)
				 {
					 //1.GENErate the Stop Condition
				 if(pI2CHandle->Sr ==I2C_DISABLE_SR)
				 {
				 I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
				 }

				 //2.Reset all the members elements of thehandle structure.
				 I2C_CloseSendData(pI2CHandle);

				 //3.Notify the application about transmission complete
				 I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_TX_CMPLT);
				 }

			 }
			 else if(pI2CHandle->TxRxState==I2C_BUSY_IN_RX)
			 {
				;
			 }
		 }
	 }

	 //4.Handled for interrupt generated by STOPF Function
	 //Note-STOPF deteection flag is applicable only for slave,For master this flag will never be set
	 // The below code block will not be executed by the mastersince STOPF will not be set by the maaster

	 temp3=pI2CHandle->pI2Cx->SR1 &(1<<I2C_SR1_STOPF);
	 if(temp1 && temp3) //STOPF flag is set
	 {
		 //STOPF is set
		 //CLEAR THE STOPF i.e(read to SR1 and write to CR1 )
		 //we have written in the sr in the temp3 line
         // now we have to write to CR1 ,dont writte anything so we can bitwise or it with the 0x0000;
		 pI2CHandle->pI2Cx->CR1|=(0x0000);  //this will make Stopf bit clear

		 //3.Notify the application that Stop is detected
		 I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_STOP);
	 }


	 //5 HAndled for interrupt generated by TXE event
	 temp3=pI2CHandle->pI2Cx->SR1 &(1<<I2C_SR1_TXE);
	 if(temp1 && temp3 && temp2) //Txe flag is set
	 {
		 //Check for device mode
		 if(pI2CHandle->pI2Cx->SR2  & (1<<I2C_SR2_MSL))
		 {
	 	   //We have to do DAta transmission
		    if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX) //TXE FLAG IS SET
		    	{
		    		I2C_MasterHandleTXEInterrupt(pI2CHandle);
		    	}

		 }
		 else
		 {
			 //Slave mode
			 //Make sure that the slave really is in transmitter mode
			 if(pI2CHandle->pI2Cx->SR2 & (1<< I2C_SR2_TRA))
			 {
				 I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
			 }
		 }
	 }

	 //6 handled for interrupt generated by Rxne event
	 temp3=pI2CHandle->pI2Cx->SR1 &(1<<I2C_SR1_RXNE);
	  if(temp1 && temp3 && temp2)
 	  {
		  //Check for device mode
		 if(pI2CHandle->pI2Cx->SR2  & (1<<I2C_SR2_MSL))
		 	 	 	 {
			 // MASTER MODE
			 	 	 if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)  //RXNE IS SET
			 	 	 	 {
			 	 		 	 I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			 	 	 	 }

	     }
		 else
		 		 {
		 			 //Slave mode
		 			 //Make sure that the slave really is in RE mode
		 			 if(!(pI2CHandle->pI2Cx->SR2 & (1<< I2C_SR2_TRA)))
		 			 {
		 				 I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
		 			 }
		 		 }
 	   }


 }




 // Some INterrupt Helper function


static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
 {


	// We have to do Data Reception
	 if(pI2CHandle->RxSize ==1)
	 {
		 *((uint16_t*)pI2CHandle->pRxBuffer)=pI2CHandle->pI2Cx->DR;
		 pI2CHandle->Rxlen--;

	 }

	 if(pI2CHandle->RxSize > 1)
	 {
		 if(pI2CHandle->Rxlen==2)
		 {
		 //Clear the ack bit
		 I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
		 }
		 //REad Dr
		 *pI2CHandle->pRxBuffer=pI2CHandle->pI2Cx->DR;
		 pI2CHandle->pRxBuffer ++;
		 pI2CHandle->Rxlen--;

	 }

	 if(pI2CHandle->RxSize == 0)
	 {
		 //close the I2c reception and notify the application

		 //1.Generate the Stop Condition
		 if(pI2CHandle->Sr==I2C_DISABLE_SR)
		 {
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx) ;
		 }

		 //2. Close the I2c Rx
		 I2C_CloseReceiveData(pI2CHandle);

		 //3.Notify the Application
		 I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_RX_CMPLT);

	 }

  }

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->Txlen >0)
	{
		 // 1.Load the bit into the Dr register
		 pI2CHandle->pI2Cx->DR=*(pI2CHandle->pTxBuffer);

		 //2.Decrement the Tx len
		 pI2CHandle->Txlen --;

		  // 3.increment the Tx buffer
		 pI2CHandle->pTxBuffer ++;

	}

}



__attribute__((weak))  void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{
	//this is weak implementation. the application may override this function.

}

/*********************************************************************
 * @fn      		  - I2C_ER_IRQHandling
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - Complete the code also define these macros in the driver
						header file
						#define I2C_ERROR_BERR  3
						#define I2C_ERROR_ARLO  4
						#define I2C_ERROR_AF    5
						#define I2C_ERROR_OVR   6
						#define I2C_ERROR_TIMEOUT 7

 */

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);
		//Implement the code to notify the application about the error
		  I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);
	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);
		//Implement the code to notify the application about the error
		  I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);
		//Implement the code to notify the application about the error
		  I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);
		//Implement the code to notify the application about the error
		  I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}

}

void I2C_SlaveSendData(I2C_RegDef_t *pI2C,uint8_t Data)
{
	pI2C->DR=Data;
}
uint8_t I2C_SLaveReceiveData(I2C_RegDef_t *pI2C)
{
  return 	(uint8_t)pI2C->DR;
}
