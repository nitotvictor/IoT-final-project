#include <stdio.h>
#include "freertos/FreeRTOS.h" //sets configuration required to run freeRTOS on ESP32
#include "freertos/task.h" //provides the multitasking functionality
#include "sdkconfig.h" //make sdkconfig options available to the project build system and source files
#include "led.h"
#include "mcp9700.h"
#include "vma311.h"
#include "bme680.h"
#include "wifi.h"
#include "aio.h"
#include "mqtt.h"
#include "bme680.h"


#define BUILTIN_LED_GPIO GPIO_NUM_2 //GPIO 2 assigned to LED
#define MCP9700_ADC_UNIT ADC_UNIT_1 //ADC1 for MCP9700
#define MCP9700_ADC_CHANNEL ADC_CHANNEL_4 //Channel 4 for MCP9700
#define VMA311_GPIO GPIO_NUM_5 //GPIO 5 assigned to VMA311


void app_main()
{
    //Create variables for each sensor
    uint32_t mcp_temp;
    char mcp9700_temperature[8];
    char vma311_temperature[8];
    char vma311_humidity[8];
    char bme680_humidity[8];
    char bme680_pressure[8];
    char bme680_gas_resistance[8];
    char bme680_temperature[8];

    vma311_data_t vma311_data;
    struct bme680_dev bme;
    struct bme680_field_data bme_data;
  
    

    /* Device initialization */
    
    //Wi-Fi connection
    wifi_init("Freebox-A28900", "condida-sospitatis6-gemellorum-emoti8"); //wifi connection
    //wifi_init("Raspberry", "esilv-evd21");
    //wifi_init("iPhone", "azertyazerty");
    
    //Adafruit.io initialization
    aio_init("victornitot","aio_wSii70UyFJTrweGsyyK4X33loIpq"); //adafruit
    aio_create_group("envmon");
    aio_create_feed("mcp9700","envmon");
    aio_create_feed("vma311","envmon");
    
    //Mqtt broker initialization
    mqtt_init("mqtts://@iot.devinci.online", "vn170735", "%%@s5$ZQ");  //parameters are : protocol, host name, username & password
    
    //Sensors initialization
    mcp9700_init(MCP9700_ADC_UNIT, MCP9700_ADC_CHANNEL); //mcp9700 init
    vma311_init(VMA311_GPIO); //vma311 init
    bme.intf = BME680_I2C_INTF;
    bme680_init(&bme); //bme680 init

    

    while (1)
    {
        /* Data collection, printing, MQTT publishing, adafruit publishing*/

                                    /*MCP9700*/
        
        //get value
        mcp_temp = mcp9700_get_value();
        
        //print to console
        printf("mcp9700:temp:%d\n", mcp_temp);
        
        //convert the int into a char
        sprintf(mcp9700_temperature, "%d", mcp_temp);
        
        //publish to adafruit io
        aio_create_data(mcp9700_temperature, "envmon.mcp9700");
        
        //publish to mqtt borker
        mqtt_publish("vn170735/mcp9700/temp",mcp9700_temperature);


                             /* VMA311 (DHT11) */
        
        vma311_data = vma311_get_values();
        
        //Convert the int into a char
        sprintf(vma311_temperature,"%d", vma311_data.t_int);
        sprintf(vma311_humidity,"%d", vma311_data.rh_int);
        
        //print to console
        if (vma311_data.status == VMA311_OK) //if no time out error absurdity 
        {
            printf("vma311:temp:%d.%d\n", vma311_data.t_int, vma311_data.t_dec);
            printf("vma311:humidity:%d.%d\n", vma311_data.rh_int, vma311_data.rh_dec);
        }
        else
        {
            printf("vma311:error\n");
        }
        
        //Publish on adafruit
        aio_create_data(vma311_temperature, "envmon.vma311-temp");
        aio_create_data(vma311_humidity, "envmon.vma311-humidity");
        
        //Publish on MQTT broker
        mqtt_publish("vn170735/vma311/temp",vma311_temperature);
        mqtt_publish("vn170735/vma311/humidity",vma311_humidity);


                            /*BME680*/
        
        bme680_get_sensor_data(&bme_data, &bme); //get values
        
        //Print to console
        printf("bme680:temp:%d\n", bme_data.temperature);
        printf("bme680:humidity:%d\n", bme_data.humidity);
        printf("bme680:pressure:%d\n", bme_data.pressure);
        printf("bme680:gas_resistance:%d\n", bme_data.gas_resistance);
        
        //Convert the int into a char
        sprintf(bme680_temperature,"%d", bme_data.temperature); 
        sprintf(bme680_humidity,"%d", bme_data.humidity);
        sprintf(bme680_pressure,"%d", bme_data.pressure);
        sprintf(bme680_gas_resistance,"%d", bme_data.gas_resistance);
        
        //Publish on adafruit
        aio_create_data(bme680_humidity, "envmon.bme680-temp");
        aio_create_data(bme680_pressure, "envmon.bme680-humidity");
        aio_create_data(bme680_gas_resistance, "envmon.bme680-pressure");
        aio_create_data(bme680_temperature, "envmon.bme680-gas_resistance");
        
        //Publish on MQTT broker
        mqtt_publish("vn170735/bme680/temp",bme680_temperature);
        mqtt_publish("vn170735/bme680/humidity",bme680_humidity);
        mqtt_publish("vn170735/bme680/pressure",bme680_pressure);
        mqtt_publish("vn170735/bme680/gas_resistance",bme680_gas_resistance);
        
     
        vTaskDelay(5000 / portTICK_PERIOD_MS); // 5 sec delay
    }
}
