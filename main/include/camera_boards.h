#ifndef _CAMERA_BOARDS_H
#define _CAMERA_BOARDS_H

#if CONFIG_CAMERA_BOARD_SEL == CAMERA_BOARD_AITHINKER_ESP32_CAM
// Ai-Thinker ESP32-CAM pin mapping
// GPIO32 needs to be pulled low or the camera will reset and block i2c
// See https://github.com/espressif/esp32-camera/issues/66#issuecomment-526283681
#  define CAM_PIN_PWDN  32
#  define CAM_PIN_RESET -1 //software reset will be performed
#  define CAM_PIN_XCLK  0
#  define CAM_PIN_SIOD  26
#  define CAM_PIN_SIOC  27

#  define CAM_PIN_D7    35
#  define CAM_PIN_D6    34
#  define CAM_PIN_D5    39
#  define CAM_PIN_D4    36
#  define CAM_PIN_D3    21
#  define CAM_PIN_D2    19
#  define CAM_PIN_D1    18
#  define CAM_PIN_D0     5
#  define CAM_PIN_VSYNC 25
#  define CAM_PIN_HREF  23
#  define CAM_PIN_PCLK  22
#elif CONFIG_CAMERA_BOARD_SEL == CAMERA_BOARD_ESPRESSIF_WROVER_KIT
// Espressif WROVER-KIT pin mapping
#  define CAM_PIN_PWDN  -1 //power down is not used
#  define CAM_PIN_RESET -1 //software reset will be performed
#  define CAM_PIN_XCLK  21
#  define CAM_PIN_SIOD  26
#  define CAM_PIN_SIOC  27

#  define CAM_PIN_D7    35
#  define CAM_PIN_D6    34
#  define CAM_PIN_D5    39
#  define CAM_PIN_D4    36
#  define CAM_PIN_D3    19
#  define CAM_PIN_D2    18
#  define CAM_PIN_D1     5
#  define CAM_PIN_D0     4
#  define CAM_PIN_VSYNC 25
#  define CAM_PIN_HREF  23
#  define CAM_PIN_PCLK  22
#else
#error "Please specify a board in the configuration"
#endif

#endif

