#include "vma311.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static vma311_t vma311; //instance of a vma311 struct

static void vma311_send_start_signal();
static int vma311_wait(uint16_t, int);
static int vma311_check_response();
static inline vma311_status_t vma311_read_byte(uint8_t *);
static vma311_status_t vma311_check_crc(uint8_t *);

void vma311_init(gpio_num_t num) //initializes the gpio
{
    vma311.num = num; //assign the pin value
    vma311.last_read_time = -2000000;
    gpio_reset_pin(num);
    vTaskDelay(pdMS_TO_TICKS(1000)); //waiting for 1 sec 
}

vma311_data_t vma311_get_values() //get_values() called in the main 
{
    vma311_data_t error_data = {VMA311_TIMEOUT_ERROR, -1, -1, -1, -1}; //if there is an error, return -1 in the array
    uint8_t data[5] = {0, 0, 0, 0, 0}; //the vma has 5 bytes so we have an 5 dim array to store the values
    if (esp_timer_get_time() - 2000000 < vma311.last_read_time)
    {
        return vma311.data; //last measure
    }
    vma311.last_read_time = esp_timer_get_time(); //sets the time of the last reading to the beginning of this reading
    vma311_send_start_signal(); //activates the sensor 
    if (vma311_check_response() == VMA311_TIMEOUT_ERROR)
    {
        return error_data; //When it takes too long, return an error
    }
    
    for (int i = 0; i < 5; ++i)
    {
        if (vma311_read_byte(&data[i]))//this function converts the bits sent by the sensor to a digital value
        {
            return error_data;
        }
    }
    
    if (vma311_check_crc(data) != VMA311_CRC_ERROR)// if there's no transmission error, then the data sent by the sensor is correct and we assign them to the sensor object
    {
        vma311.data.rh_int = data[0];
        vma311.data.rh_dec = data[1];
        vma311.data.t_int = data[2];
        vma311.data.t_dec = data[3];
        vma311.data.status = VMA311_OK;
    }
    return vma311.data; //Returning humidity and temperature
}

void vma311_send_start_signal()// this code indicates the different steps for sending a start signal to the sensor, so it returns data
{
    gpio_set_direction(vma311.num, GPIO_MODE_OUTPUT); //set GPIO as an output
    gpio_set_level(vma311.num, 0); //gpio set to 0
    ets_delay_us(20 * 1000);
    gpio_set_level(vma311.num, 1); //gpio set to 1
    ets_delay_us(40);
}

int vma311_wait(uint16_t us, int level)
{
    int us_ticks = 0;
    while (gpio_get_level(vma311.num) == level) //if the level in parameter corresponds to the previous level 
    {
        if (us_ticks++ > us) 
        {
            return VMA311_TIMEOUT_ERROR; //time too long
        }
        ets_delay_us(1);
    }
    return us_ticks;
}

int vma311_check_response()
{
    gpio_set_direction(vma311.num, GPIO_MODE_INPUT); // set gpio as an input
    if (vma311_wait(81, 0) == VMA311_TIMEOUT_ERROR || vma311_wait(81, 1) == VMA311_TIMEOUT_ERROR) // if it stays on 0 or 1 for more than 80 ms, it will return an error
    {
        return VMA311_TIMEOUT_ERROR;
    }
    return VMA311_OK;
}

vma311_status_t vma311_read_byte(uint8_t *byte) // this function reads one byte (there are 5 bytes in total)
{
    for (int i = 0; i < 8; ++i)
    {
        if (vma311_wait(50, 0) == VMA311_TIMEOUT_ERROR)
        {
            return VMA311_TIMEOUT_ERROR;
        }
        if (vma311_wait(70, 1) > 28)
        {
            *byte |= (1 << (7 - i)); //the sensor sends the highest bit first
        }                            
    }
    return VMA311_OK;
}

vma311_status_t vma311_check_crc(uint8_t data[])
{
    int sum = 0;
    for (int i = 0; i < 4; ++i) 
    {
        sum += data[i]; //summing each value of the sensor
    }
    if (sum != data[4])
    {
        return VMA311_CRC_ERROR; //error because the sum of the sensor values is differenet 
    }
    else
    {
        return VMA311_OK;
    }
}
