/*
 * task_disp.h
 *
 *  Created on: 2017-6-27
 *      Author: redchenjs
 */

#ifndef INC_TASKS_TASK_DISP_H_
#define INC_TASKS_TASK_DISP_H_

#define MENU_ITEM_NUM   4

enum menu_level {
    MENU_LEVEL_1   = 0x0,
    MENU_LEVEL_2   = 0x1
};

enum menu_item {
    MENU_ITEM_VOLTAGE   = 0x0,
    MENU_ITEM_SPECTRUM  = 0x1,
    MENU_ITEM_HARMONIC  = 0x2,
    MENU_ITEM_FREQUENCY = 0x3
};

extern void task_disp(void);

#endif /* INC_TASKS_TASK_DISP_H_ */
