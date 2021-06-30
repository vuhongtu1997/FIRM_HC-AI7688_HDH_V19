/*
 * JSON_PARSE.cpp
 *
 *  Created on: Apr 10, 2021
 *      Author: trthang
 */

#include "JSON_AHG.hpp"
#include "../../MQTT/MQTT.hpp"
#include <sqlite3.h>

//-----------------convert
template<typename T>
string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

char* ToChar(string s) {
	char *send = new char[s.length() + 1];
	strcpy(send, s.c_str());
	return send;
}

/*-----------------------------------------------DATABASE-------------------------------------------------------*/

/*-------------------------SQL---------------------------*/
int ADR;
string CONTROL;
StringBuffer sendToGW;
Writer<StringBuffer> jsonAHG(sendToGW);

static int SQLADR(void *data, int argc, char **argv, char **azColName){
	try {
		ADR = atoi((const char*) argv[0]);
	} catch (exception &e) {
	}
	return 0;
}

static int SQLCONTROL(void *data, int argc, char **argv, char **azColName){
	try {
		CONTROL = toString(argv[0]);
	} catch (exception &e) {
	}
	return 0;
}

static int SQLJsonD(void *data, int argc, char **argv, char **azColName){
	try {
		for(int i = 0; i<argc; i++){
			jsonAHG.Int(atoi((const char*) argv[i]));
		}
		return 0;
	} catch (exception &e) {
	}
	return 0;
}

static int UPDATE_DEVICE(void *data, int argc, char **argv, char **azColName){
	try {
		jsonAHG.Reset(sendToGW);
		sendToGW.Clear();

		jsonAHG.StartObject();
		jsonAHG.Key("CMD");
		jsonAHG.String("DEVICE_UPDATE");
		jsonAHG.Key("DATA");
		jsonAHG.StartObject();
		jsonAHG.Key("DEVICE_ID");
		string a = toString(argv[0]);
		jsonAHG.String(ToChar(a));
		jsonAHG.Key("DEVICE_UNICAST_ID");
		jsonAHG.Int(atoi((const char*) argv[1]));
		jsonAHG.Key("ID");
		jsonAHG.Int(atoi((const char*) argv[2]));
		jsonAHG.Key("VAL<sqlite3.h>UE");
		jsonAHG.Int(atoi((const char*) argv[3]));
		jsonAHG.EndObject();
		jsonAHG.EndObject();

		string addSceneGroup = sendToGW.GetString();
		cout<<addSceneGroup<<endl;
		char * sendT = new char[addSceneGroup.length()+1];
		strcpy(sendT, addSceneGroup.c_str());
		MqttSendAPP(mosq, sendT);


	} catch (exception &e) {
	}
	return 0;
}

static int ERROR(void *data, int argc, char **argv, char **azColName){
	try {
		string DeviceId = toString(argv[0]);
		StringBuffer sendToApp;
		Writer<StringBuffer> json(sendToApp);

		json.StartObject();
		json.Key("CMD");
		json.String("DEVICE_UPDATE");
		json.Key("DEVICE_ID");
		json.String(ToChar(DeviceId));
		json.Key("STATUS");
		json.String("OFFLINE");
		json.EndObject();

		cout << sendToApp.GetString() << endl;
		string s = sendToApp.GetString();
		char * sendT = new char[s.length()+1];
		strcpy(sendT, s.c_str());
		MqttSendAPP(mosq, sendT);
	} catch (exception &e) {
	}
	return 0;
}

static int SQLADRDELGS(void *data, int argc, char **argv, char **azColName){
	try {
		for(int i = 0; i<argc; i++){
			ADR = (atoi((const char*) argv[i]));
			string sql ="SELECT DeviceUnicastId FROM GroupingDeviceMapping WHERE GroupUnicastId  = "+toString(ADR)+";";
			DBSQLJSON(mosq, sql);
		}
	} catch (exception &e) {
	}
	return 0;
}



/*-------------------------DB---------------------------*/

int DBCHECK(struct mosquitto *mosq,string control, string sql){
	sqlite3 *DB;
	int exit = 0;
	do {
			exit = sqlite3_open("/root/rd.Sqlite", &DB);
		} while (exit != SQLITE_OK);
	char* messaggeError;

	string code;

	if(control.compare("ADR") == 0){
		exit = sqlite3_exec(DB, sql.c_str(), SQLADR, 0, &messaggeError);
	}else if (control.compare("CONTROL") == 0) {
		exit = sqlite3_exec(DB, sql.c_str(), SQLCONTROL, 0, &messaggeError);
		code = CONTROL;
	}


	if (exit != SQLITE_OK) {
		sqlite3_free(messaggeError);
		cout<<messaggeError<<endl;
	}
	else
		sqlite3_close(DB);

	return (0);
}

int DBSQL(struct mosquitto *mosq, string sql) {
	sqlite3* DB;
	int exit = 0;
	do {
		exit = sqlite3_open("/root/rd.Sqlite", &DB);
	} while (exit != SQLITE_OK);
	char* messaggeError = 0;

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		sqlite3_free(messaggeError);
	}
	else
		sqlite3_close(DB);

	return (0);
}

int DBSQLJSON(struct mosquitto *mosq, string sql) {
	sqlite3* DB;
	int exit = 0;
	do {
			exit = sqlite3_open("/root/rd.Sqlite", &DB);
		} while (exit != SQLITE_OK);
	char* messaggeError;

	exit = sqlite3_exec(DB, sql.c_str(), SQLJsonD, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		sqlite3_free(messaggeError);
	}
	else
		sqlite3_close(DB);

	return (0);
}

int DBJSON(struct mosquitto *mosq,string control, string sql) {
	sqlite3* DB;
	int exit = 0;
	do {
			exit = sqlite3_open("/root/rd.Sqlite", &DB);
		} while (exit != SQLITE_OK);
	char* messaggeError;

	if(control.compare("ADDGROUPTOEVENT") == 0){
		jsonAHG.Reset(sendToGW);
		sendToGW.Clear();
		const char *jobj = sql.c_str();
		Document document;
		document.Parse(jobj);
		const Value& DATA = document["DATA"];
		string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();

		int SCENE_UNICAST;
		if(DATA.HasMember("SCENE_UNICAST_ID")){
			SCENE_UNICAST = SCENE_UNICAST_ID(EVENT_TRIGGER_ID);
		}else {
			ADR =-1;
			string checkId = "SELECT SceneUnicastID from EventTriggerID where EventTriggerId='"+EVENT_TRIGGER_ID+"';";
			DBCHECK(mosq, "ADR", checkId);
			SCENE_UNICAST = ADR;
		}
		int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
		string check = toString(checkControlEvent(mosq, EVENT_TRIGGER_TYPE_ID));
		if(check.compare("SCENE") == 0){
			jsonAHG.StartObject();
			jsonAHG.Key("ADR");
			jsonAHG.StartArray();

			const Value& GROUPS = DATA["GROUPS"];

			for (rapidjson::SizeType i = 0; i < GROUPS.Size(); i++){
				const Value& a = GROUPS[i];
				string GROUP_ID = a["GROUP_ID"].GetString();
				string sql2 = "SELECT DeviceUnicastId FROM GroupingDeviceMapping WHERE GroupingId = '" + GROUP_ID + "';";
				exit = sqlite3_exec(DB, sql2.c_str(), SQLJsonD, 0, &messaggeError);
			}
			jsonAHG.EndArray();
			jsonAHG.Key("ADDSCENE");
			jsonAHG.Int(SCENE_UNICAST);
			jsonAHG.EndObject();

			string addSceneGroup = sendToGW.GetString();
			cout<<addSceneGroup<<endl;
			char * sendT = new char[addSceneGroup.length()+1];
			strcpy(sendT, addSceneGroup.c_str());
			MqttSend(mosq, sendT);
		}
		jsonAHG.Reset(sendToGW);
		sendToGW.Clear();
	}else if (control.compare("DELGROUPTOEVENT") == 0) {
		jsonAHG.Reset(sendToGW);
		sendToGW.Clear();

		const char *jobj = sql.c_str();
		Document document;
		document.Parse(jobj);
		const Value& DATA = document["DATA"];

		string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
		int SCENE_UNICAST = SCENE_UNICAST_ID(EVENT_TRIGGER_ID);
		int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
		string check = toString(checkControlEvent(mosq, EVENT_TRIGGER_TYPE_ID));
		if(check.compare("SCENE") == 0){
			const Value& a = DATA["GROUPS"];
			jsonAHG.StartObject();
			jsonAHG.Key("ADR");
			jsonAHG.StartArray();
			for (rapidjson::SizeType i = 0; i < a.Size(); i++){
				const Value& b = a[i];
				string GROUP_ID = b.GetString();
				string sql1 = "SELECT DeviceUnicastId FROM GroupingDeviceMapping WHERE GroupingId = '" + GROUP_ID + "';";
				exit = sqlite3_exec(DB, sql1.c_str(), SQLJsonD, 0, &messaggeError);
			}
			jsonAHG.EndArray();
			if(check.compare("SCENE") == 0){
				jsonAHG.Key("DELSCENE");
				jsonAHG.Int(SCENE_UNICAST);
			}
			jsonAHG.EndObject();

			string addSceneGroup = sendToGW.GetString();
			char * sendT = new char[addSceneGroup.length()+1];
			strcpy(sendT, addSceneGroup.c_str());
			MqttSend(mosq, sendT);
		}

	}else if (control.compare("DELGROUP") == 0){
		jsonAHG.Reset(sendToGW);
		sendToGW.Clear();

		const char *jobj = sql.c_str();
		Document document;
		document.Parse(jobj);
		const Value& DATA = document["DATA"];
		string GROUP_ID = DATA["GROUP_ID"].GetString();
		int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);

		jsonAHG.StartObject();
		jsonAHG.Key("ADR");
		jsonAHG.StartArray();

		string sql123 = "SELECT DeviceUnicastId FROM GroupingDeviceMapping WHERE GroupingId = '" + GROUP_ID + "';";
		exit = sqlite3_exec(DB, sql123.c_str(), SQLJsonD, 0, &messaggeError);
		jsonAHG.EndArray();
		jsonAHG.Key("DELGROUP");
		jsonAHG.Int(GROUP_UNICAST);
		jsonAHG.EndObject();

		string sDelete = sendToGW.GetString();

		char * sendT = new char[sDelete.length()+1];
		strcpy(sendT, sDelete.c_str());
		MqttSend(mosq, sendT);


		string creGroup = "INSERT OR REPLACE INTO GROUPID (GroupingId, GroupUnicastId, ValueCreate)"
					" values ('', "+ toString(GROUP_UNICAST) + ", 0); ";
		exit = sqlite3_exec(DB, creGroup.c_str(), NULL, 0, &messaggeError);
		string creGroup1 = "DELETE FROM GROUPING WHERE GroupingId = '"+ GROUP_ID +"';";
		exit = sqlite3_exec(DB, creGroup1.c_str(), NULL, 0, &messaggeError);
		string delDeviceGr = "DELETE FROM GroupingDeviceMapping WHERE GroupingId = '" + GROUP_ID + "' "
				"AND GroupUnicastId = " + toString(GROUP_UNICAST) +";";
		exit = sqlite3_exec(DB, delDeviceGr.c_str(), NULL, 0, &messaggeError);
	}else if (control.compare("DELSCENE") == 0){
		jsonAHG.Reset(sendToGW);
		sendToGW.Clear();

		const char *jobj = sql.c_str();
		Document document;
		document.Parse(jobj);

		const Value& DATA = document["DATA"];
		string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();

		jsonAHG.StartObject();
		jsonAHG.Key("ADR");
		jsonAHG.StartArray();

		string sqlselectG = "SELECT GroupUnicastId FROM EventTriggerOutputGroupingMapping WHERE EventTriggerId='"+EVENT_TRIGGER_ID+"';";
		exit = sqlite3_exec(DB, sqlselectG.c_str(), SQLADRDELGS, 0, &messaggeError);

		string sql = "SELECT DeviceUnicastId FROM EventTriggerOutputDeviceMapping Where EventTriggerId = '"+EVENT_TRIGGER_ID+"';";
		exit = sqlite3_exec(DB, sql.c_str(), SQLJsonD, 0, &messaggeError);
		jsonAHG.EndArray();

		ADR = -1;
		string sqlCheck = "SELECT EventTriggerTypeId FROM EventTrigger "
				"WHERE EventTriggerId = '"+ EVENT_TRIGGER_ID +"';";
		DBCHECK(mosq, "ADR", sqlCheck);

		string check = toString(checkControlEvent(mosq, ADR));
		if(check.compare("SCENE") == 0){
			jsonAHG.Key("DELSCENE");
			ADR = -1;
			string sqlADR = "SELECT SceneUnicastID FROM EventTriggerID "
					"WHERE EventTriggerId = '"+ EVENT_TRIGGER_ID +"';";
			DBCHECK(mosq, "ADR", sqlADR);
			jsonAHG.Int(ADR);
			jsonAHG.EndObject();

			string s = sendToGW.GetString();
			cout<<s<<endl;
			char * sendT = new char[s.length()+1];
			strcpy(sendT, s.c_str());
			MqttSend(mosq, sendT);

			ADR = -1;
			string checkAdrS = "SELECT SceneUnicastID FROM EventTrigger where EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
			DBCHECK(mosq, "ADR", checkAdrS);

			string delDT = "delete from EventTrigger where EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
			DBSQL(mosq, delDT);
			string delD = "delete from EventTriggerOutputDeviceMapping where EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
			DBSQL(mosq, delD);
			string delG = "delete from EventTriggerOutputGroupingMapping where EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
			DBSQL(mosq, delG);
			string delDV = "delete from EventTriggerOutputDeviceSetupValue where EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
			DBSQL(mosq, delDV);
			string delGV = "delete from EventTriggerOutputGroupingSetupValue where EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
			DBSQL(mosq, delGV);
			string creGroup = "INSERT OR REPLACE INTO EventTriggerID (EventTriggerId, SceneUnicastID, ValueCreate)"
								" values ('', "+ toString(ADR) + ", 0); ";
			DBSQL(mosq, creGroup);
		}

	}else if (control.compare("UPDATE_DEVICE") == 0){
		jsonAHG.Reset(sendToGW);
		sendToGW.Clear();

		exit = sqlite3_exec(DB, sql.c_str(), UPDATE_DEVICE, 0, &messaggeError);


	}else if(control.compare("ERROR") == 0){
		exit = sqlite3_exec(DB, sql.c_str(), ERROR, 0, &messaggeError);
	}

	if (exit != SQLITE_OK) {
		sqlite3_free(messaggeError);
	}
	else
		sqlite3_close(DB);
	return (0);
}


/*-----------------------------------------------GW---------------------------------------------------------*/

void SendDeviceToGW(struct mosquitto *mosq, int idDevice, int idControl, int value){
	char* control;
	control = checkControl(mosq, idControl);

	StringBuffer sendToGW;
	Writer<StringBuffer> jsonAHG(sendToGW);
	jsonAHG.StartObject();
	jsonAHG.Key("ADR");
	if(toString(control).compare("CCT") == 0 && idDevice < 49152){
		idDevice ++;
	}
	jsonAHG.Int(idDevice);
	jsonAHG.Key(control);
	jsonAHG.Int(value);
	jsonAHG.Key("TIME");
	jsonAHG.Int(0);
	jsonAHG.EndObject();

	string s = sendToGW.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
}

/*-----------------------------------------------APP---------------------------------------------------------*/

//Điều khiển đèn chiếu sáng
void Device(struct mosquitto *mosq, char* jobj){
	int R = -1;
	int G = -1;
	int B = -1;
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string CMD;
		if(document.HasMember("CMD")){
			CMD = document["CMD"].GetString();
			if(CMD.compare("DEVICE") == 0){
				if(document.HasMember("DATA")){
					const Value& DATA = document["DATA"];
					if(DATA.HasMember("DEVICE_ID") && DATA.HasMember("PROPERTIES")){
						string DEVICE_ID = DATA["DEVICE_ID"].GetString();
						//check GUID + UNICAST
						int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
						const Value& PROPERTIES = DATA["PROPERTIES"];
						for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
							const Value& a = PROPERTIES[i];
							if(a.HasMember("ID") && a.HasMember("VALUE")){
								int ID = a["ID"].GetInt();
								int VALUE = a["VALUE"].GetInt();
								char* controlRGB;
								controlRGB = checkControl(mosq, a["ID"].GetInt());
								if(toString(controlRGB).compare("HUE") == 0 || toString(controlRGB).compare("SATURATION") == 0 || toString(controlRGB).compare("LIGHTNESS") == 0){
									if(toString(controlRGB).compare("HUE") == 0){
										R = a["VALUE"].GetInt();
									}else if(toString(controlRGB).compare("SATURATION") == 0){
										G = a["VALUE"].GetInt();
									}else if(toString(controlRGB).compare("LIGHTNESS") == 0){
										B = a["VALUE"].GetInt();
									}

									if(R >= 0 && G >= 0 && B >= 0){
										StringBuffer sendToGW;
										Writer<StringBuffer> jsonAHG(sendToGW);
										jsonAHG.StartObject();
										jsonAHG.Key("ADR");
										jsonAHG.Int(DEVICE_UNICAST);
										jsonAHG.Key("HUE");
										jsonAHG.Int(R);
										jsonAHG.Key("SATURATION");
										jsonAHG.Int(G);
										jsonAHG.Key("LIGHTNESS");
										jsonAHG.Int(B);
										jsonAHG.Key("TIME");
										jsonAHG.Int(0);
										jsonAHG.EndObject();
										string s = sendToGW.GetString();
										char * sendT = new char[s.length()+1];
										strcpy(sendT, s.c_str());
										MqttSend(mosq, sendT);
										R = -1;
										G = -1;
										B = -1;
									}
								}else{
									//SEND TO GW
									SendDeviceToGW(mosq, DEVICE_UNICAST, ID, VALUE);
								}
							}
						}
					}
				}
			}
		}
	}
}

//Xóa device khỏi mạng
void DelDevice(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string cmd;
		if(document.HasMember("CMD")){
			cmd = document["CMD"].GetString();
			if(cmd.compare("RESET_NODE") == 0){
				const Value& DEVICES = document["DATA"];
				StringBuffer sendToGW;
				Writer<StringBuffer> jsonAHG(sendToGW);
				jsonAHG.StartObject();
				jsonAHG.Key("CMD");
				jsonAHG.String("RESETNODE");
				jsonAHG.Key("ADR");
				jsonAHG.StartArray();
				for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
					const Value& a = DEVICES[i];
					string DEVICE_ID = a.GetString();
					//check GUID + UNICAST
					int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
					jsonAHG.Int(DEVICE_UNICAST);
				}
				jsonAHG.EndArray();
				jsonAHG.EndObject();
				string s = sendToGW.GetString();
				char * sendT = new char[s.length()+1];
				strcpy(sendT, s.c_str());
				MqttSend(mosq, sendT);
			}
		}
	}
}

//Group
void Group(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string CMD;
		if(document.HasMember("CMD")){
			CMD = document["CMD"].GetString();
			if(CMD.compare("GROUP") == 0){
				if(document.HasMember("TYPE")){
					string TYPE = document["TYPE"].GetString();
					if(TYPE.compare("CREATE") == 0){//Create Group (add device)
						CreateGroup(mosq, jobj);
					}else if(TYPE.compare("ADD_DEVICE") == 0){//Add device
						AddDeviceGroup(mosq, jobj);
					}else if(TYPE.compare("REMOVE_DEVICE") == 0){//Del Device
						DelDeviceGroup(mosq, jobj);
					}else if(TYPE.compare("DELETE") == 0){//Del Group
						DelGroup(mosq, jobj);
					}
				}else {
					ControlGroup(mosq, jobj);
				}
			}
		}
	}
}

//Call EventTrigger
void CallEventTrigger(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string CMD;
		if(document.HasMember("CMD")){
			CMD = document["CMD"].GetString();
			if(CMD.compare("EVENT_TRIGGER") == 0){
				const Value& DATA = document["DATA"];
				if(DATA.HasMember("EVENT_TRIGGER_ID")){

					if(!document.HasMember("TYPE")){
						string EVENTTRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
						string sql1 = "SELECT EventTriggerTypeId FROM EventTrigger WHERE EventTriggerId = '"+ EVENTTRIGGER_ID + "';";
						DBCHECK(mosq, "ADR", sql1);
						string check = toString(checkControlEvent(mosq, ADR));
						ADR = -1;
						string sql = "SELECT SceneUnicastID FROM EventTrigger WHERE EventTriggerId = '"+ EVENTTRIGGER_ID + "';";
						DBCHECK(mosq, "ADR", sql);

						StringBuffer sendToGW;
						Writer<StringBuffer> jsonAHG(sendToGW);
						jsonAHG.StartObject();
						if(check.compare("SCENE") == 0){
							jsonAHG.Key("CALLSCENE");
							jsonAHG.Int(ADR);
							jsonAHG.Key("TIME");
							jsonAHG.Int(0);
							jsonAHG.EndObject();
						}
						string s = sendToGW.GetString();
						cout<<s<<endl;
						char * sendT = new char[s.length()+1];
						strcpy(sendT, s.c_str());
						MqttSend(mosq, sendT);
					}
				}
			}
		}
	}
}

//EventTrigger
void EventTrigger(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string CMD;
		if(document.HasMember("CMD")){
			CMD = document["CMD"].GetString();
			if(CMD.compare("EVENT_TRIGGER") == 0){
				if(document.HasMember("TYPE")){
					string TYPE = document["TYPE"].GetString();
					//CREATE OR EDIT EVENTTRIGGER
					if(TYPE.compare("CREATE") == 0){
						Document document;
						document.Parse(jobj);
						if(document.IsObject()){
							string CMD;
							if(document.HasMember("CMD")){
								CMD = document["CMD"].GetString();
								if(CMD.compare("EVENT_TRIGGER") == 0){
									if(document.HasMember("TYPE")){
										string TYPE = document["TYPE"].GetString();
										//CREATE OR EDIT EVENTTRIGGER
										if(TYPE.compare("CREATE") == 0){
											const Value& DATA = document["DATA"];
											int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
											CONTROL = "";
											string checkD = "SELECT Code FROM EventTriggerType WHERE EventTriggerTypeId="+toString(EVENT_TRIGGER_TYPE_ID)+";";
											DBCHECK(mosq, "CONTROL", checkD);
											if(CONTROL.compare("SCENE") == 0){
												CreateScene(mosq, jobj);
											}else if(CONTROL.compare("RULE") == 0){
												CreateRule(mosq, jobj);
											}
										}
									}
								}
							}
						}
					}else if (TYPE.compare("EDIT") == 0) {
						const Value& DATA = document["DATA"];
						int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
						CONTROL = "";
						string checkD = "SELECT Code FROM EventTriggerType WHERE EventTriggerTypeId="+toString(EVENT_TRIGGER_TYPE_ID)+";";
						DBCHECK(mosq, "CONTROL", checkD);
						if(CONTROL.compare("SCENE") == 0){
							EditScene(mosq, jobj);
						}else if(CONTROL.compare("RULE") == 0){
							EditRule(mosq, jobj);
						}
					}else if (TYPE.compare("DELETE") == 0) {
						const Value& DATA = document["DATA"];
						if(DATA.HasMember("EVENT_TRIGGER_ID")){
							string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
							string checkD = "SELECT Code FROM EventTriggerType INNER JOIN EventTrigger ON EventTriggerType.EventTriggerTypeId = EventTrigger.EventTriggerTypeId WHERE EventTriggerId='"+EVENT_TRIGGER_ID+"';";
							DBCHECK(mosq, "CONTROL", checkD);
							if(CONTROL.compare("SCENE") == 0){
								DelScene(mosq, jobj);
							}else if(CONTROL.compare("RULE") == 0){
								DelRule(mosq, jobj);
								StringBuffer sendToGW;
								Writer<StringBuffer> json(sendToGW);
								json.StartObject();
								json.Key("CMD");
								json.String("EVENT_TRIGGER");
								json.Key("TYPE");
								json.String("DELETE");
								json.Key("DATA");
								json.StartObject();
								json.Key("EVENT_TRIGGER_ID");
								json.String(ToChar(EVENT_TRIGGER_ID));
								json.EndObject();
								json.EndObject();

								string s = sendToGW.GetString();
								cout<<s<<endl;
								char * sendT = new char[s.length()+1];
								strcpy(sendT, s.c_str());
								MqttSendAPP(mosq, sendT);
							}
						}
					}
				}
			}
		}
	}
}

//EventTrigger Output
void EventOutput(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string CMD;
		if(document.HasMember("CMD")){
			CMD = document["CMD"].GetString();
			if(CMD.compare("EVENT_TRIGGER_OUTPUT_DEVICE_MAPPING") == 0){
				if(document.HasMember("TYPE")){
					string TYPE = document["TYPE"].GetString();
					//ADD DEVICE GROUP EVENTTRIGGER
					if(TYPE.compare("ADD") == 0){
						AddEventTriggerOuput(mosq, jobj);
					}else if (TYPE.compare("DELETE") == 0) {
						DeleteEventTriggerOuput(mosq, jobj);
					}
				}
			}
		}
	}
}

//Scan - Stop
void ScanStop(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string cmd;
		if(document.HasMember("CMD")){
			cmd = document["CMD"].GetString();
			if(cmd.compare("SCAN") == 0){
				StringBuffer sendToGW;
				Writer<StringBuffer> jsonAHG(sendToGW);
				jsonAHG.StartObject();
				jsonAHG.Key("CMD");
				jsonAHG.String("SCAN");
				jsonAHG.EndObject();

				string s = sendToGW.GetString();
				char * sendT = new char[s.length()+1];
				strcpy(sendT, s.c_str());
				MqttSend(mosq, sendT);
			}else if(cmd.compare("STOP") == 0){
				StringBuffer sendToGW;
				Writer<StringBuffer> jsonAHG(sendToGW);
				jsonAHG.StartObject();
				jsonAHG.Key("CMD");
				jsonAHG.String("STOP");
				jsonAHG.EndObject();

				string s = sendToGW.GetString();
				char * sendT = new char[s.length()+1];
				strcpy(sendT, s.c_str());
				MqttSend(mosq, sendT);
			}
		}
	}else {
		cout<<"JSON ERROR"<<endl;
	}
}

void InputSensor(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string cmd;
		if(document.HasMember("CMD")){
			cmd = document["CMD"].GetString();
			if(cmd.compare("SET_SCENE_FOR_SENSOR") == 0){
				if(document.HasMember("TYPE")){
					string TYPE = document["TYPE"].GetString();
					if(TYPE.compare("CREATE") == 0 || TYPE.compare("EDIT") == 0){
						CreateSensor(mosq, jobj);
					}else if(TYPE.compare("DELETE") == 0){
						DelSensor(mosq, jobj);
					}
				}
			}
		}
	}
}

void InputRemote(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string cmd;
		if(document.HasMember("CMD")){
			cmd = document["CMD"].GetString();
			if(cmd.compare("SET_SCENE_FOR_REMOTE") == 0){
				if(document.HasMember("TYPE")){
					string TYPE = document["TYPE"].GetString();
					if(TYPE.compare("CREATE") == 0 || TYPE.compare("EDIT") == 0){
						CreateRemote(mosq, jobj);
					}else if(TYPE.compare("DELETE") == 0){
						DelRemote(mosq, jobj);
					}
				}
			}
		}
	}
}

void InputDoorSensor(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string cmd;
		if(document.HasMember("CMD")){
			cmd = document["CMD"].GetString();
			if(cmd.compare("SCENE_FOR_DOOR_SENSOR") == 0){
				if(document.HasMember("TYPE")){
					string TYPE = document["TYPE"].GetString();
					if(TYPE.compare("CREATE") == 0 || TYPE.compare("EDIT") == 0){
						CreateDoorSensor(mosq, jobj);
					}else if(TYPE.compare("DELETE") == 0){
//						DelDoorSensor(mosq, jobj);
					}
				}
			}
		}
	}
}

//--------------------------------------------GROUP------------------------------------------------------------------

void CreateGroup(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	if(DATA.HasMember("GROUP_ID") && DATA.HasMember("CATEGORY_ID") && DATA.HasMember("NAME")){
		string GROUP_ID = DATA["GROUP_ID"].GetString();
		int CATEGORY_ID = DATA["CATEGORY_ID"].GetInt();
		string NAME = DATA["NAME"].GetString();
		//ADD DEVICE TO GROUP
		int GroupUnicastId = createGroupId(mosq);
		string creGroupID = "INSERT OR REPLACE INTO GROUPID (GroupUnicastId, GroupingId, ValueCreate)"
				" values ("+ toString(GroupUnicastId) + ",'"+ GROUP_ID +"', 1); ";
		DBSQL(mosq, creGroupID);

		string creGroup = "INSERT OR REPLACE INTO GROUPING (GroupingId, GroupUnicastId, Name, CategoryId)"
				" values ('"+ GROUP_ID + "', "+ toString(GroupUnicastId) + ", '"+ NAME + "', "+ toString(CATEGORY_ID) + "); ";
		DBSQL(mosq, creGroup);


		if(DATA.HasMember("DEVICES")){
			const Value& DEVICES = DATA["DEVICES"];
			//json
			StringBuffer sendToGW;
			Writer<StringBuffer> jsonAHG(sendToGW);
			jsonAHG.StartObject();
			jsonAHG.Key("ADR");
			jsonAHG.StartArray();
			for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
				const Value& a = DEVICES[i];
				string DEVICE_ID = a.GetString();
				int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
				//ADD DEVICE TO GROUP
				string addDeviceGr = "INSERT OR REPLACE INTO GroupingDeviceMapping (GroupUnicastId, GroupingId, DeviceId, DeviceUnicastId)"
						" values ("+ toString(GroupUnicastId) + ",'"+ GROUP_ID + "', '"+ DEVICE_ID + "' ,"+ toString(DEVICE_UNICAST) + ");";
				DBSQL(mosq, addDeviceGr);
				jsonAHG.Int(DEVICE_UNICAST);
			}
			jsonAHG.EndArray();
			jsonAHG.Key("ADDGROUP");
			jsonAHG.Int(GroupUnicastId);
			jsonAHG.EndObject();

			string s = sendToGW.GetString();
			cout<<s<<endl;
			char * sendT = new char[s.length()+1];
			strcpy(sendT, s.c_str());
			MqttSend(mosq, sendT);
		}
	}
}

void AddDeviceGroup(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	if(DATA.HasMember("GROUP_ID") && DATA.HasMember("DEVICES")){
		string GROUP_ID = DATA["GROUP_ID"].GetString();
		int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);
		const Value& DEVICES = DATA["DEVICES"];

		//json
		StringBuffer sendToGW;
		Writer<StringBuffer> jsonAHG(sendToGW);
		jsonAHG.StartObject();
		jsonAHG.Key("ADR");
		jsonAHG.StartArray();
		for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
			const Value& a = DEVICES[i];
			string DEVICE_ID = a.GetString();
			int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
			string addDeviceGr = "INSERT OR REPLACE INTO GroupingDeviceMapping (GroupingId, GroupUnicastId, DeviceId, DeviceUnicastId)"
					" values ('"+ GROUP_ID + "', "+ toString(GROUP_UNICAST) + ",'"+ DEVICE_ID + "' ,"+ toString(DEVICE_UNICAST) + ");";
			DBSQL(mosq, addDeviceGr);
			jsonAHG.Int(DEVICE_UNICAST);
		}
		jsonAHG.EndArray();
		jsonAHG.Key("ADDGROUP");
		jsonAHG.Int(GROUP_UNICAST);
		jsonAHG.EndObject();

		string s = sendToGW.GetString();
		char * sendT = new char[s.length()+1];
		strcpy(sendT, s.c_str());
		MqttSend(mosq, sendT);
	}
}

void DelDeviceGroup(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	if(DATA.HasMember("GROUP_ID") && DATA.HasMember("DEVICES")){
		string GROUP_ID = DATA["GROUP_ID"].GetString();
		int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);
		const Value& DEVICES = DATA["DEVICES"];
		//json
		StringBuffer sendToGW;
		Writer<StringBuffer> jsonAHG(sendToGW);
		jsonAHG.StartObject();
		jsonAHG.Key("ADR");
		jsonAHG.StartArray();
		for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
			const Value& a = DEVICES[i];
			string DEVICE_ID = a.GetString();
			int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
			string delDeviceGr = "DELETE FROM GroupingDeviceMapping WHERE GroupingId = '" + GROUP_ID + "' "
					"AND DeviceId = '" + DEVICE_ID +"' AND GroupUnicastId = " + toString(GROUP_UNICAST) +" "
							"AND DeviceUnicastId = " + toString(DEVICE_UNICAST) +";";
			DBSQL(mosq, delDeviceGr);
			jsonAHG.Int(DEVICE_UNICAST);
		}
		jsonAHG.EndArray();
		jsonAHG.Key("DELGROUP");
		jsonAHG.Int(GROUP_UNICAST);
		jsonAHG.EndObject();

		string s = sendToGW.GetString();
		char * sendT = new char[s.length()+1];
		strcpy(sendT, s.c_str());
		MqttSend(mosq, sendT);
	}
}

void DelGroup(struct mosquitto *mosq, char* jobj){
	string demo = toString(jobj);
	DBJSON(mosq, "DELGROUP", demo);
}

void ControlGroup(struct mosquitto *mosq, char* jobj){
	int R = -1;
	int G = -1;
	int B = -1;
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	if(DATA.HasMember("GROUP_ID") && DATA.HasMember("PROPERTIES")){
		string GROUP_ID = DATA["GROUP_ID"].GetString();
		int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);
		const Value& PROPERTIES = DATA["PROPERTIES"];
		for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
			const Value& a = PROPERTIES[i];

			if(a.HasMember("ID") && a.HasMember("VALUE")){
				int ID = a["ID"].GetInt();
				int VALUE = a["VALUE"].GetInt();
				char* controlRGB;
				controlRGB = checkControl(mosq, a["ID"].GetInt());
				if(toString(controlRGB).compare("HUE") == 0 || toString(controlRGB).compare("SATURATION") == 0 || toString(controlRGB).compare("LIGHTNESS") == 0){
					if(toString(controlRGB).compare("HUE") == 0){
						R = a["VALUE"].GetInt();
					}else if(toString(controlRGB).compare("SATURATION") == 0){
						G = a["VALUE"].GetInt();
					}else if(toString(controlRGB).compare("LIGHTNESS") == 0){
						B = a["VALUE"].GetInt();
					}

					if(R >= 0 && G >= 0 && B >= 0){
						StringBuffer sendToGW;
						Writer<StringBuffer> jsonAHG(sendToGW);
						jsonAHG.StartObject();
						jsonAHG.Key("ADR");
						jsonAHG.Int(GROUP_UNICAST);
						jsonAHG.Key("HUE");
						jsonAHG.Int(R);
						jsonAHG.Key("SATURATION");
						jsonAHG.Int(G);
						jsonAHG.Key("LIGHTNESS");
						jsonAHG.Int(B);
						jsonAHG.Key("TIME");
						jsonAHG.Int(0);
						jsonAHG.EndObject();
						string s = sendToGW.GetString();
						char * sendT = new char[s.length()+1];
						strcpy(sendT, s.c_str());
						MqttSend(mosq, sendT);
						R = -1;
						G = -1;
						B = -1;
					}
				}else {
					//SEND TO GW
					SendDeviceToGW(mosq, GROUP_UNICAST, ID, VALUE);
				}
			}
		}
	}
}

//--------------------------------------------EVENTTRIGGER------------------------------------------------------------------

void CreateScene(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
	string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
	string GROUP_ID = DATA["GROUP_ID"].GetString();
	int PRIORITY = DATA["PRIORITY"].GetInt();
	int HAS_TIMER = DATA["HAS_TIMER"].GetInt();
	string START_AT, END_AT;
	if(HAS_TIMER == 1){
		START_AT = DATA["START_AT"].GetString();
		END_AT = DATA["END_AT"].GetString();
	}
	int SceneId = createSceneId(mosq);
	string creGroupID = "INSERT OR REPLACE INTO EventTriggerID (SceneUnicastID, EventTriggerId, ValueCreate)"
			" values ("+ toString(SceneId) + ",'"+ EVENT_TRIGGER_ID +"', 1); ";
	DBSQL(mosq, creGroupID);
	string addEvent = "INSERT OR REPLACE INTO EventTrigger (EventTriggerId, GroupId, EventTriggerTypeId, SceneUnicastID, "
			"HasTimer, Priority, StartAt, EndAt) "
			"values ('" + EVENT_TRIGGER_ID + "', '" + GROUP_ID + "', " + toString(EVENT_TRIGGER_TYPE_ID) + ", " + toString(SceneId) + ","
					"" + toString(HAS_TIMER) + "," + toString(PRIORITY) + ",'" + START_AT + "','" + END_AT + "');";
	DBSQL(mosq, addEvent);
	if(DATA.HasMember("EACH_DAY")){
		const Value& EACH_DAY = DATA["EACH_DAY"];
		for (rapidjson::SizeType i = 0; i < EACH_DAY.Size(); i++){
			string a = EACH_DAY[i].GetString();
			string updateDay = "UPDATE EventTrigger SET " + a + " = 1 WHERE EventTriggerId= '"+EVENT_TRIGGER_ID+"';";
			DBSQL(mosq, updateDay);
		}
	}
	string check = toString(checkControlEvent(mosq, EVENT_TRIGGER_TYPE_ID));
	//DEVICE
	if(DATA.HasMember("DEVICES")){
		const Value& DEVICES = DATA["DEVICES"];
		//ADD DEVICE SCENE
		StringBuffer sendToGW;
		Writer<StringBuffer> jsonAHG(sendToGW);
		jsonAHG.StartObject();
		jsonAHG.Key("ADR");
		jsonAHG.StartArray();

		for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
			const Value& a = DEVICES[i];
			string DEVICE_ID = a["DEVICE_ID"].GetString();
			int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
			const Value& PROPERTIES = a["PROPERTIES"];

			string addDeviceS = "INSERT OR REPLACE INTO EventTriggerOutputDeviceMapping(EventTriggerId, DeviceId, DeviceUnicastId) "
					"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+");";
			DBSQL(mosq, addDeviceS);
			if(check.compare("SCENE") == 0){
				jsonAHG.Int(DEVICE_UNICAST);
			}
			for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
				const Value& b = PROPERTIES[i];
				int ID = b["ID"].GetInt();
				int VALUE = b["VALUE"].GetInt();

				string addDeviceS = "INSERT OR REPLACE INTO EventTriggerOutputDeviceSetupValue"
						"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, DeviceAttributeValue) "
						"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(ID)+", "+toString(VALUE)+");";
				DBSQL(mosq, addDeviceS);
			}
		}
		jsonAHG.EndArray();
		jsonAHG.Key("ADDSCENE");
		jsonAHG.Int(SceneId);
		jsonAHG.EndObject();

		string s = sendToGW.GetString();
		char * sendT = new char[s.length()+1];
		strcpy(sendT, s.c_str());
		MqttSend(mosq, sendT);
	}
	//GROUP
	if(DATA.HasMember("GROUPS")){
		const Value& GROUPS = DATA["GROUPS"];
		for (rapidjson::SizeType i = 0; i < GROUPS.Size(); i++){
			const Value& a = GROUPS[i];
			string GROUP_ID = a["GROUP_ID"].GetString();
			int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);
			const Value& PROPERTIES = a["PROPERTIES"];
			string addGroupS = "INSERT OR REPLACE INTO EventTriggerOutputGroupingMapping(EventTriggerId, GroupingId, GroupUnicastId) "
					"values ('"+EVENT_TRIGGER_ID+"', '"+GROUP_ID+"', "+toString(GROUP_UNICAST)+");";
			DBSQL(mosq, addGroupS);
			for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
				const Value& b = PROPERTIES[i];
				int ID = b["ID"].GetInt();
				int VALUE = b["VALUE"].GetInt();
				string addGroupSVa = "INSERT OR REPLACE INTO EventTriggerOutputGroupingSetupValue"
						"(EventTriggerId, GroupingId, GroupUnicastId, DeviceAttributeId, DeviceAttributeValue) "
						"values ('"+EVENT_TRIGGER_ID+"', '"+GROUP_ID+"', "+toString(GROUP_UNICAST)+", "+toString(ID)+", "+toString(VALUE)+");";
				DBSQL(mosq, addGroupSVa);
			}
		}
		string demo = toString(jobj);
		DBJSON(mosq, "ADDGROUPTOEVENT", demo);
	}
	CONTROL="";
	string checkEvent = "SELECT EventTriggerId FROM EVENTTRIGGER WHERE GroupId = '" + GROUP_ID +"';";
	DBCHECK(mosq, "CONTROL", checkEvent);
	if(CONTROL.compare(EVENT_TRIGGER_ID) == 0){
		StringBuffer sendToGW;
		Writer<StringBuffer> jsonAHG(sendToGW);
		jsonAHG.StartObject();
		jsonAHG.Key("CMD");
		jsonAHG.String("EVENT_TRIGGER");
		jsonAHG.Key("TYPE");
		jsonAHG.String("CREATE");
		jsonAHG.Key("DATA");
		jsonAHG.StartObject();
		jsonAHG.Key("EVENT_TRIGGER_ID");
		jsonAHG.String(ToChar(EVENT_TRIGGER_ID));
		jsonAHG.Key("GROUP_ID");
		jsonAHG.String(ToChar(GROUP_ID));
		jsonAHG.Key("SCENE_UNICAST_ID");
		jsonAHG.Int(SceneId);
		jsonAHG.Key("EVENT_TRIGGER_TYPE_ID");
		jsonAHG.Int(EVENT_TRIGGER_TYPE_ID);
		jsonAHG.Key("PRIORITY");
		jsonAHG.Int(PRIORITY);
		jsonAHG.Key("HAS_TIMER");
		jsonAHG.Int(HAS_TIMER);
		if(HAS_TIMER == 1){
			jsonAHG.Key("START_AT");
			jsonAHG.String(ToChar(START_AT));
			jsonAHG.Key("END_AT");
			jsonAHG.String(ToChar(END_AT));
		}
		jsonAHG.EndObject();
		jsonAHG.EndObject();
		string s = sendToGW.GetString();
		char * sendT = new char[s.length()+1];
		strcpy(sendT, s.c_str());
		MqttSendAPP(mosq, sendT);
		cout<<s<<endl;
	}
}

void EditScene(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
	string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
	string GROUP_ID = DATA["GROUP_ID"].GetString();
	int PRIORITY = DATA["PRIORITY"].GetInt();
	int HAS_TIMER = DATA["HAS_TIMER"].GetInt();
	string START_AT, END_AT;
	if(HAS_TIMER == 1){
		START_AT = DATA["START_AT"].GetString();
		END_AT = DATA["END_AT"].GetString();
	}

	int SCENE_UNICAST = SCENE_UNICAST_ID(EVENT_TRIGGER_ID);

	string addEvent = "INSERT OR REPLACE INTO EventTrigger (EventTriggerId, GroupId, EventTriggerTypeId, SceneUnicastID, "
			"HasTimer, Priority, StartAt, EndAt, ValueCreate) "
			"values ('" + EVENT_TRIGGER_ID + "', '" + GROUP_ID + "', " + toString(EVENT_TRIGGER_TYPE_ID) + ", " + toString(SCENE_UNICAST) + ","
					"" + toString(HAS_TIMER) + "," + toString(PRIORITY) + ",'" + START_AT + "','" + END_AT + "', 1);";
	DBSQL(mosq, addEvent);

	string check = "SELECT EventTriggerId FROM EVENTTRIGGER WHERE GroupId = '" + GROUP_ID +"';";
	DBCHECK(mosq, "CONTROL", check);
	string Update = "UPDATE EventTrigger SET EachMonday = 0, EachTuesday = 0, EachWednesday = 0, EachThursday = 0, "
			"EachFirday = 0, EachSaturday = 0, EachSunday = 0"
			" WHERE EventTriggerId= '"+EVENT_TRIGGER_ID+"';";
	DBSQL(mosq, Update);
	if(DATA.HasMember("EACH_DAY")){
		const Value& EACH_DAY = DATA["EACH_DAY"];
		for (rapidjson::SizeType i = 0; i < EACH_DAY.Size(); i++){
			string a = EACH_DAY[i].GetString();

			string updateDay = "UPDATE EventTrigger SET " + a + " = 1 WHERE EventTriggerId= '"+EVENT_TRIGGER_ID+"';";
			DBSQL(mosq, updateDay);
		}
	}
	if(CONTROL.compare(EVENT_TRIGGER_ID) == 0){
		MqttSendAPP(mosq, jobj);
	}
}

void DelScene(struct mosquitto *mosq, char* jobj){
	MqttSendAPP(mosq, jobj);
	string demo = toString(jobj);
	DBJSON(mosq, "DELSCENE", demo);
}

void CreateRule(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string CMD;
		if(document.HasMember("CMD")){
			CMD = document["CMD"].GetString();
			if(CMD.compare("EVENT_TRIGGER") == 0){
				const Value& DATA = document["DATA"];
				string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
				string GROUP_ID = DATA["GROUP_ID"].GetString();
				int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
				int PRIORITY = DATA["PRIORITY"].GetInt();
				int HAS_TIMER = DATA["HAS_TIMER"].GetInt();
				int FADE_IN;
				if(DATA.HasMember("FADE_IN")){
					FADE_IN = DATA["FADE_IN"].GetInt();
				}
				string START_AT, END_AT;
				if(HAS_TIMER == 1){
					START_AT = DATA["START_AT"].GetString();
					END_AT = DATA["END_AT"].GetString();
				}

				int LOGICAL_OPERATOR_ID= DATA["LOGICAL_OPERATOR_ID"].GetInt();
//				int STATUS_ID = DATA["STATUS_ID"].GetInt();

				string addEvent = "INSERT OR REPLACE INTO EventTrigger (EventTriggerId, GroupId, EventTriggerTypeId, "
						"HasTimer, Priority, StartAt, EndAt, LogicalOperatorID, FADE_IN) "
						"values ('" + EVENT_TRIGGER_ID + "', '" + GROUP_ID + "', " + toString(EVENT_TRIGGER_TYPE_ID) + ","
								"" + toString(HAS_TIMER) + "," + toString(PRIORITY) + ",'" + START_AT + "','" + END_AT + "', "+toString(LOGICAL_OPERATOR_ID)+", "+toString(FADE_IN)+");";
				DBSQL(mosq, addEvent);


				if(DATA.HasMember("EACH_DAY")){
					const Value& EACH_DAY = DATA["EACH_DAY"];
					for (rapidjson::SizeType i = 0; i < EACH_DAY.Size(); i++){
						string a = EACH_DAY[i].GetString();

						string updateDay = "UPDATE EventTrigger SET " + a + " = 1 WHERE EventTriggerId= '"+EVENT_TRIGGER_ID+"';";
						DBSQL(mosq, updateDay);
					}
				}

				//DEVICE
				if(DATA.HasMember("DEVICES")){
					const Value& DEVICES = DATA["DEVICES"];
					for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
						const Value& a = DEVICES[i];
						string DEVICE_ID = a["DEVICE_ID"].GetString();
						int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
						const Value& PROPERTIES = a["PROPERTIES"];

						string addDeviceS = "INSERT OR REPLACE INTO EventTriggerOutputDeviceMapping(EventTriggerId, DeviceId, DeviceUnicastId) "
								"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+");";
						DBSQL(mosq, addDeviceS);

						for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
							const Value& b = PROPERTIES[i];
							int ID = b["ID"].GetInt();
							int VALUE = b["VALUE"].GetInt();

							string addDeviceS = "INSERT OR REPLACE INTO EventTriggerOutputDeviceSetupValue"
									"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, DeviceAttributeValue) "
									"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(ID)+", "+toString(VALUE)+");";
							DBSQL(mosq, addDeviceS);
						}
					}
				}

				//GROUP
				if(DATA.HasMember("GROUPS")){
					const Value& GROUPS = DATA["GROUPS"];
					for (rapidjson::SizeType i = 0; i < GROUPS.Size(); i++){
						const Value& a = GROUPS[i];
						string GROUP_ID = a["GROUP_ID"].GetString();
						int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);
						const Value& PROPERTIES = a["PROPERTIES"];

						string addGroupS = "INSERT OR REPLACE INTO EventTriggerOutputGroupingMapping(EventTriggerId, GroupingId, GroupUnicastId) "
								"values ('"+EVENT_TRIGGER_ID+"', '"+GROUP_ID+"', "+toString(GROUP_UNICAST)+");";
						DBSQL(mosq, addGroupS);

						for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
							const Value& b = PROPERTIES[i];
							int ID = b["ID"].GetInt();
							int VALUE = b["VALUE"].GetInt();
							string addGroupSVa = "INSERT OR REPLACE INTO EventTriggerOutputGroupingSetupValue"
									"(EventTriggerId, GroupingId, GroupUnicastId, DeviceAttributeId, DeviceAttributeValue) "
									"values ('"+EVENT_TRIGGER_ID+"', '"+GROUP_ID+"', "+toString(GROUP_UNICAST)+", "+toString(ID)+", "+toString(VALUE)+");";
							DBSQL(mosq, addGroupSVa);
						}
					}
				}

				//GROUP
				if(DATA.HasMember("SCENE")){
					const Value& SCENE = DATA["SCENE"];
					for (rapidjson::SizeType i = 0; i < SCENE.Size(); i++){
						const Value& a = SCENE[i];
						string SCENE_ID = a["SCENE_ID"].GetString();
						string check = "SELECT SceneUnicastID FROM EventTrigger WHERE GroupId = '"+SCENE_ID+"';";
						ADR = -1;
						DBCHECK(mosq, "ADR", check);
						string addSceneS = "INSERT OR REPLACE INTO EventTriggerOutputSceneMapping(EventTriggerId, SceneId, SceneUnicastId) "
								"values ('"+EVENT_TRIGGER_ID+"', '"+SCENE_ID+"', "+toString(ADR)+");";
						DBSQL(mosq, addSceneS);
						cout<<addSceneS<<endl;
					}
				}


				StringBuffer sendToGW;
				Writer<StringBuffer> json(sendToGW);
				json.StartObject();
				json.Key("CMD");
				json.String("EVENT_TRIGGER");
				json.Key("DATA");
				json.StartObject();
				json.Key("EVENT_TRIGGER_ID");
				json.String(ToChar(EVENT_TRIGGER_ID));
				json.Key("STATUS");
				json.String("SUCCESS");
				json.EndObject();
				json.EndObject();

				string s = sendToGW.GetString();
				cout<<s<<endl;
				char * sendT = new char[s.length()+1];
				strcpy(sendT, s.c_str());
				MqttSendAPP(mosq, sendT);
			}
		}
	}
}

void EditRule(struct mosquitto *mosq, char* jobj){
	DelRule(mosq, jobj);
	CreateRule(mosq, jobj);
}

void DelRule(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string CMD;
		if(document.HasMember("CMD")){
			CMD = document["CMD"].GetString();
			if(CMD.compare("EVENT_TRIGGER") == 0){
				const Value& DATA = document["DATA"];
				string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();

				string sqlEV = "DELETE FROM EVENTTRIGGER WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"';";
				DBSQL(mosq, sqlEV);

				string sqlD = "DELETE FROM EventTriggerOutputDeviceMapping WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"';";
				DBSQL(mosq, sqlD);

				string sqlG = "DELETE FROM EventTriggerOutputGroupingMapping WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"';";
				DBSQL(mosq, sqlG);

				string sqlDV = "DELETE FROM EventTriggerOutputDeviceSetupValue WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"';";
				DBSQL(mosq, sqlDV);

				string sqlGV = "DELETE FROM EventTriggerOutputGroupingSetupValue WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"';";
				DBSQL(mosq, sqlGV);

				string sqlSV = "DELETE FROM EventTriggerOutputSceneMapping WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"';";
				DBSQL(mosq, sqlSV);
			}
		}
	}
}

//--------------------------------------------EVENTTRIGGER OUTPUT------------------------------------------------------------------

void AddEventTriggerOuput(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	if(DATA.HasMember("EVENT_TRIGGER_ID") && DATA.HasMember("GUID")
		 && DATA.HasMember("EVENT_TRIGGER_TYPE_ID")){
		int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
		string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
		string GUID = DATA["GUID"].GetString();
		int SCENE_UNICAST = SCENE_UNICAST_ID(EVENT_TRIGGER_ID);
		string check = toString(checkControlEvent(mosq, EVENT_TRIGGER_TYPE_ID));
		//DEVICE
		if(DATA.HasMember("DEVICES")){
			const Value& DEVICES = DATA["DEVICES"];
			//ADD DEVICE SCENE
			StringBuffer sendToGW;
			Writer<StringBuffer> jsonAHG(sendToGW);
			jsonAHG.StartObject();
			jsonAHG.Key("ADR");
			jsonAHG.StartArray();
			for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
				const Value& a = DEVICES[i];
				string DEVICE_ID = a["DEVICE_ID"].GetString();
				int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
				const Value& PROPERTIES = a["PROPERTIES"];

				string addDeviceS = "INSERT OR REPLACE INTO EventTriggerOutputDeviceMapping(EventTriggerId, DeviceId, DeviceUnicastId) "
						"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+");";
				DBSQL(mosq, addDeviceS);
				for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
					const Value& b = PROPERTIES[i];
					int ID = b["ID"].GetInt();
					int VALUE = b["VALUE"].GetInt();

					string addDeviceS = "INSERT OR REPLACE INTO EventTriggerOutputDeviceSetupValue"
							"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, DeviceAttributeValue) "
							"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(ID)+", "+toString(VALUE)+");";
					DBSQL(mosq, addDeviceS);
					if(check.compare("SCENE") == 0){
						jsonAHG.Int(DEVICE_UNICAST);
					}
				}
			}
			jsonAHG.EndArray();
			jsonAHG.Key("ADDSCENE");
			jsonAHG.Int(SCENE_UNICAST);
			jsonAHG.EndObject();

			string s = sendToGW.GetString();
			cout<<s<<endl;
			char * sendT = new char[s.length()+1];
			strcpy(sendT, s.c_str());
			MqttSend(mosq, sendT);
		}
		//GROUP
		if(DATA.HasMember("GROUPS")){
			const Value& GROUPS = DATA["GROUPS"];
			for (rapidjson::SizeType i = 0; i < GROUPS.Size(); i++){
				const Value& a = GROUPS[i];
				string GROUP_ID = a["GROUP_ID"].GetString();
				int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);
				const Value& PROPERTIES = a["PROPERTIES"];

				string addGroupS = "INSERT OR REPLACE INTO EventTriggerOutputGroupingMapping(EventTriggerId, GroupingId, GroupUnicastId) "
						"values ('"+EVENT_TRIGGER_ID+"', '"+GROUP_ID+"', "+toString(GROUP_UNICAST)+");";
				DBSQL(mosq, addGroupS);
				for (rapidjson::SizeType i = 0; i < PROPERTIES.Size(); i++){
					const Value& b = PROPERTIES[i];
					int ID = b["ID"].GetInt();
					int VALUE = b["VALUE"].GetInt();
					string addGroupSVa = "INSERT OR REPLACE INTO EventTriggerOutputGroupingSetupValue"
							"(EventTriggerId, GroupingId, GroupUnicastId, DeviceAttributeId, DeviceAttributeValue) "
							"values ('"+EVENT_TRIGGER_ID+"', '"+GROUP_ID+"', "+toString(GROUP_UNICAST)+", "+toString(ID)+", "+toString(VALUE)+");";
					DBSQL(mosq, addGroupSVa);
				}
			}
			string demo = toString(jobj);
			DBJSON(mosq, "ADDGROUPTOEVENT", demo);
		}
	}
}

void DeleteEventTriggerOuput(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	if(DATA.HasMember("EVENT_TRIGGER_ID") && DATA.HasMember("GUID")
			 && DATA.HasMember("EVENT_TRIGGER_TYPE_ID")){
		string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
		string GUID = DATA["GUID"].GetString();
		int SCENE_UNICAST = SCENE_UNICAST_ID(EVENT_TRIGGER_ID);
		int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();

		string check = toString(checkControlEvent(mosq, EVENT_TRIGGER_TYPE_ID));
		if(DATA.HasMember("DEVICES")){
			const Value& DEVICES = DATA["DEVICES"];
			StringBuffer sendToGW;
			Writer<StringBuffer> jsonAHG(sendToGW);
			jsonAHG.StartObject();
			jsonAHG.Key("ADR");
			jsonAHG.StartArray();
			for (rapidjson::SizeType i = 0; i < DEVICES.Size(); i++){
				const Value& a = DEVICES[i];
				string DEVICE_ID = a.GetString();
				int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
				string delDevice = "DELETE FROM EventTriggerOutputDeviceMapping "
						"WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"' AND DeviceId = '"+DEVICE_ID+"' AND DeviceUnicastId = "+toString(DEVICE_UNICAST)+";";
				DBSQL(mosq, delDevice);
				string delDeviceValue = "DELETE FROM EventTriggerOutputDeviceSetupValue "
						"WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"' AND DeviceId = '"+DEVICE_ID+"' AND DeviceUnicastId = "+toString(DEVICE_UNICAST)+";";
				DBSQL(mosq, delDeviceValue);
				jsonAHG.Int(DEVICE_UNICAST);
			}
			jsonAHG.EndArray();
			if(check.compare("SCENE") == 0){
				jsonAHG.Key("DELSCENE");
				jsonAHG.Int(SCENE_UNICAST);
			}
			jsonAHG.EndObject();
			string s = sendToGW.GetString();
			char * sendT = new char[s.length()+1];
			strcpy(sendT, s.c_str());
			MqttSend(mosq, sendT);
		}
		if(DATA.HasMember("GROUPS")){
			const Value& GROUPS = DATA["GROUPS"];
			for (rapidjson::SizeType i = 0; i < GROUPS.Size(); i++){
				const Value& a = GROUPS[i];
				string GROUP_ID = a.GetString();
				int GROUP_UNICAST = GROUP_UNICAST_ID(GROUP_ID);
				string delGroup = "DELETE FROM EventTriggerOutputGroupingMapping "
						"WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"' AND GroupingId = '"+GROUP_ID+"' AND DeviceUnicastId = "+toString(GROUP_UNICAST)+";";
				DBSQL(mosq, delGroup);
				string delGroupValue = "DELETE FROM EventTriggerOutputGroupingSetupValue "
						"WHERE EventTriggerId = '"+EVENT_TRIGGER_ID+"' AND GroupingId = '"+GROUP_ID+"' AND DeviceUnicastId = "+toString(GROUP_UNICAST)+";";
				DBSQL(mosq, delGroupValue);
			}
			string demo = toString(jobj);
			DBJSON(mosq, "DELGROUPTOEVENT", demo);
		}
	}
}

//--------------------------------------------INPUT SENSOR------------------------------------------------

void CreateSensor(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	if(DATA.HasMember("DEVICE_ID") && DATA.HasMember("EVENT_TRIGGER_ID")){
		int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
		string EventTypeid = toString(checkControlEvent(mosq, EVENT_TRIGGER_TYPE_ID));
		string DEVICE_ID = DATA["DEVICE_ID"].GetString();
		int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
		string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
		if(EventTypeid.compare("SCENE") == 0){
			ADR =-1;
			string sqlId = "SELECT SceneUnicastID FROM EventTrigger WHERE EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
			DBCHECK(mosq, "ADR", sqlId);

			int dem =0;
			//to GW
			StringBuffer sendToGW;
			Writer<StringBuffer> json(sendToGW);
			json.StartObject();
			json.Key("CMD");
			json.String("SETSCENEFORSENSOR");
			json.Key("ADR");
			json.Int(DEVICE_UNICAST);
			json.Key("TYPE");
			if(DATA.HasMember("LIGHT_SENSOR") && !DATA.HasMember("PIR_SENSOR")){
				const Value& LIGHT_SENSOR = DATA["LIGHT_SENSOR"];
				dem = 0;
				json.Int(dem);
				json.Key("LIGHT_SENSOR");
				json.StartObject();
				int COMPAIRISON_OPERATOR_ID = LIGHT_SENSOR["COMPARISON_OPERATOR_ID"].GetInt();
				json.Key("CONDITION");
				json.Int(COMPAIRISON_OPERATOR_ID);

				if(LIGHT_SENSOR.HasMember("LOW_LUX") && !LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();
					json.Key("LOW_LUX");
					json.Int(LOW_LUX);
				}
				if(!LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LUX_MAX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();
					json.Key("HIGHT_LUX");
					json.Int(LUX_MAX);
				}
				if(LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();
					int LUX_MAX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();
					json.Key("LOW_LUX");
					json.Int(LOW_LUX);
					json.Key("HIGHT_LUX");
					json.Int(LUX_MAX);
				}
				json.EndObject();
				json.Key("SCENEID");
				json.Int(ADR);
				json.Key("SRGBID");
				json.Int(0);
				json.EndObject();

			}
			if(!DATA.HasMember("LIGHT_SENSOR") && DATA.HasMember("PIR_SENSOR")){
				const Value& PIR_SENSOR = DATA["PIR_SENSOR"];
				int PIR = PIR_SENSOR["PIR"].GetInt();
				int COMPAIRISON_OPERATOR_ID = PIR_SENSOR["COMPARISON_OPERATOR_ID"].GetInt();
				dem = 1;
				json.Int(dem);
				json.Key("PIR_SENSOR");
				json.StartObject();
				json.Key("CONDITION");
				json.Int(COMPAIRISON_OPERATOR_ID);
				json.Key("PIR");
				json.Int(PIR);
				json.EndObject();
				json.Key("SCENEID");
				json.Int(ADR);
				json.Key("SRGBID");
				json.Int(0);
				json.EndObject();
			}
			if(DATA.HasMember("LIGHT_SENSOR") && DATA.HasMember("PIR_SENSOR")){
				const Value& PIR_SENSOR = DATA["PIR_SENSOR"];
				const Value& LIGHT_SENSOR = DATA["LIGHT_SENSOR"];
				int COMPAIRISON_OPERATOR_ID_LUX = LIGHT_SENSOR["COMPARISON_OPERATOR_ID"].GetInt();
				int COMPAIRISON_OPERATOR_ID_PIR = PIR_SENSOR["COMPARISON_OPERATOR_ID"].GetInt();
				int PIR = PIR_SENSOR["PIR"].GetInt();

				dem = 2;
				json.Int(dem);
				json.Key("LIGHT_SENSOR");
				json.StartObject();
				json.Key("CONDITION");
				json.Int(COMPAIRISON_OPERATOR_ID_LUX);
				if(LIGHT_SENSOR.HasMember("LOW_LUX") && !LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();
					json.Key("LOW_LUX");
					json.Int(LOW_LUX);
				}
				if(!LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LUX_MAX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();
					json.Key("HIGHT_LUX");
					json.Int(LUX_MAX);
				}
				if(LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();
					int LUX_MAX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();
					json.Key("LOW_LUX");
					json.Int(LOW_LUX);
					json.Key("HIGHT_LUX");
					json.Int(LUX_MAX);
				}
				json.EndObject();
				json.Key("PIR_SENSOR");
				json.StartObject();
				json.Key("CONDITION");
				json.Int(COMPAIRISON_OPERATOR_ID_PIR);
				json.Key("PIR");
				json.Int(PIR);
				json.EndObject();
				json.Key("SCENEID");
				json.Int(ADR);
				json.Key("SRGBID");
				json.Int(0);
				json.EndObject();
			}
			string s = sendToGW.GetString();
			cout<<s<<endl;
			char * sendT = new char[s.length()+1];
			strcpy(sendT, s.c_str());
			MqttSend(mosq, sendT);
		}else if (EventTypeid.compare("RULE") == 0) {
			if(DATA.HasMember("LIGHT_SENSOR")){
				int a = checkDeviceAttribute(mosq, "LUX");
				const Value& LIGHT_SENSOR = DATA["LIGHT_SENSOR"];
				int COMPAIRISON_OPERATOR_ID = LIGHT_SENSOR["COMPARISON_OPERATOR_ID"].GetInt();

				if(LIGHT_SENSOR.HasMember("LOW_LUX") && !LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();

					string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
							"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue)"
							"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(a)+","+toString(COMPAIRISON_OPERATOR_ID)+","+toString(LOW_LUX)+")";
					DBSQL(mosq, add2);
				}
				if(!LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int HIGHT_LUX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();

					string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
							"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValueMAX)"
							"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(a)+","+toString(COMPAIRISON_OPERATOR_ID)+","+toString(HIGHT_LUX)+")";
					DBSQL(mosq, add2);
				}
				if(LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
					int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();
					int HIGHT_LUX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();

					string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
							"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue, DeviceAttributeValueMAX)"
							"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(a)+","+toString(COMPAIRISON_OPERATOR_ID)+","+toString(LOW_LUX)+","+toString(HIGHT_LUX)+")";
					DBSQL(mosq, add2);
				}

			}
			if(DATA.HasMember("PIR_SENSOR")){
				const Value& PIR_SENSOR = DATA["PIR_SENSOR"];
				int a = checkDeviceAttribute(mosq, "PIR");
				int PIR = PIR_SENSOR["PIR"].GetInt();
				int COMPAIRISON_OPERATOR_ID = PIR_SENSOR["COMPARISON_OPERATOR_ID"].GetInt();

				string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
						"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue)"
						"values ('"+EVENT_TRIGGER_ID+"', '"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(a)+","+toString(COMPAIRISON_OPERATOR_ID)+","+toString(PIR)+")";
				DBSQL(mosq, add2);
			}
		}
		MqttSendAPP(mosq, jobj);
	}
}

void DelSensor(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	string DEVICE_ID = DATA["DEVICE_ID"].GetString();
	string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
	StringBuffer sendToGW;
	Writer<StringBuffer> json(sendToGW);
	json.StartObject();
	json.Key("CMD");
	json.String("DELSCENEFORSENSOR");
	json.Key("ADR");
	ADR = -1;
	string adrD = "SELECT DeviceUnicastId FROM Device WHERE DeviceId ='"+DEVICE_ID+"';";
	DBCHECK(mosq, "ADR", adrD);
	json.Int(ADR);
	json.Key("SCENEID");
	ADR = -1;
	string adrET = "SELECT SceneUnicastID FROM EventTrigger WHERE EventTriggerId ='"+EVENT_TRIGGER_ID+"';";
	DBCHECK(mosq, "ADR", adrET);
	json.Int(ADR);
	json.EndObject();

	string s = sendToGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
}

//--------------------------------------------INPUT REMOTE------------------------------------------------

void CreateRemote(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	int EVENT_TRIGGER_TYPE_ID = DATA["EVENT_TRIGGER_TYPE_ID"].GetInt();
	string EventTypeid = toString(checkControlEvent(mosq, EVENT_TRIGGER_TYPE_ID));
	string DEVICE_ID = DATA["DEVICE_ID"].GetString();
	int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
	string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
	const Value& DEVICE_ATTRIBUTE_VALUE = DATA["DEVICE_ATTRIBUTE_VALUE"];
	string BUTTON_VALUE = DEVICE_ATTRIBUTE_VALUE["BUTTON_VALUE"].GetString();
	int MODE_VALUE = DEVICE_ATTRIBUTE_VALUE["MODE_VALUE"].GetInt();
	int attID = checkDeviceAttribute(mosq, "BUTTON_VALUE");

	if(EventTypeid.compare("SCENE") == 0){
		StringBuffer sendToGW;
		Writer<StringBuffer> jsonAHG(sendToGW);
		jsonAHG.StartObject();
		jsonAHG.Key("CMD");
		jsonAHG.String("SETSCENEFORREMOTE");
		jsonAHG.Key("ADR");
		jsonAHG.Int(DEVICE_UNICAST);
		jsonAHG.Key("BUTTONID");
		jsonAHG.String(ToChar(BUTTON_VALUE));
		jsonAHG.Key("MODEID");
		jsonAHG.Int(MODE_VALUE);
		jsonAHG.Key("SCENEID");
		ADR = -1;
		string checkADR = "SELECT SceneUnicastID from EventTrigger where EventTriggerId='"+EVENT_TRIGGER_ID+"';";
		DBCHECK(mosq, "ADR", checkADR);
		jsonAHG.Int(ADR);
		jsonAHG.Key("SRGBID");
		jsonAHG.Int(0);
		jsonAHG.EndObject();

		string s = sendToGW.GetString();
		cout<<s<<endl;
		char * sendT = new char[s.length()+1];
		strcpy(sendT, s.c_str());
		MqttSend(mosq, sendT);
	}else if(EventTypeid.compare("RULE") == 0){
		string addsql = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue(EventTriggerId,DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue)"\
			" values ('"+EVENT_TRIGGER_ID+"','"+DEVICE_ID+"', "+toString(DEVICE_UNICAST)+", "+toString(attID)+",1,"+toString(MODE_VALUE)+");";
		cout<<addsql<<endl;
		DBSQL(mosq, addsql);
		MqttSendAPP(mosq, jobj);
	}
}

void DelRemote(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	string DEVICE_ID = DATA["DEVICE_ID"].GetString();
	string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
	const Value& DEVICE_ATTRIBUTE_VALUE = DATA["DEVICE_ATTRIBUTE_VALUE"];
	string BUTTON_VALUE = DEVICE_ATTRIBUTE_VALUE["BUTTON_VALUE"].GetString();
	int MODE_VALUE = DEVICE_ATTRIBUTE_VALUE["MODE_VALUE"].GetInt();
	ADR = -1;
	string sqlCheckDevice = "SELECT DeviceUnicastId FROM Device WHERE DeviceId='"+DEVICE_ID+"';";
	DBCHECK(mosq, "ADR", sqlCheckDevice);

	StringBuffer sendToGW;
	Writer<StringBuffer> json(sendToGW);
	json.StartObject();
	json.Key("CMD");
	json.String("DELSCENEFORREMOTE");
	json.Key("ADR");
	json.Int(ADR);
	json.Key("BUTTONID");
	json.String(ToChar(BUTTON_VALUE));
	json.Key("MODEID");
	json.Int(MODE_VALUE);
	json.EndObject();

	string s = sendToGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
}

//----------------------------------------------INPUT DOORSENSOR-------------------------------------------------
void CreateDoorSensor(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	string DEVICE_ID = DATA["DEVICE_ID"].GetString();

	int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
	string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
	//int attID = DATA["DEVICE_ATTRIBUTE_ID"].GetInt();
	const Value& DEVICE_ATTRIBUTE_VALUE = DATA["DEVICE_ATTRIBUTE_VALUE"];
	int DOOR = DEVICE_ATTRIBUTE_VALUE["DOOR"].GetInt();
//	cout << "unicast: " + DEVICE_UNICAST+ " dOOR: "+DOOR <<endl;
	printf("ID: %d, DOOR: %d\n",DEVICE_UNICAST, DOOR);
	ADR = -1;
	string checkADR = "SELECT SceneUnicastID from EventTrigger where EventTriggerId='"+EVENT_TRIGGER_ID+"';";
	DBCHECK(mosq, "ADR", checkADR);

	StringBuffer sendToGW;
	Writer<StringBuffer> jsonAHG(sendToGW);
	jsonAHG.StartObject();
	jsonAHG.Key("CMD");jsonAHG.String("ADDSCENE_DOOR_SENSOR");
	jsonAHG.Key("DATA");
	jsonAHG.StartObject();
		jsonAHG.Key("DEVICE_UNICAST_ID"); jsonAHG.Int(DEVICE_UNICAST);
		jsonAHG.Key("DOOR_SENSOR");
		jsonAHG.StartObject();
			jsonAHG.Key("DOOR"); jsonAHG.Int(DOOR);
		jsonAHG.EndObject();
		jsonAHG.Key("SCENEID"); jsonAHG.Int(ADR);
	jsonAHG.EndObject();
	jsonAHG.EndObject();

	string s = sendToGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
	/*
	Document document;
	document.Parse(jobj);
	const Value& DATA = document["DATA"];
	string DEVICE_ID = DATA["DEVICE_ID"].GetString();
	int DEVICE_UNICAST = DEVICE_UNICAST_ID(DEVICE_ID);
	string EVENT_TRIGGER_ID = DATA["EVENT_TRIGGER_ID"].GetString();
	int DEVICE_ATTRIBUTE_ID = DATA["DEVICE_ATTRIBUTE_ID"].GetInt();
	const Value& DEVICE_ATTRIBUTE_VALUE = DATA["DEVICE_ATTRIBUTE_VALUE"];
	int DOOR = DEVICE_ATTRIBUTE_VALUE["DOOR"].GetInt();
	int attID = checkDeviceAttribute(mosq, "DOOR");
	ADR = -1;
	string checkADR = "SELECT SceneUnicastID from EventTrigger where EventTriggerId='"+EVENT_TRIGGER_ID+"';";
	DBCHECK(mosq, "ADR", checkADR);

	StringBuffer sendToGW;
	Writer<StringBuffer> jsonAHG(sendToGW);
	jsonAHG.StartObject();
	jsonAHG.Key("CMD"); jsonAHG.String("ADDSCENE_DOOR_SENSOR");
	jsonAHG.Key("DATA");
	jsonAHG.StartObject();
		jsonAHG.Key("DEVICE_UNICAST_ID"); jsonAHG.Int(DEVICE_UNICAST);
		jsonAHG.Key("DOOR_SENSOR");
		jsonAHG.StartObject();
			jsonAHG.Key("DOOR"); jsonAHG.Int(DOOR);
		jsonAHG.EndObject();
		jsonAHG.Key("SCENEID"); jsonAHG.Int(ADR);
	jsonAHG.EndObject();
	jsonAHG.EndObject();

	string s = sendToGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
	*/
}

//--------------------------------------------#------------------------------------------------------------------

int createGroupId(struct mosquitto *mosq){
	int groupID;
	ADR = 0;
	string sql = "SELECT GroupUnicastId FROM GROUPID WHERE ValueCreate = '0' AND GroupingId = '' ORDER BY GroupUnicastId ASC LIMIT 1;";
	DBCHECK(mosq, "ADR", sql);
	if(ADR >= 49152 && ADR <= 49232){
		groupID = ADR;
	}else {
		ADR = 0;
		string sql1 = "SELECT GroupUnicastId FROM GROUPID ORDER BY GroupUnicastId DESC LIMIT 1;";
		DBCHECK(mosq, "ADR", sql1);
		if(ADR < 49152 || ADR > 49232){
			groupID = 49152;
		}else {
			groupID = ADR + 1;
		}
	}
	return groupID;
}

int createSceneId(struct mosquitto *mosq){
	int sceneID;
	ADR = 0;
	string sql = "SELECT SceneUnicastID FROM EventTriggerID WHERE ValueCreate = '0' AND EventTriggerId = '' ORDER BY SceneUnicastID ASC LIMIT 1;";
	DBCHECK(mosq, "ADR", sql);
	if(ADR > 0 && ADR < 65000){
		sceneID = ADR;
	}else {
		string sql = "SELECT SceneUnicastID FROM EventTriggerID ORDER BY SceneUnicastID DESC LIMIT 1;";
		DBCHECK(mosq, "ADR", sql);
		if(ADR == 0 || ADR > 65000){
			sceneID = 1;
		}else {
			sceneID = ADR + 1;
		}
	}
	return sceneID;
}

char* checkControl(struct mosquitto *mosq, int idControl){
	string sql = "SELECT Code FROM DeviceAttribute WHERE DeviceAttributeId = " + toString(idControl) +";";
	CONTROL = "";
	DBCHECK(mosq, "CONTROL", sql);
	string Check = CONTROL;
	char * control = new char[Check.length()+1];
	strcpy(control, Check.c_str());

	return control;
}

char* checkControlEvent(struct mosquitto *mosq, int idControl){
	string sql = "SELECT Code FROM EventTriggerType WHERE EventTriggerTypeId = " + toString(idControl) +";";
	CONTROL = "";
	DBCHECK(mosq, "CONTROL", sql);
	string Check = CONTROL;
	char * control = new char[Check.length()+1];
	strcpy(control, Check.c_str());

	return control;
}

int checkDeviceAttribute(struct mosquitto *mosq, string Code){
	int typeId;
	ADR = -1;
	string sql = "select DeviceAttributeID from DeviceAttribute where Code ='"+Code+"';";
	DBCHECK(mosq, "ADR", sql);
	typeId = ADR;
	return typeId;
}

int SCENE_UNICAST_ID(string SCENE_ID){
	string GroupId = "SELECT SceneUnicastID FROM EventTrigger WHERE EventTriggerId ='"+SCENE_ID+"';";
	ADR = 0;
	DBCHECK(mosq, "ADR", GroupId);
	int GROUP_UNICAST = ADR;
	return GROUP_UNICAST;
}

int GROUP_UNICAST_ID(string GROUP_ID){
	string GroupId = "SELECT GroupUnicastId FROM GROUPING WHERE GroupingId ='"+GROUP_ID+"';";
	ADR = 0;
	DBCHECK(mosq, "ADR", GroupId);
	int GROUP_UNICAST = ADR;
	return GROUP_UNICAST;
}

int DEVICE_UNICAST_ID(string DEVICE_ID){
	string checkID = "SELECT DeviceUnicastId FROM Device WHERE DeviceId = '"+DEVICE_ID+"';";
	ADR = 0;
	DBCHECK(mosq, "ADR", checkID);
	int DEVICE_UNICAST = ADR;
	if(DEVICE_ID.compare("") == 0){
		DEVICE_UNICAST = 65535;
	}
	return DEVICE_UNICAST;
}
































