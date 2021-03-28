#ifndef __VMA311_H__
#define __VMA311_H__

#include "driver/gpio.h"

/* type definitions */
typedef enum vma311_status
{
    VMA311_CRC_ERROR = -2,
    VMA311_TIMEOUT_ERROR,
    VMA311_OK
} vma311_status_t;

typedef struct vma311_data
{
    vma311_status_t status;
    int8_t rh_int;
    int8_t rh_dec;
    int8_t t_int;
    int8_t t_dec;
} vma311_data_t;

typedef struct vma311
{
    gpio_num_t num;
    int64_t last_read_time;
    vma311_data_t data;
} vma311_t;

/* function prototypes */
void          vma311_init(gpio_num_t);
vma311_data_t vma311_get_values();

#endif /* __VMA311_H__ */
