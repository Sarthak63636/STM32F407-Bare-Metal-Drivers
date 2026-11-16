/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: 05-Oct-2026
 *      Author: negis
 */

#include"stm32f407xx_spi_driver.h"

 /*
   * Some Helper function implementation
   */

 static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
 static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
 static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);


 uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx,uint32_t FlagName)
 {
	 if(pSPIx->SR & FlagName)
	 {
		 return FLAG_SET;
	 }
	 else
	 {
		return FLAG_RESET;
	 }
 }

/*************************************************************************
 * @fn                  - SPI_PeriClockControl
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
 void SPI_PeriClockControl(SPI_RegDef_t *pSPIx,uint8_t EnorDi)
 {
	 if(EnorDi== ENABLE)
	 	{
	 		if(pSPIx == SPI1)
	 		{
	 			SPI1_PCLK_EN();
	 		}
	 		else if (pSPIx== SPI2)
	 		{
	 			SPI2_PCLK_EN();
	 		}
	 		else if (pSPIx== SPI3)
	 		{

	 		    SPI3_PCLK_EN();
	 		}

	 	}
	 else
	 {
		 if(pSPIx == SPI1)
		 	 {
		 	    SPI1_PCLK_DI();
		 	 }
		 else if (pSPIx== SPI2)
		 	 {
		 	 	SPI2_PCLK_DI();
		 	 }
		 else if (pSPIx== SPI3)
		 	 {
		 	     SPI3_PCLK_DI();
		 	 }

	 }

 }

 /*************************************************************************
  * @fn                 -SPI_Init
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

 void SPI_Init(SPI_Handle_t *pSPIHandle)

 {

	 SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);
   // first lets configure the SPI_CR1 register

	 uint32_t tempreg =0;

	 //1.Configure the device mode

	 tempreg|=pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR ;

	 //2. Configure the bus Config

	 if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD )
	 {
		 tempreg&=~(1 << 15); //  bidi mode should be cleared

	 }

	 else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	 {
		 tempreg|=(1 << 15); //  bidi mode should be set
		 tempreg |= (1 << 14);   /* BIDIOE: transmit only (use a different value if you want to receive) */
	 }

	 else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	 {
		 //bidi mode should be cleared
		 tempreg &=~(1 << 15);

		 // RXONLY bit must be set
		 tempreg |=(1 << 10);
	 }

	 //3.Configure the spi serial clock speed (baud rate)
	 tempreg |=pSPIHandle->SPIConfig.SPI_SCLKSpeed << SPI_CR1_BR ;

	 //4.Configure the DFF
	 tempreg|=pSPIHandle->SPIConfig.SPI_DFF<<SPI_CR1_DFF ;

	 //5.configure the cpol
	 tempreg|=pSPIHandle->SPIConfig.SPI_CPOL <<SPI_CR1_CPOL;

	 //6.configure the cpha
	 tempreg|=pSPIHandle->SPIConfig.SPI_CPHA <<SPI_CR1_CPHA;

	 //7.Configure the SSM
	 tempreg|=pSPIHandle->SPIConfig.SPI_SSM <<SPI_CR1_SSM;


	  pSPIHandle->pSPIx->CR1 = tempreg;
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
 void SPI_DeInit(SPI_RegDef_t *pSPIx)
 {
	    if(pSPIx == SPI1)
	 	{
	 		SPI1_REG_RESET();
	 	}
	 	else if (pSPIx == SPI2)
	 	{
	 	    SPI2_REG_RESET();
	 	}
	 	else if (pSPIx == SPI3)
	 	{
	 		SPI3_REG_RESET();
	 	}
 }



 /*************************************************************************
  * @fn                  -SPI_SendData
  *
  * @brief				-
  *
  * @param[in]			-
  * @param[in]			-
  *
  * @return				-none
  *
  * @note                -This is blocking call
  *
  */
 void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t*pTxBuffer,uint32_t len)
 {
	 while(len>0)
	 {
		 //1.wait until txe is set
		 while (SPI_GetFlagStatus(pSPIx,SPI_TXE_FLAG) == FLAG_RESET);

		 //2.CHeck the dff bit of the CR1 register

		 if(pSPIx->CR1 & (1<<SPI_CR1_DFF) )
		 {
			 //16 bit dff
			 //1.load the data into the dr

			 pSPIx->DR =*((uint16_t*)pTxBuffer);
			 len--;
			 len--;
			 pTxBuffer+=2;
		 }
		 else
		 {
			 //8 bit dff

			 pSPIx->DR =*pTxBuffer;
			 len--;
			 pTxBuffer++;
		 }

	 }
	 while (SPI_GetFlagStatus(pSPIx, SPI_BSE_FLAG) == FLAG_SET);
 }

 /*************************************************************************
  * @fn                  -SPI_ReceiveData
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
 void SPI_ReceiveData(SPI_RegDef_t *pSPIx,uint8_t*pRxBuffer,uint32_t len)
 {
	 while(len>0)
	 {
	        // 1. wait until TX buffer is empty (TXE, not RXNE)
	        while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		 // 2. send dummy data -> this starts the clock
		  pSPIx->DR = 0xFFFF;     // 8-bit mode uses only the lower byte (0xFF)

		 // 3. wait until the reply has arrived
          while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		 //4.CHeck the dff bit of the CR1 register

		 if(pSPIx->CR1 & (1<<SPI_CR1_DFF) )
		 {
			 //16 bit dff
			 //1.load the data into the dr

			 *((uint16_t*)pRxBuffer)=pSPIx->DR ;
			 len--;
			 len--;
			 pRxBuffer+=2;
		 }
		 else
		 {
			 //8 bit dff

			 *((uint8_t*)pRxBuffer)=pSPIx->DR ;
			 len--;
			 pRxBuffer++;
		 }

	 }
 }



  /*************************************************************************
   * @fn                  -SPI_PeripheralControl
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
 void SPI_PeripheralControl(SPI_RegDef_t *pSPIx,uint8_t EnOrDi)
 {
	 if(EnOrDi ==ENABLE)
	 {
		 pSPIx->CR1|=(1<<SPI_CR1_SPE);
	 }
	 else
	 {
		 pSPIx->CR1&=~(1<<SPI_CR1_SPE);
	 }
 }

 /*************************************************************************
   * @fn                  -SPI_SSIConfig
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

 void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
 {
	 if(EnOrDi == ENABLE)
		 {
			 pSPIx->CR2|=(1<<SPI_CR2_SSOE);
		 }
		 else
		 {
			 pSPIx->CR2&=~(1<<SPI_CR2_SSOE);
		 }
 }

 /*************************************************************************
   * @fn                  -SPI_SSIConfig
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

 void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
 {
	 if(EnOrDi == ENABLE)
		 {
			 pSPIx->CR1|=(1<<SPI_CR1_SSI);
		 }
		 else
		 {
			 pSPIx->CR1&=~(1<<SPI_CR1_SSI);
		 }
 }


 /*************************************************************************
  * @fn                 - SPI_IRQInterruptConfig
  *
  * @brief				-
  *
  * @param[in]			-
  * @param[in]			-
  *
  * @return				-none
  *
  * @note               -none
  *
  */

 void SPI_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
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
 void SPI_IRQPriorityConfig(uint8_t IRQNumber ,uint32_t IRQPriority)

 {
		//1. first find out ipr register
		uint8_t iprx=IRQNumber / 4;
		uint8_t iprx_section =IRQNumber %4;

		uint8_t shift_amount =(8* iprx_section) +(8-NO_PR_BITS_IMPLEMENTED );

		*(NVIC_PR_BASE_ADDR +iprx)|=( IRQPriority << shift_amount );
 }


 /*************************************************************************
  * @fn                  -SPI_SendDataIT
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


 uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t*pTxBuffer,uint32_t len)
  {
	 uint8_t state=pSPIHandle->TxState;

	 if(state !=SPI_BUSY_IN_TX)
	 {
  //1.Save the Tx buffer address and len information in some global addresses
	 pSPIHandle->pTxBuffer=pTxBuffer;
	 pSPIHandle->Txlen=len;

  //2.Mark the SPI state as busy in the transmission so that no other code can take over same SPI peripheral
  //  until transmission is over
	 pSPIHandle->TxState=SPI_BUSY_IN_TX;

  //3.ENAble the TXEIE control bit to get interrupt whenever txe flag is set in SR
	 pSPIHandle->pSPIx->CR2 |=(1<<SPI_CR2_TXEIE);

  //4.Data Transmission will be handled by  the ISR code.
	 }
	 return state;
  }
 /*************************************************************************
  * @fn                  -SPI_ReceiveDataIT
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
  uint8_t SPI_ReceiveDataIT(SPI_Handle_t*pSPIHandle,uint8_t*pRxBuffer,uint32_t len)
  {
		 uint8_t state=pSPIHandle->RxState;

		 if(state !=SPI_BUSY_IN_RX)
		 {
	  //1.Save the Rx buffer address and len information in some global addresses
		 pSPIHandle->pRxBuffer=pRxBuffer;
		 pSPIHandle->Rxlen=len;

	  //2.Mark the SPI state as busy in the Receiving so that no other code can take over same SPI peripheral
	  //  until Receiving is over
		 pSPIHandle->RxState=SPI_BUSY_IN_RX;

	  //3.ENAble the RXNEIE control bit to get interrupt whenever txe flag is set in SR
		 pSPIHandle->pSPIx->CR2 |=(1<<SPI_CR2_RXNEIE);

	  //4.Data Receiving will be handled by the ISR code.
		 }
		 return state;
  }

  /*************************************************************************
   * @fn                  -SPI_IRQHandling
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
  void SPI_IRQHandling(SPI_Handle_t *pHandle)

  {
	  uint8_t temp1,temp2;
    //First check for TXE
	  temp1=pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	  temp2=pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	  if(temp1 && temp2)
	  {
		  //handle TXE
		  spi_txe_interrupt_handle(pHandle);
	  }

	  //check for RXNE
	 temp1=pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	 temp2=pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);
	 if(temp1 && temp2)
	 {
		//handle RXNE
		 spi_rxne_interrupt_handle(pHandle);
	 }
	   // check for ovr flag
	    temp1=pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	    temp2=pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);
	 if(temp1 && temp2)
	 {
	   //handle ovr
	 	 spi_ovr_err_interrupt_handle(pHandle);
	 }

  }

 /*
 * 	   * Some Helper function implementation
 */

	 static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
	 {

			 //CHeck the dff bit of the CR1 register

			 if(pSPIHandle->pSPIx->CR1 & (1<<SPI_CR1_DFF) )
			 {
				 //16 bit dff
				 //1.load the data into the dr

				 pSPIHandle->pSPIx->DR =*((uint16_t*)pSPIHandle->pTxBuffer);
				 pSPIHandle->Txlen--;
				 pSPIHandle->Txlen--;
				 pSPIHandle->pTxBuffer+=2;
			 }
			 else
			 {
				 //8 bit dff

				 pSPIHandle->pSPIx->DR =(*pSPIHandle->pTxBuffer);
				 pSPIHandle->Txlen--;
				 pSPIHandle->pTxBuffer++;
			 }

			 if(! pSPIHandle->Txlen)
			 {
				 //Txlen == 0,so close the spi communication and inform the application that
				 //Tx is over
				 //this prevents interrupt from setting up for TXE flag
				 SPI_CloseTransmission(pSPIHandle);
				 SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_TX_CMPLT);
			 }
	 }
	 static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
	 {
		 //CHeck the dff bit of the CR1 register

		 if(pSPIHandle->pSPIx->CR1 & (1<<SPI_CR1_DFF) )
		 {
			 //16 bit dff
			 //1.load the data into the dr

			 *((uint16_t*)pSPIHandle->pRxBuffer)= pSPIHandle->pSPIx->DR ;
			 pSPIHandle->Rxlen--;
			 pSPIHandle->Rxlen--;
			 pSPIHandle->pRxBuffer+=2;
		 }
		 else
		 {
			 //8 bit dff

			 (*pSPIHandle->pRxBuffer)= pSPIHandle->pSPIx->DR ;
			 pSPIHandle->Rxlen--;
			 pSPIHandle->pRxBuffer++;
		 }

		 if(! pSPIHandle->Rxlen)
		 {
			 //reception is complete
			 //lets turn off rxneie buffer
			 //this prevents interrupt from setting up for TXE flag
			 pSPIHandle->pSPIx->CR2 &=~(1<<SPI_CR2_RXNEIE);
			 SPI_CloseReception(pSPIHandle);
			 SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_RX_CMPLT);
		 }
	}


	 static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
	 {
       uint8_t temp;
      //1.clear the ovr flag
      if(pSPIHandle->TxState!=SPI_BUSY_IN_TX)
      {
    	temp=pSPIHandle->pSPIx->DR;
    	temp=pSPIHandle->pSPIx->SR;
      }
      (void)temp;
      //2. inform the application
      SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_OVR_CMPLT);
	 }

	 void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
	 {
		 pSPIHandle->pSPIx->CR2 &=~(1<<SPI_CR2_TXEIE);
		 pSPIHandle->pTxBuffer=NULL;
		 pSPIHandle->Txlen=0;
		 pSPIHandle->TxState=SPI_READY;
	 }
	 void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
	 {
		 pSPIHandle->pSPIx->CR2 &=~(1<<SPI_CR2_RXNEIE);
		 pSPIHandle->pRxBuffer=NULL;
		 pSPIHandle->Rxlen=0;
		 pSPIHandle->RxState=SPI_READY;
	 }

	 void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
	 {
		uint8_t temp;
	    temp=pSPIx->DR;
		temp=pSPIx->SR;
		(void)temp;
	 }


	__attribute__((weak))  void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
	{
		//this is weak implementation. the application may override this function.

	}







