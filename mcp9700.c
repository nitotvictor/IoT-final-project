#include <stdlib.h>
#include "mcp9700.h"
#include "freertos/FreeRTOS.h" //sets configuration required to run freeRTOS on ESP32
#include "freertos/task.h" //provides the multitasking functionality

static esp_adc_cal_characteristics_t *adc_chars; //points to esp_adc_cal_characterize which is ADC characterisitics storage place.
static mcp9700_t mcp9700;//instance of mcp9700 object

void mcp9700_init(adc_unit_t unit, adc_channel_t channel)
{
    if (unit == ADC_UNIT_1) 
    {
        adc1_config_width(ADC_WIDTH); //to set the width of ADC1 
        adc1_config_channel_atten(channel, ADC_ATTEN); //attenuation of ADC1 to give full scale voltage of 1,1V
    }
    else
    {
        adc2_config_channel_atten(channel, ADC_ATTEN); //mcp9700.unit == ADC_UNIT_2
        adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t)); //to allocate memory
        esp_adc_cal_characterize(unit, ADC_ATTEN, ADC_WIDTH, DEFAULT_VREF, adc_chars); //in function of the ADC, we store the voltage value to the pointed address
        mcp9700.unit = unit; //assigning parameter values
        mcp9700.channel = channel;
        mcp9700.adc_chars = *adc_chars;
}

int32_t mcp9700_get_value()
{
    uint32_t adc_reading = 0;
    uint32_t voltage; //creating the voltage variable to calculate temperature
    uint32_t temperature; //creating the temperature variable which will be returned
    for (int i = 0; i < NO_OF_SAMPLES; i++) //64 samples as defined in mcp9700.h
    {
        if (mcp9700.unit == ADC_UNIT_1)
        {
            adc_reading += adc1_get_raw(mcp9700.channel); //summing the value 
        }
        else /* mcp9700.unit == ADC_UNIT_2 */ //if the ADC used is ADC2
        {
            int raw;
            adc2_get_raw(mcp9700.channel, ADC_WIDTH, &raw);
            adc_reading += raw; //raw variable is made to store the values
        }
    }
    adc_reading /= NO_OF_SAMPLES; //we make an average of the adc value by dividing by number of samples
    voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    voltage = voltage - 500; //formula to have the voltage
    temperature = voltage/10; //temperature with the voltage
    return temperature;
}
