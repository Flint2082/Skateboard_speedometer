#ifndef SIGPROC_H
#define SIGPROC_H

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "adc.h"
#include "main.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"
#include "driver/gpio.h"

#define KERNEL_SIZE 9  // kernel size for convolution
#define SCHMITT_TRIGGER 10 // schmitt trigger threshold

#define SCHMITT_HIGH 1 // schmitt trigger high to low transition
#define SCHMITT_LOW 0 // schmitt trigger low to high transition

void analog_to_freq_conversion_task(void* parameters); // function to convert the analog signal to frequency

#endif // SIGPROC_H


