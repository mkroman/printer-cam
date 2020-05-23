#ifndef _CAMERA_H
#define _CAMERA_H

#include <esp_err.h>

// Initializes the camera subsystem
esp_err_t printer_camera_init();

// Deinitializes and frees resources related to the camera subsystem
esp_err_t printer_camera_deinit();

#endif

