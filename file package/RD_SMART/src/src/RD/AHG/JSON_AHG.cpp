/*
 * JSON_AHG.cpp
 *
 *  Created on: Apr 10, 2021
 *      Author: trthang
 */

#include "../AHG/JSON/JSON_AHG.hpp"

/*-----------------------------------------------convert-------------------------------------------------------*/
queue<string> q;
bool a = true;
extern struct mosquitto *mosq;
template<typename T>
string toStringGG(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

char* ToCharGG(string s) {
	char *send = new char[s.length() + 1];
	strcpy(send, s.c_str());
	return send;
}
//convert data json from APP
int JsonParseApp(struct mosquitto *mosq, char * jobj){
	string json = toStringGG(jobj);
	q.push(json);
//	ScanStop(mosq, jobj);
//	Device(mosq, jobj);
//	DelDevice(mosq, jobj);
//	Group(mosq, jobj);
//	EventTrigger(mosq, jobj);
//	EventOutput(mosq, jobj);
//	CallEventTrigger(mosq, jobj);
//	InputSensor(mosq, jobj);
//	InputRemote(mosq, jobj);
	return 0;
}

void* AHG_MQTT(void *argv){
	while(a){
		int size = q.size();
		while(size != 0) {
			string item = q.front();
			char * jobj = ToCharGG(item);
			ScanStop(mosq, jobj);
			Device(mosq, jobj);
			DelDevice(mosq, jobj);
			Group(mosq, jobj);
			EventTrigger(mosq, jobj);
			EventOutput(mosq, jobj);
			CallEventTrigger(mosq, jobj);
			InputSensor(mosq, jobj);
			InputRemote(mosq, jobj);
			InputDoorSensor(mosq, jobj);
			q.pop();
			size = q.size();
		}
		sleep(1);
	}
	return 0;
}
