//============================================================================
// Name        : RD_AHG.cpp
// Author      : Tr.Thang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "RD/AHG/JSON/JSON_AHG.hpp"
#include "RD/GHA/JSON/JSON_GHA.hpp"
#include "RD/TIMER/TIMER_PARES.hpp"
#include "RD/MQTT/MQTT.hpp"

using namespace std;


pthread_t threadTimer;
pthread_t jsonAHGGG;
pthread_t jsonGHAAA;

//create var
int run = 1;
struct mosquitto *mosq;

int main() {
	char clientid[24];
	int rc = 0;
	int abc = 0;

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	snprintf(clientid, 23, "mysql_log_%d", getpid());
	mosq = mosquitto_new(clientid, true, 0);
	if(mosq){
			mosquitto_connect_callback_set(mosq, ConnectCallback);
			mosquitto_message_callback_set(mosq, MessageCallback);
			abc = mosquitto_username_pw_set(mosq, mqtt_username, mqtt_password);
			rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
			pthread_create(&threadTimer, NULL, CallEvent, NULL);
			pthread_create(&jsonAHGGG, NULL, AHG_MQTT, NULL);
			pthread_create(&jsonGHAAA, NULL, GHA_MQTT, NULL);

			sleep(5);

			while(run){
				rc= abc = mosquitto_loop(mosq, -1, 1);
				if(run && rc){
					printf("----------------- CONNECT ERROR !!! -----------------\n");
					sleep(10);
					mosquitto_reconnect(mosq);
				}
			}
			mosquitto_destroy(mosq);
		}
	mosquitto_lib_cleanup();
	return 0;
}
