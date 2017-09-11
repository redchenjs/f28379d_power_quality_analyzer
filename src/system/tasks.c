/*
 * tasks.c
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */
#include <inc/tasks/task_status.h>
#include <inc/tasks/task_menu.h>
#include <inc/tasks/task_sci.h>

void tasks_loop(void)
{
    menu_run();
}

void tasks_init(void)
{
    status_init();
//    status_clear();
}
