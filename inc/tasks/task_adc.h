/*
 * app_adc.h
 *
 *  Created on: 2017-6-27
 *      Author: redchenjs
 */

#ifndef INC_TASKS_TASK_ADC_H_
#define INC_TASKS_TASK_ADC_H_

extern void adc1_fft(void);
extern void adc1_voltage_caculate(void);
extern void adc1_spectrum_caculate(void);
extern void adc1_harmonic_caculate(void);

extern void adc2_fft(void);
extern void adc2_current_caculate(void);
extern void adc2_spectrum_caculate(void);
extern void adc2_harmonic_caculate(void);

extern void adc1_2_power_caculate(void);

#endif /* INC_TASKS_TASK_ADC_H_ */
