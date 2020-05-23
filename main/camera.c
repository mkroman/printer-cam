#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_camera.h"
#include "camera.h"
#include "camera_boards.h"

static const char* TAG = "camera";

static camera_config_t s_camera_config = {
    .pin_pwdn  = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20e6,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,//YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_HD, //QQVGA-QXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 3, //0-63 lower number means higher quality
    .fb_count = 1 //if more than one, i2s runs in continuous mode. Use only with JPEG
};

esp_err_t printer_camera_init() {
    ESP_LOGI(TAG, "Initializing camera");

// Some camera modules needs to be powered up, if there's a defined power
// pin, then toggle it high
#if CAM_PIN_PWDN != -1
    gpio_config_t io_conf;

    ESP_LOGD(TAG, "Pulling CAM_PIN_PWDN (%d) high", CAM_PIN_PWDN);

    uint64_t pin_mask = (1ULL << CAM_PIN_PWDN);

    // The esp32-cam also has a flash pin that we'll want to configure as output
#  if CONFIG_CAMERA_BOARD_SEL == CAMERA_BOARD_AITHINKER_ESP32_CAM
    pin_mask |= (1ULL << GPIO_NUM_4);
#  endif

    // Disable interrupts
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // Set the pin mask to be the defined pin number
    io_conf.pin_bit_mask = pin_mask; 
    // Set pin to output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // Disable pull-up and pull-down resistors
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(CAM_PIN_PWDN, 0));
#endif

    // Initialize the camera
    esp_err_t err = esp_camera_init(&s_camera_config);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize camera");

        return err;
    } else {
        sensor_t* camera_sensor = esp_camera_sensor_get();

        ESP_LOGI(TAG, "Initialized camera with sensor %d", camera_sensor->id.PID);
    }

    return ESP_OK;
}

esp_err_t printer_camera_deinit() {
    return esp_camera_deinit();
}
