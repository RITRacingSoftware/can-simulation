#include "main.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "can.h"
#include "clock.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "error_handler.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stm32g4xx_hal.h>

// TODO fix makefile imports for dbc include
#include <formula_main_dbc.h>

// TODO
/*
 * 1. Make a RTOS task to continously send fake can data onto the RTOS queue
 * 2. Make a function to get
 *
* /

/*
 * TODO setup CAN event loop
 * TODO send data through TJA1042 transmitter and probe data
 * Procedure
 * 1. Send random CAN data onto the queue
 * 2. Decode CAN data and attach ID
 * CanMessage_s reciever;
 * core_CAN_receive_from_queue(FDCAN1, &reciever);
 * USART SEND reciever.id, + reciever.dlc, + reciever.data
 */

void heartbeat_task(void *pvParameters) {
    (void) pvParameters;
    while(true) {
        core_GPIO_toggle_heartbeat();
        vTaskDelay(1000 * portTICK_PERIOD_MS);
    }
}

uint16_t test_unpack(void) {

    // TYRE NODE
    struct formula_main_dbc_c70_tire_temps_t server_temps;
    CanMessage_s server_msg; // Instantiate can message buffer
                             // temps -> (uint8_t*)client_msg.data
    // 90c
    server_temps.tire_temp_fl_max = 90;
    server_temps.tire_temp_fr_max = 90;
    server_temps.tire_temp_rl_max = 90;
    server_temps.tire_temp_rr_max = 90;

    int sz = formula_main_dbc_c70_tire_temps_pack((uint8_t*)server_msg.data, &server_temps, 8);

    // TJA1042 CAN Transmitter
    core_CAN_add_message_to_tx_queue(FDCAN1, 1874, sz, &server_msg.data);

    CanMessage_s client_msg;
    core_CAN_receive_from_queue(FDCAN1, &client_msg);


    struct formula_main_dbc_c70_tire_temps_t client_temps;
    formula_main_dbc_c70_tire_temps_unpack(&client_temps, client_msg.data, client_msg.dlc);


    return client_temps.tire_temp_rl_max;

}

void can_read_task(void *pvParameters) {
    (void) pvParameters;
    while(true) {
        test_unpack();
    }
}

// void can_transmit_task(void *pvParameters) {
//     (void) pvParameters;
//     while(true) {
//
//     }
// }

int main(void) {
    HAL_Init();

    core_GPIO_set_heartbeat(GPIO_PIN_RESET);

    if (!core_clock_init()) error_handler();

    if (!core_CAN_init(FDCAN1)) error_handler();

    /*
     * Initialize a new task on in the FreeRTOS runtime
     * https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate
     */
    int err = xTaskCreate(
        can_read_task, /* specified task handler */
        "can_test_task", /* task name */
        1000, /* allocated stack memory space for task */
        NULL, /* parameters passed into task (none for now) */
        1, /* RTOS Task CPU Scheduling Task Priority */
        NULL /* closure for pre task running */
    );
    // Check to see if the RTOS task was successfully created
    if (err != pdPASS) {
        error_handler();
    }

    err = xTaskCreate(heartbeat_task,
        "heartbeat", /* specified task handler */
        1000, /* task name */
        NULL, /* parameters passed into task (none for now) */
        4,  /* RTOS Task CPU Scheduling Task Priority */
        NULL /* closure for pre task running */
        );

    // Check to see if the RTOS task was successfully created
    if (err != pdPASS) {
        error_handler();
    }

    // hand control over to FreeRTOS
    vTaskStartScheduler();

    // we should not get here ever
    error_handler();
    return 1;
}

// Called when stack overflows from rtos
// Not needed in header, since included in FreeRTOS-Kernel/include/task.h
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName) {
    (void) xTask;
    (void) pcTaskName;

    error_handler();
}
