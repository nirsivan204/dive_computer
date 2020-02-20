/**
 * @file adc_task.c
 *
 * @brief Holds logic for the ADC sampling task.
 */
#include <stdint.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/drivers/ADC.h>
#include "main.h"
#include "Board.h"
#include "bsp.h"
#include "alarm.h"
#include "adc_task.h"
#include "common.h"

// joystick ADC values for alarm limits
#define HIGH_ALARM_LIMIT          (800)
#define MEDIUM_ALARM_LIMIT        (550)
#define LOW_ALARM_LIMIT           (400)

// sample the joystick every 125 ticks
#define ADC_INTERVAL_TICKS        (125)

// ADC handle to the joystick
static ADC_Handle g_adc_handle;

/**
 *  SYS/BIOS task. Sample the Y-axis of the joystick.
 *
 *  @param[in] task_arg0 unused, argument from SYS/BIOS task create
 *  @param[in] task_arg1 unused, argument from SYS/BIOS task create
 */
/*lint -e{715} symbol not referenced */
void adc_task(uint32_t arg0, uint32_t arg1)
{
    ADC_Params  params;
    int_fast16_t res;
    adc_msg_t msg;
    uint16_t sample;

    (void)arg0;    // NOTE: Silence compiler warning about unused param.
    (void)arg1;

    // create the ADC channel to sample the joystick
    ADC_Params_init(&params);
    g_adc_handle = ADC_open(Board_ADC_JOYSTICK_Y, &params);
    SYS_ASSERT(g_adc_handle != NULL);

    for (;;)
    {
        // Take an ADC reading of the joystick. This is a blocking call...
        res = ADC_convert(g_adc_handle, &sample);

        if(ADC_STATUS_SUCCESS == res)
        {
            // reduce the range from 12-bit to 10-bit.
            sample = sample >> 2;

            // post the sample to the display task
            msg.sample = sample;
            (void) Mailbox_post(g_adc_mbox, &msg, BIOS_WAIT_FOREVER);

            // analyze the joystick position and determine what type of alarm
            // event to send
            if(sample > HIGH_ALARM_LIMIT)
            {
                Event_post(g_alarm_event_handle, ALARM_HIGH);
            }
            else if(sample > MEDIUM_ALARM_LIMIT)
            {
                Event_post(g_alarm_event_handle, ALARM_MEDIUM);
            }
            else if(sample < LOW_ALARM_LIMIT)
            {
                Event_post(g_alarm_event_handle, ALARM_LOW);
            }
            else
            {
                // turn the alarm off when the joystick is near its
                // resting position
                Event_post(g_alarm_event_handle, ALARM_NONE);
            }
        }

        // pause before next sample
        Task_sleep(ADC_INTERVAL_TICKS);
    }
}
