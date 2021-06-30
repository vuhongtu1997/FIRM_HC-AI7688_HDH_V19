/*
 * MQTT.h process tasks-related mqtt
 * Config mqtt
 * Create link with mqtt broker
 * Transmit mqtt
 */
#ifndef GATEWAYMANAGER_MQTT_H_
#define GATEWAYMANAGER_MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include <json-c/json.h>
#include <mosquitto.h>

/* Information of mqtt broker*/
#define mqtt_host 			"10.10.10.1"//"postman.cloudmqtt.com"//"soldier.cloudmqtt.com"//"192.168.33.1"//"192.168.100.1"//
#define mqtt_port 			1883//13001//11875//1883//
#define mqtt_username 		"RD"//"insklndl"//"jpcvzwgj"//"RD"//
#define mqtt_password 		"1"//"x9aBwks70kmQ"//"JCCSw9dYngMF"//"2k756Wus2bJE"//"1"//

#define TP_STATUS           "RD_STATUS"

extern int run;
extern struct mosquitto *mosq;
extern unsigned char qos;
extern bool hasRsp;

/*
 * Check signal to process
 *
 * @param s
 * @return null
 */
void handle_signal(int s);

/*
 * Transmit mqtt
 *
 * @param mosq mosquitto
 * @param topic topic mqtt
 * @param msg message mqtt
 * @return null
 */
int mqtt_send(struct mosquitto *mosq, char * topic,char *msg);

/*
 * Callback connect to mqtt broker
 *
 * @param mosq mosquitto
 * @param obj
 * @param result number of automatic reconnections
 * @return null
 */
void connect_callback(struct mosquitto *mosq, void *obj, int result);

/*
 * Receive and process mqtt
 * In this function call function handle message comming
 *
 * @param mosq mosquitto
 * @param obj
 * @param message message mqtt comming
 * @return null
 */
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);



/*
 * Thead manage mqtt
 * - keep connect to mqtt broker
 * - listen message coming
 */
void * MQTT_Thread(void *argv);

#ifdef __cplusplus
}
#endif

#endif /* GATEWAYMANAGER_MQTT_H_ */
