/*
 * task_disp.h
 *
 *  Created on: 2017-6-27
 *      Author: redchenjs
 */

#ifndef INC_TASKS_TASK_DISP_H_
#define INC_TASKS_TASK_DISP_H_

extern void disp_adc1_voltage(void);
extern void disp_adc1_spectrum(void);
extern void disp_adc1_harmonic(void);

extern void disp_adc2_current(void);
extern void disp_adc2_spectrum(void);
extern void disp_adc2_harmonic(void);

extern void disp_adc1_2_power(void);

extern void disp_ecap1_frequency(void);
extern void disp_ecap2_phase(void);

extern void disp_status(void);
extern void disp_learn(void);
extern void disp_time(void);
extern void disp_menu(void);

extern void disp_refresh(void);

#endif /* INC_TASKS_TASK_DISP_H_ */
