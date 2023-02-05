#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "lora.h"

uint8_t buffer[1024];

int led1 = 32;
int led2 = 33;


char *version;
char *mac_id;
int JOYSTICK;
int POTENTIOMETER;
int SWITCH;

int joy_x = 0;
int joy_y = 0;
int pot_1 = 0;
int pot_2 = 0;
int sw_1 = 0;
int sw_2 = 0;

void task_rx(void *p)
{

    gpio_pad_select_gpio(led1);
    gpio_pad_select_gpio(led2);
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);
    gpio_set_direction(led2, GPIO_MODE_OUTPUT);



   while(1) {
      lora_receive();    // put into receive mode
      while(lora_received()) {
        int rxBytes = lora_receive_packet(buffer, sizeof(buffer));
         buffer[rxBytes] = 0;
//         printf("Received JSON String: %s\n", buffer);
      }

      const char* my_json_string = (const char*)buffer;


//      ESP_LOGI("RX", "Deserialize.....");
      	cJSON *root2 = cJSON_Parse(my_json_string);

      	if (cJSON_GetObjectItem(root2, "version")) {
      		version = cJSON_GetObjectItem(root2,"version")->valuestring;
      		ESP_LOGI("RX", "version=%s",version);
      	}
      	if (cJSON_GetObjectItem(root2, "macid")) {
      		mac_id = cJSON_GetObjectItem(root2,"macid")->valuestring;
      		ESP_LOGI("RX", "macid=%s",mac_id);
      	}



  /***********joystick array start***************************/
      	char str1[25];
      	char JOY_X[10];
      	char JOY_Y[10];
    	cJSON *array1 = cJSON_GetObjectItem(root2,"JOYSTICK");
    	int array1_size = cJSON_GetArraySize(array1);
    	for (int i=0;i<array1_size;i++) {
    		cJSON *element1 = cJSON_GetArrayItem(array1, i);
    		char* joystick = cJSON_Print(element1);
//            printf("joystick: %s\n", joystick);
            strcpy(str1, joystick);
//            printf("str1: %s\n", str1);

            if (i == 0) {
			strcpy(JOY_X, str1);
			joy_x = atoi(JOY_X);
//    	    printf("\nJOY_X = %d\n", joy_x);
		    }
            if (i == 1) {
			strcpy(JOY_Y, str1);
			joy_y = atoi(JOY_Y);
//    	    printf("\nJOY_Y = %d\n", joy_y);
		    }
//        	ESP_LOGI("RX", "Joystick1=%d, Joystick2=%d", joy_x, joy_y);
    	}
/***********joystick array end***************************/


/***********potentiometer array start***************************/
      	char str2[25];
      	char POT_1[10];
      	char POT_2[10];
    	cJSON *array2 = cJSON_GetObjectItem(root2,"POTENTIOMETER");
    	int array2_size = cJSON_GetArraySize(array2);
    	for (int i=0;i<array2_size;i++) {
    		cJSON *element2 = cJSON_GetArrayItem(array2, i);
    		char* potentiometer = cJSON_Print(element2);
//            printf("potentiometer: %s\n", potentiometer);
            strcpy(str2, potentiometer);
//            printf("str1: %s\n", str1);

            if (i == 0) {
			strcpy(POT_1, str2);
			pot_1 = atoi(POT_1);
//    	    printf("POT_1 = %d\n", pot_1);
		    }
            if (i == 1) {
			strcpy(POT_2, str2);
			pot_2 = atoi(POT_2);
//    	    printf("POT_2 = %d\n", pot_2);
		    }
//        	ESP_LOGI("RX", "Potentiometer1=%d, Potentiometer2=%d", pot_1, pot_2);
    	}
/***********potentiometer array end***************************/


/***********Switch array start***************************/
		char str3[25];
		char SW_1[10];
		char SW_2[10];
		cJSON *array3 = cJSON_GetObjectItem(root2,"SWITCH");
		int array3_size = cJSON_GetArraySize(array3);
		for (int i=0;i<array3_size;i++) {
			cJSON *element3 = cJSON_GetArrayItem(array3, i);
			char* switchs = cJSON_Print(element3);
//            printf("switchs: %s\n", switchs);
			strcpy(str3, switchs);
//            printf("str1: %s\n", str1);

			if (i == 0) {
			strcpy(SW_1, str3);
			sw_1 = atoi(SW_1);
//    	    printf("SW_1 = %d\n", sw_1);
			}
			if (i == 1) {
			strcpy(SW_2, str3);
			sw_2 = atoi(SW_2);
//    	    printf("SW_2 = %d\n", sw_2);
			}
//        	ESP_LOGI("RX", "Switch1=%d, Switch2=%d", sw_1, sw_2);
		}
/***********Switch array end***************************/




        ESP_LOGI("RX", "joyx:%d, joyy:%d, pot1:%d, pot2:%d, sw1:%d, sw2:%d\n", joy_x, joy_y, pot_1, pot_2, sw_1, sw_2);
//      	printf("joyx:%d, joyy:%d, pot1:%d, pot2:%d, btn1:%d, btn2:%d\n", joy_x, joy_y, pot_1, pot_2, sw_1, sw_2);



      		cJSON_Delete(root2);
      vTaskDelay(10);
   }
}



void app_main()
{
   lora_init();
   lora_set_frequency(433e6);
   lora_enable_crc();
   xTaskCreate(&task_rx, "task_rx", 2048, NULL, 5, NULL);
}
