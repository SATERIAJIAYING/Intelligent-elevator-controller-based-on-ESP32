#include <stdio.h>
#include "qrcode_scan.h"
#include "base64.h"
#include "urlcode.h"
#include "esp_camera.h"
#include "esp_err.h"
#include "fb_gfx.h"
#include "img_converters.h"
#include "esp_http_client.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "app_camera.hpp"



int baidu_img_ai(AppCamera* self)
{
    static const char* TAG = "baidu";

	char *base64_buf = NULL;
	char *urlcode_buf = NULL;
	camera_fb_t *fb = NULL;
	esp_err_t res = ESP_OK;
	size_t _jpg_buf_len = 0;
	uint8_t *_jpg_buf = NULL;
	char *url_buf = malloc(sizeof(char) * 512);
	ESP_LOGE(TAG, "baidu Start");
	ESP_LOGE(TAG, "Taking picture...");
 
	if (!self->pic)
	{
		ESP_LOGE(TAG, "esp_camera_fb_get failed...");
		esp_camera_deinit();
		free(url_buf);
		url_buf = NULL;
		return ESP_FAIL;
	}
	if (!fmt2jpg(pic->buf, pic->width * pic->height * 3, pic->width, pic->height, PIXFORMAT_RGB565, 80, &_jpg_buf, &_jpg_buf_len))
	{
		ESP_LOGE(TAG, "fmt2jpg failed");
		return ESP_FAIL;
	}
	ESP_LOGE(TAG, "Picture taken! Its size was: %zu bytes", _jpg_buf_len);
	/*                       base64编码                                   */
	base64_buf = base64_encode(_jpg_buf, _jpg_buf_len);
	ESP_LOGE(TAG, "base64_encode OK,size: %d", strlen(base64_buf));
	urlcode_buf = malloc(sizeof(char) * strlen(base64_buf) * 2);
	URLEncode(base64_buf, strlen(base64_buf), urlcode_buf, strlen(base64_buf) * 2);
	ESP_LOGE(TAG, "urlencode OK,size: %d", strlen(urlcode_buf));
	/*                       开始上传百度AI                         */
	esp_http_client_config_t config = {
		.url = "https://aip.baidubce.com/rest/2.0/ocr/v1/train_ticket?access_token=24.0faf0b824d3a9a426778d98ec74d8280.2592000.1656397553.282335-26339198",
		.buffer_size = 4 * 1024,
		.timeout_ms = 4000,
        .method = HTTP_METHOD_POST
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_method(client, HTTP_METHOD_POST);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
	esp_http_client_set_post_field(client, urlcode_buf, strlen(urlcode_buf));
 
	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP POST Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		free(url_buf);
		url_buf = NULL;
		// esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	else
	{
		ESP_LOGE(TAG, "HTTP POST request failed: %d", err);
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		free(url_buf);
		url_buf = NULL;
 
		// esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	return res;
}

