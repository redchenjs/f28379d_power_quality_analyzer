/*
 * task_status.h
 *
 *  Created on: 2017年8月10日
 *      Author: redchenjs
 */

#ifndef INC_TASKS_TASK_STATUS_H_
#define INC_TASKS_TASK_STATUS_H_

#include "stdint.h"

typedef enum elec_status {
    OFF = 0,
    ON  = 1,
    S1  = 2,
    S2  = 3,
    S3  = 4
} elec_status_t;

typedef struct {
    uint16_t status;
    uint16_t count_on;
    uint16_t count_off;
    double voltage;
    double current_max;
    double current_min;
    double phase;
    double frequency;
    double duty;
    double harmonic_1;
    double harmonic_2;
    double harmonic_3;
    double harmonic_4;
    double harmonic_5;
    double harmonic_6;
    double harmonic_7;
    double harmonic_8;
    double harmonic_9;
} elec_param_t;

typedef struct {
    double voltage;
    double current;
    double phase;
    double frequency;
    double duty;
    double apparent_power;
    double active_power;
    double reactive_power;
    double power_factor;
} power_param_t;

typedef struct {
    double frequency;
    double dc;
    double harmonic_1;
    double harmonic_2;
    double harmonic_3;
    double harmonic_4;
    double harmonic_5;
    double harmonic_6;
    double harmonic_7;
    double harmonic_8;
    double harmonic_9;
} harmonic_param_t;

extern void status_update(void);
extern void status_learn(void);
extern void status_view(void);
extern void status_clear(void);
extern void status_init(void);

#endif /* INC_TASKS_TASK_STATUS_H_ */
