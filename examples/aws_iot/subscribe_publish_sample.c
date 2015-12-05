/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file subscribe_publish_sample.c
 * @brief simple MQTT publish and subscribe on the same topic
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "sdkTest/sub"
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 * The application takes in the certificate path, host name , port and the number of times the publish should happen.
 *
 */
#include "espressif/esp_common.h"
#include "espressif/esp_system.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <signal.h>
//#include <memory.h>
#include <string.h> // add
#include <sys/time.h>
#include <limits.h>

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_interface.h"
#include "aws_iot_config.h"

#include "ssid_config.h"

int MQTTcallbackHandler(MQTTCallbackParams params) {

	INFO("Subscribe callback");
	INFO("%.*s\t%.*s",
			(int)params.TopicNameLen, params.pTopicName,
			(int)params.MessageParams.PayloadLen, (char*)params.MessageParams.pPayload);

	return 0;
}

void disconnectCallbackHandler(void) {
	WARN("MQTT Disconnect");
}

/**
 * @brief Default cert location
 */
char certDirectory[PATH_MAX + 1] = "../../certs";

/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
uint32_t port = AWS_IOT_MQTT_PORT;

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
uint32_t publishCount = 0;

void mqttTask(void *pvParameters) {
	printf("SDK version:%s\n", sdk_system_get_sdk_version());
	IoT_Error_t rc = NONE_ERROR;
	//int32_t i = 0;
	//bool infinitePublishFlag = true;

	printf("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

	MQTTConnectParams connectParams = MQTTConnectParamsDefault;

	connectParams.KeepAliveInterval_sec = 10;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = "CSDK-test-device";
	connectParams.pHostURL = HostAddress;
	connectParams.port = port;
	connectParams.isWillMsgPresent = false;
	connectParams.pRootCALocation = "";
	connectParams.pDeviceCertLocation = "";
	connectParams.pDevicePrivateKeyLocation = "";
	connectParams.mqttCommandTimeout_ms = 2000;
	connectParams.tlsHandshakeTimeout_ms = 5000;
	connectParams.isSSLHostnameVerify = false;// ensure this is set to true for production
	connectParams.disconnectHandler = disconnectCallbackHandler;

	INFO("Connecting...");
	rc = aws_iot_mqtt_connect(&connectParams);
	if (NONE_ERROR != rc) {
		ERROR("Error(%d) connecting to %s:%d", rc, connectParams.pHostURL, connectParams.port);
	}
	/*

	MQTTSubscribeParams subParams = MQTTSubscribeParamsDefault;
	subParams.mHandler = MQTTcallbackHandler;
	subParams.pTopic = "sdkTest/sub";
	subParams.qos = QOS_0;

	if (NONE_ERROR == rc) {
		INFO("Subscribing...");
		rc = aws_iot_mqtt_subscribe(&subParams);
		if (NONE_ERROR != rc) {
			ERROR("Error subscribing");
		}
	}

	MQTTMessageParams Msg = MQTTMessageParamsDefault;
	Msg.qos = QOS_0;
	char cPayload[100];
	sprintf(cPayload, "%s : %d ", "hello from SDK", i);
	Msg.pPayload = (void *) cPayload;


	MQTTPublishParams Params = MQTTPublishParamsDefault;
	Params.pTopic = "sdkTest/sub";


	if(publishCount != 0){
		infinitePublishFlag = false;
	}

	while (NONE_ERROR == rc && (publishCount > 0 || infinitePublishFlag)) {

		//Max time the yield function will wait for read messages
		rc = aws_iot_mqtt_yield(100);
		INFO("-->sleep");
		vTaskDelay(1000 / portTICK_RATE_MS);
		sprintf(cPayload, "%s : %d ", "hello from SDK", i++);
		Msg.PayloadLen = strlen(cPayload) + 1;
		Params.MessageParams = Msg;
		rc = aws_iot_mqtt_publish(&Params);
		if(publishCount > 0){
			publishCount--;
		}
	}

	if(NONE_ERROR != rc){
		ERROR("An error occurred in the loop.\n");
	}
	else{
		INFO("Publish done\n");
	}
	*/
	while (1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void memoryUsage(void *pvParameters) {
  while(1) {
		uint32_t memfree = sdk_system_get_free_heap_size();
		printf("==> memfree: %d\n", memfree);
    vTaskDelay(100 / portTICK_RATE_MS);
  }
}

void user_init(void)
{
  uart_set_baud(0, 115200);
	struct sdk_station_config config = {
			.ssid = WIFI_SSID,
			.password = WIFI_PASS,
	};

	/* required to call wifi_set_opmode before station_set_config */
	sdk_wifi_set_opmode(STATION_MODE);
	sdk_wifi_station_set_config(&config);

  xTaskCreate(&mqttTask, (signed char *)"mqttTask", 4096, NULL, 2, NULL);
  xTaskCreate(&memoryUsage, (signed char *)"memoryUsage", 1024, NULL, 2, NULL);
}
