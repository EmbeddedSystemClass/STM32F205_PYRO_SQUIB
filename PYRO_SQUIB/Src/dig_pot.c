#include "dig_pot.h"
#include "stm32f2xx_hal.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define I2C_MUX_ADDR		0xE0
#define I2C_POT_ADDR		0x5C
#define I2C_TIMEOUT_MS	10

extern I2C_HandleTypeDef hi2c2;



void DigPot_Init(void)
{
		HAL_GPIO_WritePin(A0_AKT_GPIO_Port, A0_AKT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A1_AKT_GPIO_Port, A1_AKT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A2_AKT_GPIO_Port, A2_AKT_Pin, GPIO_PIN_RESET);

//		I2C_AFBusyBugWorkaround(&hi2c2);		
}

//HAL_StatusTypeDef i2c_err=HAL_OK;

#define I2C_MAX_ERR_NUM		10
#define I2C_RESET_DELAY		10

extern void MX_I2C2_Init(void);
HAL_StatusTypeDef DigPot_SetValue(enDigPot DigPot, uint8_t value)
{
	  uint8_t i2cMuxReg;
		uint8_t i2cErrCnt=0;
		HAL_StatusTypeDef err=HAL_ERROR;
		
		i2cMuxReg=(DigPot&0x3)|0x4;
	
		while(i2cErrCnt<I2C_MAX_ERR_NUM)
		{
				err=HAL_I2C_Master_Transmit(&hi2c2,I2C_MUX_ADDR,&i2cMuxReg,1,I2C_TIMEOUT_MS);// select a potentiometer
				if(err!=HAL_OK)
				{
						i2cErrCnt++;
//						HAL_I2C_DeInit(&hi2c2);
//						vTaskDelay(I2C_RESET_DELAY);
//						HAL_I2C_Init(&hi2c2);
						MX_I2C2_Init();
						vTaskDelay(I2C_RESET_DELAY);
						continue;
				}
				
				err=HAL_I2C_Master_Transmit(&hi2c2,I2C_POT_ADDR,&value,1,I2C_TIMEOUT_MS);//set potentiometer value
				
				if(err!=HAL_OK)
				{
						i2cErrCnt++;
						//reset I2C
//						HAL_I2C_DeInit(&hi2c2);
//						vTaskDelay(I2C_RESET_DELAY);
//						HAL_I2C_Init(&hi2c2);
						MX_I2C2_Init();
						vTaskDelay(I2C_RESET_DELAY);
						continue;
				}
				
				break;
		}
		return err;
	
//		err=HAL_I2C_Master_Transmit(&hi2c2,I2C_MUX_ADDR,&i2cMuxReg,1,I2C_TIMEOUT_MS);// select a potentiometer
//	
//		if(err==HAL_OK)
//		{
//				err=HAL_I2C_Master_Transmit(&hi2c2,I2C_POT_ADDR,&value,1,I2C_TIMEOUT_MS);//set potentiometer value
//		}		
//		else
//		{
//				i2c_err=err;
//		}
//		
//		return err;
}


