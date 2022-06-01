#include "driver/gpio.h"

#include "app_button.hpp"
#include "app_camera.hpp"
#include "app_lcd.hpp"
#include "app_led.hpp"
#include "app_speech.hpp"

extern "C"{
#include "qrcode_scan.h"
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
}




extern "C" void app_main()
{
	

    QueueHandle_t xQueueFrame_2 = xQueueCreate(2, sizeof(camera_fb_t *));

    AppButton *key = new AppButton();
    AppSpeech *speech = new AppSpeech();
    AppCamera *camera = new AppCamera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, NULL);

    AppLCD *lcd = new AppLCD(key, speech, xQueueFrame_2);
    AppLED *led = new AppLED(GPIO_NUM_3, key, speech);

    key->attach(led);
    key->attach(lcd);

    speech->attach(led);
    speech->attach(lcd);

    lcd->run();
    camera->run();
    speech->run();
    key->run();

	ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(example_connect());
    xTaskCreate( scanner_task, "scanner", 20 * 1024, NULL, 5, NULL );
}
