#include <esp_log.h>
#include "esp_http_server.h"
#include "web_server.h"

static const char* TAG = "web_server";

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace; boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

esp_err_t jpeg_stream_handler(httpd_req_t* req) {
    uint8_t*     img_buf = NULL;
    size_t       img_buf_len = 0;
    esp_err_t    res = ESP_OK;
    camera_fb_t* fb = NULL;
    char*        header_buf[66] = {0};

    // Set the response content type
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);

    if (res != ESP_OK) {
        return res;
    }

    // Turn on the LED "flash"
#if CONFIG_CAMERA_BOARD_SEL == CAMERA_BOARD_AITHINKER_ESP32_CAM
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_4, 1));
#endif

    while (true) {
        // Get a copy of the framebuffer
        fb = esp_camera_fb_get();

        if (!fb) {
            ESP_LOGE(TAG, "Failed to acquire camera framebuffer");
            res = ESP_FAIL;
            break;
        }

        if (fb->format == PIXFORMAT_JPEG) {
            img_buf = fb->buf;
            img_buf_len = fb->len;
        } else {
            bool success = frame2jpg(fb, 12, &img_buf, &img_buf_len);

            if (!success) {
                ESP_LOGE(TAG, "Failed to compress frame buffer to jpeg");
                res = ESP_FAIL;
            }
        }

        if (res == ESP_OK) {
            // Send the multipart boundary
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }

        // Send the multipart content-type and -length headers
        if (res == ESP_OK) {
            int header_len = snprintf((char*)header_buf, 66, _STREAM_PART, img_buf_len);

            res = httpd_resp_send_chunk(req, (const char*)header_buf,  header_len);
        }

        // Send the image buffer
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char*)img_buf, img_buf_len);
        }

        if (fb->format != PIXFORMAT_JPEG) {
            free(img_buf);
        }

        // Return the framebuffer
        esp_camera_fb_return(fb);

        if (res != ESP_OK) {
            break;
        }
    }

    // Turn off the LED "flash"
#  if CONFIG_CAMERA_BOARD_SEL == CAMERA_BOARD_AITHINKER_ESP32_CAM
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_4, 0));
#  endif

    return ESP_OK;
}

// Called when the client requests a non-existing resource
esp_err_t not_found_handler(httpd_req_t* req, httpd_err_code_t code) {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "henlo friend, unfortunately resource not found :(");

    return ESP_FAIL;
}

static const httpd_uri_t jpeg_stream = {
  .uri = "/jpeg",
  .method = HTTP_GET,
  .handler = jpeg_stream_handler,
  .user_ctx = NULL
};

httpd_handle_t start_web_server() {
  httpd_handle_t handle = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  esp_err_t ret = 0;

  ESP_LOGI(TAG, "Starting http server");

  if ((ret = httpd_start(&handle, &config)) != ESP_OK) {
    ESP_LOGE(TAG, "Error starting httpd");
    ESP_LOGE(TAG, "Error: %d", ret);

    return NULL;
  }

  httpd_register_uri_handler(handle, &jpeg_stream);
  // Register a handler for when the client requests a non-existing resource
  httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, not_found_handler);

  return handle;
}

esp_err_t stop_web_server(httpd_handle_t handle) {
  return httpd_stop(handle);
}
