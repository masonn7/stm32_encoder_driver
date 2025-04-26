/*
 * encoder.h
 *
 *  Created on: 15 May 2022
 *      Author: Martin Andruska
 */

/*
MIT License

Copyright (c) 2025 Martin Andruska (https://github.com/masonn7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 ******************************************************************************
 * @file    encoder.h
 * @version	1.01
 * @brief	Each encoder instance has all the parameters stored separately.
 *
 * Timer hardware limits (resolution) for position sensing:
 *  8 bit timer: 0 - 255
 * 16 bit timer: 0 - 65535
 * 32 bit timer: 0 - 4294967295
 ******************************************************************************
 */

#ifndef __ENCODER_H_
#define __ENCODER_H_

/*
 * /////////////////////////////////////////////////////////	E	N	C	O	D	E	R /////////////////////////////////////////////////////////
 */

/* Include hal lib according to hardware. */
#include "stm32f4xx_hal.h"

typedef enum {
	ENC_CYCLE_OFF = 0,
	ENC_CYCLE_ON = 1,
	  }
Encoder_cycle;

typedef enum {
	ENC_STATIC = 0,
	ENC_DOWN = 1,
	ENC_UP = 2,
	  }
Encoder_dir;

typedef struct {
// user config
TIM_TypeDef				*enc_tim;
uint32_t				enc_dir_timeout;	// define dir value timeout period in HAL ticks (ms) after which the dir value is reset to ENC_STATIC.
// process variables
uint32_t				enc_dir_timeout_start;
uint32_t				enc_tick_present;
Encoder_dir 			enc_dir;
uint16_t 				enc_cnt;
uint16_t 				enc_cnt_last;
int32_t					enc_cnt_delta;		// counter difference in non-absolute value since last Encoder_Poll_Dir function execution
	}Encoder_t;

void Encoder_Init(
				Encoder_t		*Encoder,
				TIM_TypeDef		*set_enc_tim,
				uint32_t		 set_enc_dir_timeout);

void Encoder_Set_Cnt(Encoder_t *Encoder, uint16_t set_cnt);
uint16_t Encoder_Read_Cnt(Encoder_t *Encoder);
int32_t Encoder_Read_Cnt_Delta(Encoder_t *Encoder);
void Encoder_Dir_Timeout(Encoder_t *Encoder);
Encoder_dir Encoder_Poll_Dir(Encoder_t *Encoder);
uint32_t Encoder_Adjust(uint32_t value, uint32_t min, uint32_t max, Encoder_cycle set_cycle, Encoder_t *Encoder);

#endif /* __ENCODER_H_ */
