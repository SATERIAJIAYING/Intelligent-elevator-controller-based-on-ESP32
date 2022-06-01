#include <stdio.h>
#include "qrcode_scan.h"
#include "base64.h"
#include "urlcode.h"
#include "esp_camera.h"
#include "esp_err.h"
#include "fb_gfx.h"
#include "img_converters.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"
#include "cJSON.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "esp_tls.h"
#include "lwip/dns.h"


int baidu_img_ai()
{
    static const char* TAG = "baidu";

	char *base64_buf = NULL;
	char *urlcode_buf = NULL;
	esp_err_t res = ESP_OK;
	size_t _jpg_buf_len = 0;
	uint8_t *_jpg_buf = NULL;
	char local_response_buffer[4096] = {0};
	ESP_LOGE(TAG, "baidu Start");
	ESP_LOGE(TAG, "Taking picture...");
	camera_fb_t *pic = esp_camera_fb_get();
	if (!pic)
	{
		ESP_LOGE(TAG, "esp_camera_fb_get failed...");
		return ESP_FAIL;
	}
	if (!frame2jpg(pic, 80, &_jpg_buf, &_jpg_buf_len))
	{
		ESP_LOGE(TAG, "fmt2jpg failed");
		return ESP_FAIL;
	}
	ESP_LOGE(TAG, "Picture taken! Its size was: %zu bytes", _jpg_buf_len);
	/*                       base64编码                                   */
	base64_buf = base64_encode(_jpg_buf, _jpg_buf_len);
	ESP_LOGE(TAG, "base64_encode OK,size: %d", strlen(base64_buf));
	urlcode_buf = malloc(sizeof(char) * strlen(base64_buf) * 2);
	char* read_buf = malloc(sizeof(char) * strlen(base64_buf) * 2);
	URLEncode(base64_buf, strlen(base64_buf), urlcode_buf, strlen(base64_buf) * 2);
	ESP_LOGE(TAG, "urlencode OK,size: %d", strlen(urlcode_buf));
	ESP_LOGE(TAG, "%s", urlcode_buf);
	/*                       开始上传百度AI                         */
	esp_http_client_config_t config = {
		.url = "https://aip.baidubce.com/rest/2.0/ocr/v1/qrcode",
		.event_handler = img_http_event_handler,
		.user_data = local_response_buffer,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_method(client, HTTP_METHOD_POST);
	//esp_http_client_set_header(client, "Accept", "*/*");
	//esp_http_client_set_header(client, "Accept-Encoding", "identity");
	//esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	//esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
	esp_http_client_set_post_field(client, urlcode_buf, strlen(urlcode_buf));
 
	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP POST Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
		esp_http_client_read_response(client, read_buf, strlen(urlcode_buf));		 
		ESP_LOGE(TAG, "data:\n%s", read_buf );
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		free(read_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		read_buf = NULL;
		//esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	else
	{
		ESP_LOGE(TAG, "HTTP POST request failed: %d", esp_http_client_get_status_code(client));
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		free(read_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		read_buf = NULL;
		//esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	return res;
}
static esp_err_t img_http_event_handler(esp_http_client_event_t *evt)
{
	const char *TAG = "baidu";
	//printf("evtid = %d\n", evt->event_id);
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
	
		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
		printf("%.*s\n", evt->data_len, (char *)evt->data);

		cJSON *json_root = cJSON_Parse((char *)evt->data);
		cJSON *result_num = cJSON_GetObjectItem(json_root, "codes_result_num");

		printf("result_num = %d\n", result_num->valueint);
		if (result_num->valueint == 0)
		{
			cJSON_Delete(json_root);
			return ESP_OK;
		}
		if (result_num->valueint == 1)
		{
			cJSON *result = cJSON_GetObjectItem(json_root, "codes_result");
			cJSON *result1 = cJSON_GetArrayItem(result, 0);
			cJSON *text = cJSON_GetObjectItem(result1, "text");
			printf("text = %s\n", cJSON_GetArrayItem(text, 0)->valuestring);
			//strcpy((char *)detect_things, keyword->valuestring);

		}
		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	}
	return ESP_OK;
}

void scanner_task(void* parm)
{
	while(1)
	{
		baidu_img_ai();
		vTaskDelay(3000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
