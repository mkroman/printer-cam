#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"

#include "camera.h"
#include "web_server.h"

// FreeRTOS event group to signal when we are connected
static EventGroupHandle_t s_wifi_event_group;

static const char* TAG = "main";
static size_t s_retry_num = 0;

const int WIFI_CONNECTED_BIT = BIT0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    httpd_handle_t web_server_handle;

    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "got ipv4: %s",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

            web_server_handle = start_web_server();

            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            {
                if (s_retry_num < 3) {
                    esp_wifi_connect();
                    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                    s_retry_num++;
                    ESP_LOGI(TAG,"retry to connect to the AP");
                }
                ESP_LOGI(TAG,"connect to the AP fail\n");
                stop_web_server(&web_server_handle);
                break;
            }
        default:
            break;
    }
    return ESP_OK;
}

void app_main()
{
    s_wifi_event_group = xEventGroupCreate();

    // Initialize LWIP
    tcpip_adapter_init();

    // Set up our WiFI handling event loop
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    // Initialize the non-volatile storage
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize the camera and make sure it's functional
    ESP_ERROR_CHECK(printer_camera_init());

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Connect to the configured WiFI SSID
    ESP_LOGI(TAG, "Connecting to AP %s", CONFIG_WIFI_SSID);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASS
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config))
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Hello world");
    //ESP_ERROR_CHECK(printer_camera_deinit());
}
