/*
 * MQTT.c
 */

#include "MQTT.h"
#include "OpCode.h"
#include "Provision.h"
#include "Light.h"
#include "JsonProcess.h"
#include "slog.h"
#include "Linkerlist.h"
#include <time.h>

clock_t start,end;
char *pHeaderMqtt = "mqtt";
struct mosquitto *mosq;
unsigned char qos =2;
int run = 1;
bool countdown= false;
bool hasRsp = false;
bool isSetup = true;
uint8_t timeSendNode = 0;
bool delHead = false;

void handle_signal(int s)
{
	run = 0;
}
int mqtt_send(struct mosquitto *mosq, char * topic,char *msg)
{
	mosquitto_publish(mosq, NULL,topic, strlen(msg), msg, qos, 0);
	slog_info("(%s)Message_send: %s",pHeaderMqtt,msg);
	return 0;
}
void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	slog_info("(%s)%s: Connect callback, rc=%d",pHeaderMqtt,mqtt_host,result);
	mqtt_send(mosq,"RD_STATUS","{\"CMD\":\"CONNECTED\"}");
}
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	char* msg = (char*)message->payload;
		slog_info("(%s)Message_receive: %s",pHeaderMqtt,msg);
		if(json_tokener_parse(msg)!= NULL)
		{
				AddTail(head, msg);
				ShowLL(head);
				countdown= true;
		}
		if(head != NULL){
			hasRsp = false;
			while(!hasRsp){
				if(countdown){
					countdown = false;
					start = clock();
				}

				end = clock();
				if(((double)(end-start) >= 500000) || (hasRsp == true)){
					countdown = true;
					struct json_object * jobj = json_tokener_parse(head->message);
					Json_Parse(jobj);
					timeSendNode++;
					if(timeSendNode == 3){
						delHead = true;
						timeSendNode = 0;
						hasRsp = true;
					}
				}
			}
			if((hasRsp == true) || (delHead == true)){
				delHead = false;
				hasRsp = false;
				head = DellHead(head);
				//printf("DELETE LINKER LIST\n");
				ShowLL(head);
			}
		}
		usleep(100);
}

void * MQTT_Thread(void *argv)
{
		char clientid[24];
		int rc = 0;
		int abc = 0;

		signal(SIGINT, handle_signal);
		signal(SIGTERM, handle_signal);

		mosquitto_lib_init();

		memset(clientid, 0, 24);
		snprintf(clientid, 23, "mysql_log_%d", getpid());
		mosq = mosquitto_new(clientid, true, 0);
		if(mosq){
			mosquitto_connect_callback_set(mosq, connect_callback);
			mosquitto_message_callback_set(mosq, message_callback);

			abc = mosquitto_username_pw_set(mosq, mqtt_username, mqtt_password);
			rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);

			mosquitto_subscribe(mosq,NULL, "RD_CONTROL",qos);
			while(run){
				rc= abc = mosquitto_loop(mosq, -1, 1);
				if(run && rc){
					slog_warn("Connection mqtt error");
					sleep(4);
					mosquitto_reconnect_async(mosq);
				}
			}
			mosquitto_destroy(mosq);
		}
		mosquitto_lib_cleanup();
		return NULL;
}

