/*
 * encoder.c
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
 * @file    encoder.c
 * @version	1.01
 *
 ******************************************************************************
 */

/*
 * /////////////////////////////////////////////////////////	E	N	C	O	D	E	R /////////////////////////////////////////////////////////
 */

#include "encoder.h"

/*
 * Encoder_Init
 *
 * @brief	This function is mandatory and defines parameters of the encoder.
 */
void Encoder_Init(
				Encoder_t		*Encoder,
				TIM_TypeDef		*set_enc_tim,
				uint32_t		 set_enc_dir_timeout) {

	Encoder->enc_tim = set_enc_tim;
	Encoder->enc_dir_timeout = set_enc_dir_timeout;
}

/*
 *  ///////////////////////////////////////////////////////// P O S I T I O N   M O D E
 */

/*
 * Encoder_Set_Cnt
 *
 * @brief	Loads counter of the encoder timer with desired value (for example rotational position reference point).
 */
void Encoder_Set_Cnt(Encoder_t *Encoder, uint16_t set_cnt) {
	Encoder->enc_tim->CNT = set_cnt;
}

/*
 * Encoder_Read_Cnt
 *
 * @brief	Returns actual counter / position value.
 * 			Hardware timer counter always knows if and how the encoder value has changed, and program reads it when it needs to.
 */
uint16_t Encoder_Read_Cnt(Encoder_t *Encoder) {
	return(Encoder->enc_tim->CNT);
}

/*
 * Encoder_Read_Cnt_Delta
 *
 * @brief	Is used to return value of CNT steps difference between calling Encoder_Poll_Dir.
 */
int32_t Encoder_Read_Cnt_Delta(Encoder_t *Encoder) {
	return(Encoder->enc_cnt_delta);
}

/*
 *  ///////////////////////////////////////////////////////// A D J U S T   V A L U E   M O D E
 */

/*
 * Encoder_Dir_Timeout
 *
 * @brief	This function resets the active state of DIR stored in memory after timeout period set by user in Encoder_Init.
 * 			It enables tasks to have a period of time for reading the values after encoder is physically inactive.
 */
void Encoder_Dir_Timeout(Encoder_t *Encoder) {
	if (Encoder->enc_dir != ENC_STATIC) {
		if ( (HAL_GetTick() - Encoder->enc_dir_timeout_start) >= Encoder->enc_dir_timeout) {
			Encoder->enc_dir = ENC_STATIC;
		}
	}
}

/*
 * Encoder_Poll_Dir
 *
 * @brief	Returns direction of the encoder since last calling it.
 * 			Timer counter value is polled and compared to previous poll, together with DIR bit value an increment, decrement or static is stored.
 *
 * @detail	This function will not change CNT value and does not interfere with position measurement
 */

Encoder_dir Encoder_Poll_Dir(Encoder_t *Encoder){
	Encoder->enc_cnt_last = Encoder->enc_cnt;
	Encoder->enc_cnt = Encoder->enc_tim->CNT;
	if (Encoder->enc_cnt_last != Encoder->enc_cnt) {
		Encoder->enc_dir_timeout_start = HAL_GetTick();
		if ( (Encoder->enc_tim->CR1 & TIM_CR1_DIR) == 0) {	/////////////////////////// works, but, IS IT CORRECT ?
			Encoder->enc_cnt_delta = (Encoder->enc_cnt - Encoder->enc_cnt_last);
			return(ENC_UP);
		}
		else {
			Encoder->enc_cnt_delta = (Encoder->enc_cnt_last - Encoder->enc_cnt);
			return(ENC_DOWN);
		}
	}
	else {
		return(ENC_STATIC);
	}
}

/*
 * Encoder_Adjust
 *
 * @brief	This function implements passed variable adjustment using timer counter direction polling,
 * 			min & max limit control and min-max loop cycle mode.
 *
 * @detail	Not affected by timer counter wrap event since DIR bit is monitored.
 *
 * 			Variable put in argument is updated upon each function call, process is limited by min / max value,
 * 			and cycling mode - overflow ( when incrementing max-->min or decrementing max<-min).
 *
 * 			Function is expected to be called while the main program / task is looping in adjustment state,
 * 			for example LCD menu page with numerical setting being modified by this function.
 * 			This method achieves very high responsiveness.
 */
uint32_t Encoder_Adjust(uint32_t value, uint32_t min, uint32_t max, Encoder_cycle set_cycle, Encoder_t *Encoder) {
	Encoder_Dir_Timeout(Encoder);
	Encoder->enc_dir = Encoder_Poll_Dir(Encoder);
	switch (Encoder->enc_dir) {
		case (ENC_STATIC):
				return(value);
			break;
		case (ENC_UP):
				if (value < max) {
					Encoder->enc_dir = ENC_STATIC;
					return(++value);
				}
				else if (value >= max) {
					switch (set_cycle) {
						case (ENC_CYCLE_OFF):
								Encoder->enc_dir = ENC_STATIC;
								return(max);
							break;
						case (ENC_CYCLE_ON):
								Encoder->enc_dir = ENC_STATIC;
								return(min);
							break;
						default:
								Encoder->enc_dir = ENC_STATIC;
								return(max);
							break;
					}
				}
				else {
					Encoder->enc_dir = ENC_STATIC;
					return(value);
				}
			break;
		case (ENC_DOWN):
				if (value > min) {
					Encoder->enc_dir = ENC_STATIC;
					return(--value);
				}
				if (value <= min) {
					switch (set_cycle) {
						case (ENC_CYCLE_OFF):
								Encoder->enc_dir = ENC_STATIC;
								return(min);
							break;
						case (ENC_CYCLE_ON):
								Encoder->enc_dir = ENC_STATIC;
								return(max);
							break;
						default:
								Encoder->enc_dir = ENC_STATIC;
								return(min);
							break;
					}
				}
				else {
					Encoder->enc_dir = ENC_STATIC;
					return(value);
				}
			break;
		default:
				Encoder->enc_dir = ENC_STATIC;
				return(value);
			break;
	}
}
