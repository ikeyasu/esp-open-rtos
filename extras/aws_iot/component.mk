# Component makefile for extras/aws_iot

AWS_IOT_SDK_DIR=$(aws_iot_ROOT)/aws-iot-device-sdk-embedded-C

#IoT client directory
IOT_CLIENT_DIR=$(AWS_IOT_SDK_DIR)/aws_iot_src
IOT_INCLUDE_DIRS += $(IOT_CLIENT_DIR)/protocol/mqtt
IOT_INCLUDE_DIRS += $(IOT_CLIENT_DIR)/protocol/mqtt/aws_iot_embedded_client_wrapper
IOT_INCLUDE_DIRS += $(IOT_CLIENT_DIR)/protocol/mqtt/aws_iot_embedded_client_wrapper/platform_esp_open_rtos
IOT_INCLUDE_DIRS += $(IOT_CLIENT_DIR)/utils

PLATFORM_DIR = $(IOT_CLIENT_DIR)/protocol/mqtt/aws_iot_embedded_client_wrapper/platform_esp_open_rtos
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/protocol/mqtt/aws_iot_embedded_client_wrapper/aws_iot_mqtt_embedded_client_wrapper.c
IOT_SRC_FILES += $(shell find $(PLATFORM_DIR)/ -name '*.c')

#MQTT Paho Embedded C client directory
MQTT_DIR = $(AWS_IOT_SDK_DIR)/aws_mqtt_embedded_client_lib
MQTT_C_DIR = $(MQTT_DIR)/MQTTClient-C/src
MQTT_EMB_DIR = $(MQTT_DIR)/MQTTPacket/src

MQTT_INCLUDE_DIR += -I $(MQTT_EMB_DIR)
MQTT_INCLUDE_DIR += -I $(MQTT_C_DIR)

MQTT_SRC_FILES += $(shell find $(MQTT_EMB_DIR)/ -name '*.c')
MQTT_SRC_FILES += $(MQTT_C_DIR)/MQTTClient.c

INC_DIRS += $(IOT_INCLUDE_DIRS) $(MQTT_INCLUDE_DIR)

# Logging level control
LOG_FLAGS += -DIOT_DEBUG
LOG_FLAGS += -DIOT_INFO
LOG_FLAGS += -DIOT_WARN
LOG_FLAGS += -DIOT_ERROR

# args for passing into compile rule generation
aws_iot_INC_DIRS =
aws_iot_SRC_DIR =
aws_iot_EXTRA_SRC_FILES = $(IOT_SRC_FILES) $(MQTT_SRC_FILES)
aws_iot_CFLAGS =  $(LOG_FLAGS) -mlongcalls

$(eval $(call component_compile_rules,aws_iot))
