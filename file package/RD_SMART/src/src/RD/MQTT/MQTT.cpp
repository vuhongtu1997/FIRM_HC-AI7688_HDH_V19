/*
 * MQTT.cpp
 *
 *  Created on: Apr 12, 2021
 *      Author: trthang
 */

#include "MQTT.hpp"



//connect MQTT
void ConnectCallback(struct mosquitto *mosq, void *obj, int rc){
	if(rc){
		printf("ERROR WITH RESULT CODE: %d !!!\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "HC.CONTROL", 2);//subscribe topic APP
	mosquitto_subscribe(mosq, NULL, "RD_STATUS", 2);//subscribe topic GW
}

//MQTT public GW
int MqttSend(struct mosquitto *mosq, char *msg){
	if(strlen(msg) > 10){
		mosquitto_publish(mosq, NULL, "RD_CONTROL", strlen(msg), msg, 2, 0);//public
	}
	return 0;
}

//MQTT public APP
int MqttSendAPP(struct mosquitto *mosq, char *msg){
	mosquitto_publish(mosq, NULL, "HC.CONTROL.RESPONSE", strlen(msg), msg, 2, 0);//public
	return 0;
}

//MQTT data sub topic
void MessageCallback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	char* msg = (char*)message->payload;
	try {
		if(strcmp(message->topic, "HC.CONTROL") == 0){
			JsonParseApp(mosq, msg);
			Timer_AHG(mosq, msg);
		}else if(strcmp(message->topic, "RD_STATUS") == 0) {
			JsonParseGw(mosq, msg);
			BUTTON(mosq, msg);
		}
	} catch (exception &e) {
	}
}

