/*
 * JSON_GHA.cpp
 *
 *  Created on: Apr 10, 2021
 *      Author: trthang
 */

#include "../GHA/JSON/JSON_GHA.hpp"

queue<string> qu;
bool b = true;
extern struct mosquitto *mosq;
template<typename T>
string toStringAA(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

char* ToCharAA(string s) {
	char *send = new char[s.length() + 1];
	strcpy(send, s.c_str());
	return send;
}

int JsonParseGw(struct mosquitto *mosq, char * jobj){
	string json = toStringAA(jobj);
	qu.push(json);
//	AdrGwToApp(mosq, jobj);
//	TYPEDEVICE(mosq, jobj);
	return 0;
}

void* GHA_MQTT(void *argv){
	while(b){
		int sizea = qu.size();
		while(sizea != 0) {
			string item = qu.front();
			char *jobj = ToCharAA(item);
			AdrGwToApp(mosq, jobj);
			TYPEDEVICE(mosq, jobj);
			qu.pop();
			sizea = qu.size();
		}
		sleep(1);
	}
	return 0;
}
