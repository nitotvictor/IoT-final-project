#ifndef __VMA311_H__
#define __VMA311_H__

#include "driver/gpio.h"

// type definitions
typedef enum vma311_status
{
    VMA311_CRC_ERROR = -2,
    VMA311_TIMEOUT_ERROR,
    VMA311_OK
} vma311_status_t;

typedef struct vma311_data //Defining the type of variables (int and decimals) with int8_t
{
    vma311_status_t status;
    int8_t rh_int; //int humidity
    int8_t rh_dec; //decimal humidity
    int8_t t_int; //int temp
    int8_t t_dec; //decimal temp
} vma311_data_t;

typedef struct vma311 //to define which gpio is linked and collect the values 
{
    gpio_num_t num;
    int64_t last_read_time;
    vma311_data_t data;
} vma311_t;

//function prototypes 
void          vma311_init(gpio_num_t); //to set the gpio linked
vma311_data_t vma311_get_values(); //get values that we will use in the main function

#endif
