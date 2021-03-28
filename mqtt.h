#ifndef __MQTT_H__
#define __MQTT_H__

#include "mqtt_client.h"

typedef struct mqtt
{
    esp_mqtt_client_handle_t client;
} mqtt_t;

void mqtt_init(const char *, const char *, const char *);
void mqtt_publish(const char *, const char *);

#endif /* __MQTT_H__ */
