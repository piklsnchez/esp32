/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_system.h"
#include "esp_adc_cal.h"

#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_6)      //GPIO 34

void app_main(void){
    //Init ADC
    adc1_config_width(ADC_WIDTH_9Bit);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_6db);
    //Init DAC
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_enable(DAC_CHANNEL_2);
    dac_output_voltage(DAC_CHANNEL_2, 255);
    uint32_t voltage;
    while(1){
        voltage = adc1_get_raw(ADC1_TEST_CHANNEL);
	//printf("%d mV\n",voltage);
	dac_output_voltage(DAC_CHANNEL_1, voltage/2);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
