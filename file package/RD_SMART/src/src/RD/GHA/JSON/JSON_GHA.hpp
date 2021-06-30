/*
 * JSON_GHA.hpp
 *
 *  Created on: Apr 10, 2021
 *      Author: trthang
 */

#ifndef RD_JSON_JSON_GHA_HPP_
#define RD_JSON_JSON_GHA_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <ctime>
#include <sqlite3.h>
#include <mosquitto.h>
#include <string>
#include <iostream>
#include <sstream>
#include <queue>

#include "../../../rapidjson/document.h"
#include "../../../rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;



void* GHA_MQTT(void *argv);

/*-----------------------------------------------GW-------------------------------------------------------------*/

int JsonParseGw(struct mosquitto *mosq, char * jobj);

void AdrGwToApp(struct mosquitto *mosq, char* jobj);

void TYPEDEVICE(struct mosquitto *mosq, char* jobj);

char* checkControlCode(struct mosquitto *mosq, int idControl);

string getGUID(struct mosquitto *mosq, int adr);

int getUnicast(struct mosquitto *mosq, string control);

int checkType(struct mosquitto *mosq, string Code);

int setDay();
int setTime();

string getGUID(struct mosquitto *mosq, int adr);

int getUnicast(struct mosquitto *mosq, string control);

//#
void ONOFF(struct mosquitto *mosq, char* jobj);
void CCT(struct mosquitto *mosq, char* jobj);
void DIM(struct mosquitto *mosq, char* jobj);
void HSL(struct mosquitto *mosq, char* jobj);
void ADDGROUP(struct mosquitto *mosq, char* jobj);
void DELGROUP(struct mosquitto *mosq, char* jobj);
void CALLSCENE(struct mosquitto *mosq, char* jobj);
void ADDSCENE(struct mosquitto *mosq, char* jobj);
void DELSCENE(struct mosquitto *mosq, char* jobj);
void LUX(struct mosquitto *mosq, char* jobj);
void PIR(struct mosquitto *mosq, char* jobj);
void POWER(struct mosquitto *mosq, char* jobj);
void RESETNODE(struct mosquitto *mosq, char* jobj);
void SETSCENEFORREMOTE(struct mosquitto *mosq, char* jobj);
void DELSCENEFORREMOTE(struct mosquitto *mosq, char* jobj);
void SETSCENEFORSENSOR(struct mosquitto *mosq, char* jobj);
void DELSCENEFORSENSOR(struct mosquitto *mosq, char* jobj);




#endif /* RD_JSON_JSON_GHA_HPP_ */
