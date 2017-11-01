/**
 * ADC1_CHANNEL_6 GPIO34
 * ADC1_CHANNEL_7 GPIO35
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "socket.h"

const double VOLTAGE_ADC_VDD         = 1100.0d;
const double CURRENT_ADC_VDD         = 2000.0d;
const double VOLTAGE_DAC_VDD         = 3300.0d;
const double CURRENT_DAC_VDD         = 3.3d;
const double VOLTAGE_ADC_ATTENUATION = 10.0d;
const double CURRENT_ADC_ATTENUATION = 1.0d;
const double VOLTAGE_DAC_ATTENUATION = 4.3d;
const double CURRENT_DAC_ATTENUATION = 1.0d;
//const double DAC2_OFFSET             = 1267.0d;
const double MAX8BIT                 = 255.0d;
const double MAX12BIT                = 4095.0d;

char*  doVoltage(char* cmd);
double getVoltage();
void   setVoltage(double voltage);
char*  doCurrent(char* cmd);
double getCurrent();
void   setCurrent(double current);

esp_err_t event_handler(void* ctx, system_event_t* event){
  if(event->event_id == SYSTEM_EVENT_STA_GOT_IP){
    printf("ipaddress: " IPSTR "\n", IP2STR(&event->event_info.got_ip.ip_info.ip));
  }
  return ESP_OK;
}

void app_main(void){
  //Init ADC
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_6db);//current 2.0v
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_0db);//voltage 1.1v
  //Init DAC
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_enable(DAC_CHANNEL_2);
  dac_output_voltage(DAC_CHANNEL_1, 255);//current
  dac_output_voltage(DAC_CHANNEL_2, 1);//voltage
  //Init TCP
  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  wifi_config_t staConf = {
    .sta = {
      .ssid      = "",
      .password  = "",
      .bssid_set = 0
    }
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &staConf));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_connect());

  struct socket* sock = socket_new();
  sock->sBind(sock);
  sock->sListen(sock);

  struct sockaddr_in clientAddress;
  while(true){
    int fd = sock->sAccept(sock, &clientAddress);
    struct socket* clientSock = socket_new1(fd);
    if(clientSock->fd > -1){
      char* r = clientSock->sRead(clientSock);
      printf("%s\n", r);
      const char voltage[] = "voltage";
      const char current[] = "current";
      char* tmp = r;
      char* cmd = strtok_r(tmp, " ", &tmp);
      char* message;
      if(0 == strcmp(voltage, cmd)){
        message = doVoltage(tmp);
      } else if(0 == strcmp(current, cmd)){
	message = doCurrent(tmp);
      } else {
        const char tmp[] = "bad command\n";
	message = (char*)malloc(strlen(tmp));
	strcpy(message, tmp);
      }
      clientSock->sWrite(clientSock, message);
      free(message);
    }
    clientSock->delete(clientSock);
  }
  sock->delete(sock);
}

char* doVoltage(char* cmd){
  char* tmp  = cmd;
  char* mode = strtok_r(tmp, " ", &tmp);
  if(0 == strcmp("set", mode)){
    double value = atof(strtok_r(tmp, " ", &tmp));
    setVoltage(value);
  }
  double voltage = getVoltage();
  char* result = calloc(25, sizeof(char));
  sprintf(result, "voltage: %f\n", voltage);
  puts(result);
  return result;
}

double getVoltage(){
  //1100 mv / 4095 reading from adc * 4095 no of steps * 10 attenuation / 1000 from mv
  int raw = adc1_get_raw(ADC1_CHANNEL_6);
  printf("voltage get: (%d)", raw);
  return (VOLTAGE_ADC_VDD / MAX12BIT) * raw * VOLTAGE_ADC_ATTENUATION / 1000.0d;
}

void setVoltage(double voltage){
  //((11 voltage * 1000 v to mv) - 1250 mv offset) / 3 attenuation / (3300 mv / 255 no of steps)
  double v = ((voltage * 1000.0d) /*- DAC2_OFFSET*/) / VOLTAGE_DAC_ATTENUATION / (VOLTAGE_DAC_VDD / MAX8BIT);
  if(v > MAX8BIT){
    v = MAX8BIT;
  } else if(v < 0){
    v = 0;
  }
  printf("voltage set: %f(%d)\n", voltage, (int)v);
  dac_output_voltage(DAC_CHANNEL_2, (int)v);
}

char* doCurrent(char* cmd){
  char* tmp = cmd;
  char* mode = strtok_r(tmp, " ", &tmp);
  if(0 == strcmp("set", mode)){
    double value = atof(strtok_r(tmp, " ", &tmp));
    setCurrent(value);
  }
  double current = getCurrent();
  char* result = calloc(25, sizeof(char));
  sprintf(result, "current get: %f\n", current);
  puts(result);
  return result;
}

double getCurrent(){
  //2000 mv / 4095 reading from adc * 4095 no of steps * 1 attenuation / 1000 from mv
  return (CURRENT_ADC_VDD / MAX12BIT) * adc1_get_raw(ADC1_CHANNEL_7) * CURRENT_ADC_ATTENUATION / 1000.0d;
}

void setCurrent(double current){
  double c = current / CURRENT_DAC_VDD * MAX8BIT / CURRENT_DAC_ATTENUATION;
  if(c > MAX8BIT){
    c = MAX8BIT;
  } else if(c < 0){
    c = 0;
  }
  printf("current set: %f(%d)\n", current, (int)c);
  dac_output_voltage(DAC_CHANNEL_1, (int)c);
}
