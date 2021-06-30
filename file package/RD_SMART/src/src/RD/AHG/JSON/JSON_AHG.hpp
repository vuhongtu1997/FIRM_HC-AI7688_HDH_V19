/*
 * JSON_PARSE.hpp
 *
 *  Created on: Apr 10, 2021
 *      Author: trthang
 */

#ifndef RD_JSON_JSON_HPP_
#define RD_JSON_JSON_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <string>
#include <mosquitto.h>
#include <iostream>
#include <sstream>
#include <queue>

#include "../../../rapidjson/document.h"
#include "../../../rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;



extern pthread_t tmp_thread;


void* AHG_MQTT(void *argv);

/*-----------------------------------------------APP---------------------------------------------------------*/

//Parse Json APP
int JsonParseApp(struct mosquitto *mosq, char * jobj);

//Device
void Device(struct mosquitto *mosq, char* jobj);

//Delete Device
void DelDevice(struct mosquitto *mosq, char* jobj);

//Group
void Group(struct mosquitto *mosq, char* jobj);
	//Create Group + Device
	void CreateGroup(struct mosquitto *mosq, char* jobj);
	//Add Device
	void AddDeviceGroup(struct mosquitto *mosq, char* jobj);
	//Delete Device
	void DelDeviceGroup(struct mosquitto *mosq, char* jobj);
	//Delete Group
	void DelGroup(struct mosquitto *mosq, char* jobj);
	//Control Group
	void ControlGroup(struct mosquitto *mosq, char* jobj);

//Call Scene
void CallEventTrigger(struct mosquitto *mosq, char* jobj);

//EventTrigger
void EventTrigger(struct mosquitto *mosq, char* jobj);
	//Create EventTrigger Scene
	void CreateScene(struct mosquitto *mosq, char* jobj);
	//Create EventTrigger Scene
	void EditScene(struct mosquitto *mosq, char* jobj);
	//Delete EventTrigger Scene
	void DelScene(struct mosquitto *mosq, char* jobj);

	//Create EventTrigger Rule
	void CreateRule(struct mosquitto *mosq, char* jobj);
	//Create EventTrigger Rule
	void EditRule(struct mosquitto *mosq, char* jobj);
	//Delete EventTrigger Rule
	void DelRule(struct mosquitto *mosq, char* jobj);

//EventTriggerOutput
void EventOutput(struct mosquitto *mosq, char* jobj);
	//ADD
	void AddEventTriggerOuput(struct mosquitto *mosq, char* jobj);
	//DELETE
	void DeleteEventTriggerOuput(struct mosquitto *mosq, char* jobj);

//Scan/Stop
void ScanStop(struct mosquitto *mosq, char* jobj);

//EventTrigger InputSensor
void InputSensor(struct mosquitto *mosq, char* jobj);
	//Create
	void CreateSensor(struct mosquitto *mosq, char* jobj);
	//Delete
	void DelSensor(struct mosquitto *mosq, char* jobj);
//EventTrigger InputRemote
void InputRemote(struct mosquitto *mosq, char* jobj);
	//Create
	void CreateRemote(struct mosquitto *mosq, char* jobj);
	//Delete
	void DelRemote(struct mosquitto *mosq, char* jobj);

//EventTrigger InputSensor
void InputDoorSensor(struct mosquitto *mosq, char* jobj);
	//Create
	void CreateDoorSensor(struct mosquitto *mosq, char* jobj);
	//Delete
	void DelDoorSensor(struct mosquitto *mosq, char* jobj);

/*-----------------------------------------------GW---------------------------------------------------------*/



/*-----------------------------------------------DATABASE---------------------------------------------------------*/

int DBJSON(struct mosquitto *mosq,string control, string sql);

int DBCHECK(struct mosquitto *mosq,string control, string sql);

int DBSQL(struct mosquitto *mosq, string sql);

int DBSQLJSON(struct mosquitto *mosq, string sql);

/*-----------------------------------------------#---------------------------------------------------------*/

int createGroupId(struct mosquitto *mosq);

int createSceneId(struct mosquitto *mosq);

char* checkControl(struct mosquitto *mosq, int idControl);

char* checkControlEvent(struct mosquitto *mosq, int idControl);

int checkDeviceAttribute(struct mosquitto *mosq, string Code);

int SCENE_UNICAST_ID(string SCENE_ID);

int GROUP_UNICAST_ID(string GROUP_ID);

int DEVICE_UNICAST_ID(string DEVICE_ID);


#endif /* RD_JSON_JSON_HPP_ */
