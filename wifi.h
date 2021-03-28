#ifndef __WIFI_H__
#define __WIFI_H__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

void wifi_init(const char *, const char *);

#endif /* __WIFI_H__ */
