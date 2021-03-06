#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "socket.h"

esp_err_t event_handler(void* ctx, system_event_t* event){
  if(event->event_id == SYSTEM_EVENT_STA_GOT_IP){
    printf("ipaddress: " IPSTR "\n", IP2STR(&event->event_info.got_ip.ip_info.ip));
  }
  return ESP_OK;
}

void app_main(void){
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
    struct socket* clientSock = socket_new1(sock->sAccept(sock, &clientAddress));
    if(clientSock->fd > -1){
      char* r = clientSock->sRead(clientSock);
      printf("%s\n", r);
    }
    clientSock->delete(clientSock);
  }
  sock->delete(sock);
}
