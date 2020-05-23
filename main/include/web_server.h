#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <esp_err.h>
#include <esp_camera.h>
#include <esp_http_server.h>

// Stops the web server
//
// This is a blocking operation
extern esp_err_t stop_web_server(httpd_handle_t handle);

// Starts the web server and returns a handle to it
extern httpd_handle_t start_web_server();

// Starts the web server and returns a handle to it
extern esp_err_t stop_web_server(httpd_handle_t handle);

#endif

