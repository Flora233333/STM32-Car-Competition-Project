#include "NRF24L01.h"

uint8_t RX_BUF[RX_PLOAD_WIDTH];		//�������ݻ���
uint8_t TX_BUF[TX_PLOAD_WIDTH];		//�������ݻ���
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x68,0x86,0x66,0x88,0x28};  // ����һ����̬���͵�ַ�������ַ��5���ֽڣ�40λ����Ϊ��ַλ�����ó���5����Ӧ������������ֽ�
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x68,0x86,0x66,0x88,0x28};	// �շ���ͬһ����ַ��ԭ������ղؼ�

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 


void SPI_NRF_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*������ӦIO�˿ڵ�ʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
                         |NRF_CSN_GPIO_CLK
                         |NRF_CE_GPIO_CLK
                         |NRF_IRQ_GPIO_CLK,ENABLE);

 /*ʹ��SPI1ʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

   /*���� SPI_NRF_SPI�� SCK,MISO,MOSI���ţ�GPIOA^5,GPIOA^6,GPIOA^7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���ù���
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  /*����SPI_NRF_SPI��CE����,��SPI_NRF_SPI�� CSN ����*/
   GPIO_InitStructure.GPIO_Pin = NRF_CSN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(NRF_CSN_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(NRF_CE_GPIO_PORT, &GPIO_InitStructure);


   /*����SPI_NRF_SPI��IRQ����*/
  GPIO_InitStructure.GPIO_Pin = NRF_IRQ_GPIO_CLK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //��������
  GPIO_Init(NRF_IRQ_GPIO_PORT, &GPIO_InitStructure); 
		  
  /* �����Զ���ĺ꣬��������csn���ţ�NRF�������״̬ */
  NRF_CSN_HIGH(); 
 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					           //��ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				         //���ݴ�С8λ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 			               //ʱ�Ӽ��ԣ�����ʱΪ��
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						           //��1��������Ч��������Ϊ����ʱ��
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					           //NSS�ź����������
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //8��Ƶ��9MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				       //��λ��ǰ
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}

/**
  * @brief   ������NRF��/дһ�ֽ�����
  * @param   д�������
  *		@arg dat 
  * @retval  ��ȡ�õ�����
  */
uint8_t SPI_NRF_RW(uint8_t dat)
{  	
   /* �� SPI���ͻ������ǿ�ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  
   /* ͨ�� SPI2����һ�ֽ����� */
  SPI_I2S_SendData(SPI1, dat);		
 
   /* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/**
  * @brief   ������NRF�ض��ļĴ���д��һ������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  *		@arg dat:��Ҫ��Ĵ���д�������
  * @retval  NRF��status�Ĵ�����״̬
  */
u8 SPI_NRF_WriteReg(uint8_t reg, uint8_t dat)
{
 	uint8_t status;
	
	NRF_CE_LOW(); //����������ʧ��NRF���ܽ��мĴ�����д����
	/*�õ�CSN��ʹ��SPI����*/
    NRF_CSN_LOW();
				
	/*��������Ĵ����� */
	status = SPI_NRF_RW(reg);
		 
	 /*��Ĵ���д������*/
    SPI_NRF_RW(dat); 
	          
	/*CSN���ߣ����*/	   
  	NRF_CSN_HIGH();	
		
	/*����״̬�Ĵ�����ֵ*/
   	return(status);
}

/**
  * @brief   ���ڴ�NRF�ض��ļĴ�������һ������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  * @retval  �Ĵ����е�����
  */
u8 SPI_NRF_ReadReg(uint8_t reg)
{
 	uint8_t reg_val;

	NRF_CE_LOW();
	/*�õ�CSN��ʹ��SPI����*/
 	NRF_CSN_LOW();
				
  	 /*���ͼĴ�����*/
	SPI_NRF_RW(reg); 

	 /*��ȡ�Ĵ�����ֵ */
	reg_val = SPI_NRF_RW(NOP);
	            
   	/*CSN���ߣ����*/
	NRF_CSN_HIGH();		
   	
	return reg_val;
}

/**
  * @brief   ������NRF�ļĴ����� ���� ��������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf�����ڴ洢���������ļĴ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
u8 SPI_NRF_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
 	uint8_t status, byte_cnt;

	NRF_CE_LOW();
	/*�õ�CSN��ʹ��SPI����*/
	NRF_CSN_LOW();
		
	/*���ͼĴ�����*/		
	status = SPI_NRF_RW(reg); 

 	/*��ȡ����������*/
	 for(byte_cnt=0; byte_cnt<bytes; byte_cnt++)		  
	   pBuf[byte_cnt] = SPI_NRF_RW(NOP); //��NRF24L01��ȡ���ݣ�NOP = 1111 1111,Ϊ�����ֲ����ض��Ķ�ʱ��SPI����ָ�� 

	 /*CSN���ߣ����*/
	NRF_CSN_HIGH();	
		
 	return status;		//���ؼĴ���״ֵ̬
}

/**
  * @brief   ������NRF�ļĴ����� ���� д������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf���洢�˽�Ҫд��д�Ĵ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
u8 SPI_NRF_WriteBuf(uint8_t reg , uint8_t *pBuf, uint8_t bytes)
{
	 uint8_t status,byte_cnt;
	 NRF_CE_LOW();
   	 /*�õ�CSN��ʹ��SPI����*/
	 NRF_CSN_LOW();			

	 /*���ͼĴ�����*/	
  	 status = SPI_NRF_RW(reg); 
 	
  	  /*�򻺳���д������*/
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		SPI_NRF_RW(*pBuf++);	//д���ݵ������� 	 
	  	   
	/*CSN���ߣ����*/
	NRF_CSN_HIGH();			
  
  	return (status);	//����NRF24L01��״̬ 		
}

/**
  * @brief  ���ò��������ģʽ
  * @param  ��
  * @retval ��
  */
void NRF_RX_Mode(void)

{
	NRF_CE_LOW();	

   SPI_NRF_WriteBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);//дRX�ڵ��ַ

   SPI_NRF_WriteReg(NRF_WRITE_REG + EN_AA, 0x01);    //ʹ��ͨ��0���Զ�Ӧ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01);//ʹ��ͨ��0�Ľ��յ�ַ    

   SPI_NRF_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL);      //����RFͨ��Ƶ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��      

   SPI_NRF_WriteReg(NRF_WRITE_REG + RF_SETUP, 0x0f); //����TX�������,0db����,2Mbps,���������濪��   

   SPI_NRF_WriteReg(NRF_WRITE_REG + CONFIG, 0x0f);  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 

/*CE���ߣ��������ģʽ*/	
   NRF_CE_HIGH();
} 

/**
  * @brief  ���÷���ģʽ
  * @param  ��
  * @retval ��
  */
void NRF_TX_Mode(void)
{  
	NRF_CE_LOW();		

   SPI_NRF_WriteBuf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    //дTX�ڵ��ַ 

   SPI_NRF_WriteBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK   

   SPI_NRF_WriteReg(NRF_WRITE_REG + EN_AA, 0x01);     //ʹ��ͨ��0���Զ�Ӧ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01); //ʹ��ͨ��0�Ľ��յ�ַ  

   SPI_NRF_WriteReg(NRF_WRITE_REG + SETUP_RETR, 0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��

   SPI_NRF_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL);       //����RFͨ��ΪCHANAL

   SPI_NRF_WriteReg(NRF_WRITE_REG + RF_SETUP, 0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
	
   SPI_NRF_WriteReg(NRF_WRITE_REG + CONFIG, 0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�

/*CE���ߣ����뷢��ģʽ*/	
  NRF_CE_HIGH();
	 //Delay(0xFFFF); //CEҪ����һ��ʱ��Ž��뷢��ģʽ
   delay_us(20);//����Ҫ����Delay���У������Ǹ�ʱ����ܲ���
}

/**
  * @brief  ��Ҫ����NRF��MCU�Ƿ���������
  * @param  ��
  * @retval SUCCESS/ERROR ��������/����ʧ��
  */
uint8_t NRF_Check(void)
{
	uint8_t buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	uint8_t buf1[5];
	uint8_t i; 
	 
	/*д��5���ֽڵĵ�ַ.  */  
	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

	/*����д��ĵ�ַ */
	SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
	 
	/*�Ƚ�*/               
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i == 5)
		return SUCCESS ;        //MCU��NRF�ɹ����� 
	else
		return ERROR ;        //MCU��NRF����������
}

/**
  * @brief   ������NRF�ķ��ͻ�������д������
  * @param   
  *		@arg txBuf���洢�˽�Ҫ���͵����ݵ����飬�ⲿ����	
  * @retval  ���ͽ�����ɹ�����TXDS,ʧ�ܷ���MAXRT��ERROR
  */
uint8_t NRF_Tx_Dat(uint8_t *txbuf)
{
	uint8_t state;  

	 /*ceΪ�ͣ��������ģʽ1*/
	NRF_CE_LOW(); //����״̬CE���������ߣ�0 -> 1�������ֲ��д��

	/*д���ݵ�TX BUF ��� 32���ֽ�*/						
    SPI_NRF_WriteBuf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);

      /*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */   
 	NRF_CE_HIGH();
	  	
	  /*�ȴ���������ж� */                            
	while(NRF_Read_IRQ()!=0); 	
	
	/*��ȡ״̬�Ĵ�����ֵ */                              
	state = SPI_NRF_ReadReg(STATUS);

	 /*���TX_DS��MAX_RT�жϱ�־*/                  
	SPI_NRF_WriteReg(NRF_WRITE_REG + STATUS, state); //�����ְ�ԭ����д��ȥ��ȷʵҲ������жϱ�־λ����ý���ֲ�����

	SPI_NRF_WriteReg(FLUSH_TX,NOP);    //���TX FIFO�Ĵ��� 

	 /*�ж��ж�����*/    
	if(state & MAX_RT)                     //�ﵽ����ط�����
			 return MAX_RT; 

	else if(state & TX_DS)                  //�������
		 	return TX_DS;
	 else						  
			return ERROR;                 //����ԭ����ʧ��
} 

/**
  * @brief   ���ڴ�NRF�Ľ��ջ������ж�������
  * @param   
  *		@arg rxBuf �����ڽ��ո����ݵ����飬�ⲿ����	
  * @retval 
  *		@arg ���ս��
  */
uint8_t NRF_Rx_Dat(uint8_t *rxbuf)
{
	uint8_t state; 
	NRF_CE_HIGH();	 //�������״̬
	 /*�ȴ������ж�*/
	while(NRF_Read_IRQ() == 0) //�������
  {
    NRF_CE_LOW();  	 //�������״̬��ֻ�н���ǰ����ʱCE���ߣ�
    /*��ȡstatus�Ĵ�����ֵ  */               
    state = SPI_NRF_ReadReg(STATUS);
     
    /* ����жϱ�־*/      
    SPI_NRF_WriteReg(NRF_WRITE_REG + STATUS, state);

    /*�ж��Ƿ���յ�����*/
    if(state & RX_DR)                                 //���յ�����
    {
		SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf, RX_PLOAD_WIDTH);//��ȡ����
        SPI_NRF_WriteReg(FLUSH_RX,NOP);          //���RX FIFO�Ĵ���
		return RX_DR; 
    }
    else    
      return ERROR;                    //û�յ��κ�����
  }
  
  return ERROR;                    //û�յ��κ�����
}

extern uint8_t status;

void Check(void) {
	status = NRF_Check(); 

  /*�ж�����״̬*/  
    if(status == SUCCESS)	   
     printf("\r\nNRF1��MCU���ӳɹ�\r\n");  
    else	  
     printf("\r\nNRF1��MCU����ʧ��\r\n");
}
