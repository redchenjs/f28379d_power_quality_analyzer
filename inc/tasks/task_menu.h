/*
 * task_menu.h
 *
 *  Created on: 2017-7-25
 *      Author: redchenjs
 */

#ifndef INC_TASKS_TASK_MENU_H_
#define INC_TASKS_TASK_MENU_H_

enum menu_level_t {
    MENU_LEVEL_1   = 0x0,
    MENU_LEVEL_2   = 0x1
};

extern void menu_run(void);

#endif /* INC_TASKS_TASK_MENU_H_ */
