/*
 * main.c
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */
#include "inc/system/init.h"
#include "inc/system/tasks.h"

int main(void)
{
    device_init();           // 器件初始化，包括器件的配置，功能模块配置
    driver_init();           // 外设驱动初始化

    tasks_init();            // 任务初始化

    while (1) {
        tasks_loop();        // 任务主循环

        device_suspend();    // 器件进入休眠模式
    }
}
