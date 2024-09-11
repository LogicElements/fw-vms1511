/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "i2c.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "common.h"

#include "modbus_slave.h"
#include "configuration.h"
#include "mb_upgrade.h"
#include "system_msp.h"
#include "switch.h"
//#include "adc.h"
#include "control.h"
#include "flash_app.h"
#include "led.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define I2CEXPANDERADDRESS	64;	// 7 Bit sequence - 0 1 0 0 0 0 0 0 - 0 1 0 0 (fixed) A2 A1 A0 R/W	A2,A1,A0 == 0, R/W == 0 --> Write

// I2C Register bank address
// All for settings IOCON.BANK = 0	(IOCON register byte 7)
#define I2CIOCONADDRESS		0x0A;
#define I2CGPIOA			0x12;
#define I2CGPIOB			0x13;
#define I2CIODIRAADDRESS	0x00;
#define I2CIODIRBADDRESS	0x01;

//	After reset set I2CIODIRx register 0000 0000 --> GPIOx 0-7 == Output pins
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uint32_t tick = 0;

	/* Remap reset vector and enable interrupts */
	System_RemapApplicationVector();
	__enable_irq();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

	uint8_t data_buffer[2];
	uint16_t expander_address = I2CEXPANDERADDRESS;
	uint16_t gpioaaddress = I2CGPIOA;

	amp_gain_t current_gain = GAIN_1;
	amp_load_t current_load = LOAD_100R;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_IWDG_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

	/* Initialize all configured peripherals */
	System_InitWdg();
	System_CrcInit();
	Config_Init();
	FlashApp_Init();
	MbSlave_Init();
	Led_Init();
	Switch_Init();
	Control_Init();
	HAL_GPIO_WritePin(MCU_ON_GPIO_Port, MCU_ON_Pin, 1);
	HAL_GPIO_WritePin(I2C_NRST_GPIO_Port, I2C_NRST_Pin, 1);

	int i = 0;	// For MCU_LED blink


	// set IODIRA register 0000 0000 == all GPIOA´s pins are output (reset state is all input)
	data_buffer[0] = 0;
	data_buffer[1] = 0;
	HAL_I2C_Master_Transmit(&hi2c2, expander_address, data_buffer,
			sizeof(data_buffer), 500);

	// Set I2C expander register address into data buffer
	data_buffer[0] = gpioaaddress;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		/* High priority handles */
		MbSlave_Handle();

		// Simple LED blinking for MCU´s working visible ctrl
		if (i > 100000)
		{
			i = 0;
			HAL_GPIO_TogglePin(MCU_ON_GPIO_Port, MCU_ON_Pin);
		}
		else
			i++;

		//	OAMP Gain & Input impedance settings via MODBUS
		if ((current_gain != conf.amp.gain) || (current_load != conf.amp.load))
		{
			current_load = conf.amp.load;
			current_gain = conf.amp.gain;
			data_buffer[1] = (1u << (current_load)) | (1u << (current_gain + 4));
			HAL_I2C_Master_Transmit(&hi2c2, expander_address, data_buffer,sizeof(data_buffer), 500);
		}

		/* Low priority - slow handles */
		if (TICK_EXPIRED(tick))
		{
			tick = HAL_GetTick() + PERIOD_HANDLE_LOW_PRIO;
			conf.sys.io_input = Switch_GetAll();
			Control_Handle();
			MbSlave_UpdateSlaveAddress();

			if (Switch_GetPressed() > (5000 - 750) / PERIOD_HANDLE_LOW_PRIO)
			{
				FlashApp_RequestFactorySettings();
			}
			if (Switch_GetReleased()
					> 0&& Switch_GetReleased() <= (2500 - 750) / PERIOD_HANDLE_LOW_PRIO)
			{
				System_Reset();
			}
			FlashApp_Handle();
			System_ReloadWdg();
		}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* UART receive complete Callbacks */
	MbSlave_RxCpltCallback(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/* UART transmit complete Callbacks */
	MbSlave_TxCpltCallback(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	/* UART error Callbacks */
	MbSlave_ErrorCallback(huart);
}

#define FW_UPGR_MASK            0xFFFF
#define FW_UPGR_VALUE_FIRM      CONF_TARGET_DEVICE

Status_t MbUpgr_HeaderClb(uint16_t type, uint16_t mode, uint32_t size)
{
	Status_t ret = STATUS_OK;

	if ((type & FW_UPGR_MASK) == FW_UPGR_VALUE_FIRM)
	{
		switch (mode)
		{
		case MB_UPGR_MODE_ERASE_AT_START:
			ret = System_FlashErase((uint32_t) CONF_C_APP_BUFFER_OFFSET,
					(uint32_t) CONF_C_APP_BUFFER_OFFSET + size);
			break;
		case MB_UPGR_MODE_APPLY:
			System_Delay(1000);
			System_Reset();
			break;
		default:
			break;
		}
	}
	else
	{
		ret = STATUS_ERROR;
	}

	return ret;
}

Status_t MbUpgr_WritePageClb(uint32_t offset, uint8_t *data, uint16_t length)
{
	Status_t ret = STATUS_OK;

	/* Write new data into flash */
	ret = System_FlashProgram((uint32_t) CONF_C_APP_BUFFER_OFFSET + offset,
			data, length);

	return ret;
}

Status_t MbUpgr_WriteDoneClb(uint32_t length)
{
	Status_t ret = STATUS_OK;

	if ((MbUpgr_GetType() & FW_UPGR_MASK) == FW_UPGR_VALUE_FIRM)
	{
		/* The very last packet, verify image */
		ret = System_VerifyImage((uint32_t*) CONF_C_APP_BUFFER_OFFSET);

		/* In case of error, erase application buffer */
		if (ret)
		{
			System_FlashErase((uint32_t) CONF_C_APP_BUFFER_OFFSET,
					(uint32_t) CONF_C_APP_BUFFER_OFFSET + length);
		}
	}
	else
	{
		ret = STATUS_ERROR;
	}

	return ret;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
