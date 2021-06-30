/*
 * MQTT.hpp
 *
 *  Created on: Apr 12, 2021
 *      Author: trthang
 */

#ifndef RD_MQTT_MQTT_HPP_
#define RD_MQTT_MQTT_HPP_

#include "../../RD/AHG/JSON/JSON_AHG.hpp"
#include "../../RD/GHA/JSON/JSON_GHA.hpp"
#include "../../RD/TIMER/TIMER_PARES.hpp"

//bien mqtt
#define mqtt_host "10.10.10.1"
#define mqtt_username 		"RD"
#define mqtt_password 		"1"
#define mqtt_port 1883

extern struct mosquitto *mosq;

/*-----------------------------------------------MQTT---------------------------------------------------------*/
//connect MQTT
void ConnectCallback(struct mosquitto *mosq, void *obj, int rc);

//MQTT take data
void MessageCallback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

//MQTT public msg
int MqttSend(struct mosquitto *mosq, char *msg);

int MqttSendAPP(struct mosquitto *mosq, char *msg);



#endif /* RD_MQTT_MQTT_HPP_ */
