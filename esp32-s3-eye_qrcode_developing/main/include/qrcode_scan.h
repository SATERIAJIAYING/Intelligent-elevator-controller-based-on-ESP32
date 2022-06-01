#ifndef QRCODE_H
#define QRCODE_H

#include "esp_camera.h"
#include "esp_http_client.h"

int baidu_img_ai();
static esp_err_t img_http_event_handler(esp_http_client_event_t *evt);
void scanner_task(void*);
#endif