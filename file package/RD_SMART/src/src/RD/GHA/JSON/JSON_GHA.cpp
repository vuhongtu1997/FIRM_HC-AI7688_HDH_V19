/*
 * JSON_GHA.cpp
 *
 *  Created on: Apr 10, 2021
 *      Author: trthang
 */

#include "JSON_GHA.hpp"
#include "../../MQTT/MQTT.hpp"

//-----------------convert
template<typename T>
string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

char* ToCharGHA(string s) {
	char *sendChar = new char[s.length() + 1];
	strcpy(sendChar, s.c_str());
	return sendChar;
}

//--------------------------------------------------------DB--------------------------------------------

int ADRSL;
string CONTROLSL;

static int CheckTimer(void *data, int argc, char **argv, char **azColName){
	try {
		int TimerNow = setTime();
		int DayNow = setDay();
		string DeviceId = toString(argv[0]);
		if(DayNow > atoi((const char*) argv[1])){
			if((24 * 60 + TimerNow - atoi((const char*) argv[2])) > 15){
				string SQL = "UPDATE Device SET StatusId = 0";

				StringBuffer sendToApp;
				Writer<StringBuffer> json(sendToApp);

				json.StartObject();
				json.Key("CMD");
				json.String("DEVICE_UPDATE");
				json.Key("DEVICE_ID");
				json.String(ToCharGHA(DeviceId));
				json.Key("STATUS");
				json.String("OFFLINE");
				json.EndObject();

				cout << sendToApp.GetString() << endl;
				string s = sendToApp.GetString();
				char * sendT = new char[s.length()+1];
				strcpy(sendT, s.c_str());
				MqttSendAPP(mosq, sendT);
			}else {
				cout<<"Đèn hoạt động ổn !!!"<<endl;
			}
		}else if (DayNow == atoi((const char*) argv[1])) {
			if(TimerNow - atoi((const char*) argv[2]) > 15){
				string SQL = "UPDATE Device SET StatusId = 0";

				StringBuffer sendToApp;
				Writer<StringBuffer> json(sendToApp);

				json.StartObject();
				json.Key("CMD");
				json.String("DEVICE_UPDATE");
				json.Key("DEVICE_ID");
				json.String(ToCharGHA(DeviceId));
				json.Key("STATUS");
				json.String("OFFLINE");
				json.EndObject();

				cout << sendToApp.GetString() << endl;
				string s = sendToApp.GetString();
				char * sendT = new char[s.length()+1];
				strcpy(sendT, s.c_str());
				MqttSendAPP(mosq, sendT);
			}else {
				cout<<"Đèn hoạt động ổn !!!"<<endl;
			}
		}
	} catch (exception &e) {
	};
	return 0;
}

static int SQLADR(void *data, int argc, char **argv, char **azColName){
	try {
		ADRSL = 0;
		ADRSL = atoi((const char*) argv[0]);
	} catch (exception &e) {
	}
	return 0;
}

static int SQLCONTROL(void *data, int argc, char **argv, char **azColName){
	try {
		CONTROLSL = "";
		CONTROLSL = toString(argv[0]);
	} catch (exception &e) {
	}
	return 0;
}

int DBCHECKGHA(struct mosquitto *mosq,string control, string sql){
	sqlite3* DB;
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
		code = CONTROLSL;
	}else if (control.compare("CHECK_TIMER") == 0){
		exit = sqlite3_exec(DB, sql.c_str(), CheckTimer, 0, &messaggeError);
	}


	if (exit != SQLITE_OK) {
		sqlite3_free(messaggeError);
	}
	else
		sqlite3_close(DB);

	return (0);
}

//----------------------------------------------------------------------------------------------------

void AdrGwToApp(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	if(document.HasMember("ADR")){
		if (document.HasMember("ONOFF")) {
			ONOFF(mosq, jobj);
		}else if (document.HasMember("CCT")){
			CCT(mosq, jobj);
		}else if (document.HasMember("DIM")){
			DIM(mosq, jobj);
		}else if (document.HasMember("HUE") && document.HasMember("SATURATION") && document.HasMember("LIGHTNESS")) {
			HSL(mosq, jobj);
		}else if (document.HasMember("ADDGROUP")){
			ADDGROUP(mosq, jobj);
		}else if (document.HasMember("DELGROUP")){
			DELGROUP(mosq, jobj);
		}else if (document.HasMember("CALLSCENE")){
			CALLSCENE(mosq, jobj);
		}else if (document.HasMember("ADDSCENE")){
			ADDSCENE(mosq, jobj);
		}else if (document.HasMember("DELSCENE")){
			DELSCENE(mosq, jobj);
		}else if (document.HasMember("LUX")){
			LUX(mosq, jobj);
		}else if (document.HasMember("PIR")){
			PIR(mosq, jobj);
		}else if (document.HasMember("POWER")){
			POWER(mosq, jobj);
		}else if (document.HasMember("CMD")){
			string cmd = document["CMD"].GetString();
			if(cmd.compare("RESETNODE") == 0){
				RESETNODE(mosq, jobj);
			}else if (cmd.compare("SETSCENEFORREMOTE") == 0){
				SETSCENEFORREMOTE(mosq, jobj);
			}else if (cmd.compare("DELSCENEFORREMOTE") == 0){
				DELSCENEFORREMOTE(mosq, jobj);
			}else if (cmd.compare("SETSCENEFORSENSOR") == 0){
				SETSCENEFORSENSOR(mosq, jobj);
			}else if (cmd.compare("DELSCENEFORSENSOR") == 0){
				DELSCENEFORSENSOR(mosq, jobj);
			}
		}
	}else if(document.HasMember("CMD") && !document.HasMember("ADR")) {
		string CMD = document["CMD"].GetString();
		if(CMD.compare("STOP")==0){
			MqttSendAPP(mosq, jobj);
		}
	}
}

void TYPEDEVICE(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	if(document.IsObject()){
		string cmd;
		if(document.HasMember("CMD")){
			cmd = document["CMD"].GetString();
			if(cmd.compare("TYPE_DEVICE") == 0){
				MqttSendAPP(mosq, jobj);
				const Value& DATA = document["DATA"];
				if(DATA.HasMember("DEVICE_UNICAST_ID") && DATA.HasMember("DEVICE_ID") && DATA.HasMember("DEVICE_KEY") && DATA.HasMember("NET_KEY") && DATA.HasMember("APP_KEY") && DATA.HasMember("DEVICE_TYPE_ID")){
					int DEVICE_UNICAST_ID = DATA["DEVICE_UNICAST_ID"].GetInt();
					string DEVICE_ID = DATA["DEVICE_ID"].GetString();
					string DEVICE_KEY = DATA["DEVICE_KEY"].GetString();
					string NET_KEY = DATA["NET_KEY"].GetString();
					string APP_KEY = DATA["APP_KEY"].GetString();
					int DeviceTypeId = DATA["DEVICE_TYPE_ID"].GetInt();
					string addDevice = "INSERT OR REPLACE INTO DEVICE (DeviceId, DeviceUnicastId, AppKey, NetKey, DeviceKey, DeviceTypeId, Owner) "
											"values ( '" + DEVICE_ID + "'," + toString(DEVICE_UNICAST_ID) + ",'" + APP_KEY + "','" + NET_KEY + "'"
													",'" + DEVICE_KEY + "'," + toString(DeviceTypeId) + ", 1);";
					cout<<addDevice<<endl;
					DBSQL(mosq, addDevice);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------

void ONOFF(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int value = document["ONOFF"].GetInt();
	if (adr > 0 && adr <= 49151) {
		int typeId = checkType(mosq, "ONOFF");
		CONTROLSL="";
		string sql = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr)+";";
		DBCHECKGHA(mosq, "CONTROL", sql);

		json.StartObject();
		json.Key("CMD");
		json.String("DEVICE");
		json.Key("DATA");
		json.StartObject();
		json.Key("DEVICE_ID");
		json.String(ToCharGHA(CONTROLSL));
		json.Key("DEVICE_UNICAST_ID");
		json.Int(adr);
		json.Key("PROPERTIES");
		json.StartArray();
		json.StartObject();
		json.Key("ID");
		json.Int(typeId);
		json.Key("VALUE");
		json.Int(value);
		json.EndObject();
		json.EndArray();
		json.EndObject();
		json.EndObject();

		cout << sendToApp.GetString() << endl;
		string s = sendToApp.GetString();
		char * sendT = new char[s.length()+1];
		strcpy(sendT, s.c_str());
		MqttSendAPP(mosq, sendT);

		int timer = setTime();
		int day = setDay();

		string addsql = "INSERT OR REPLACE INTO DeviceAttributeValue(DeviceId,DeviceUnicastId, DeviceAttributeId, Value, UpdateDay, UpdateTime)"\
			"values('"+toString(CONTROLSL)+"',"+toString(adr)+", "+toString(typeId)+" ,"+toString(value)+", "+toString(day)+", "+toString(timer)+");";
		DBSQL(mosq, addsql);
	}
}

void CCT(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int value = document["CCT"].GetInt();
	CONTROLSL="";
	int typeId = checkType(mosq, "CCT");
	string sql = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr - 1)+";";
	DBCHECKGHA(mosq, "CONTROL", sql);

	json.StartObject();
	json.Key("CMD");
	json.String("DEVICE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_ID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr - 1);
	json.Key("PROPERTIES");
	json.StartArray();
	json.StartObject();
	json.Key("ID");
	json.Int(typeId);
	json.Key("VALUE");
	json.Int(value);
	json.EndObject();
	json.EndArray();
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);

	int timer = setTime();
	int day = setDay();

	string addsql = "INSERT OR REPLACE INTO DeviceAttributeValue(DeviceId,DeviceUnicastId, DeviceAttributeId, Value, UpdateDay, UpdateTime)"\
		"values('"+toString(CONTROLSL)+"',"+toString(adr - 1)+", "+toString(typeId)+" ,"+toString(value)+", "+toString(day)+", "+toString(timer)+");";
	DBSQL(mosq, addsql);
}

void DIM(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int value = document["DIM"].GetInt();
	CONTROLSL="";
	int typeId = checkType(mosq, "DIM");
	string sql = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr)+";";
	DBCHECKGHA(mosq, "CONTROL", sql);

	json.StartObject();
	json.Key("CMD");
	json.String("DEVICE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_ID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("PROPERTIES");
	json.StartArray();
	json.StartObject();
	json.Key("ID");
	json.Int(typeId);
	json.Key("VALUE");
	json.Int(value);
	json.EndObject();
	json.EndArray();
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);

	int timer = setTime();
	int day = setDay();

	string addsql = "INSERT OR REPLACE INTO DeviceAttributeValue(DeviceId,DeviceUnicastId, DeviceAttributeId, Value, UpdateDay, UpdateTime)"\
		"values('"+toString(CONTROLSL)+"',"+toString(adr)+", "+toString(typeId)+" ,"+toString(value)+", "+toString(day)+", "+toString(timer)+");";
	DBSQL(mosq, addsql);
}

void HSL(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int H = document["HUE"].GetInt();
	int S = document["SATURATION"].GetInt();
	int L = document["LIGHTNESS"].GetInt();
	string sql = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr)+";";
	DBCHECKGHA(mosq, "CONTROL", sql);
	int typeId1 = checkType(mosq, "HUE");
	int typeId2 = checkType(mosq, "SATURATION");
	int typeId3 = checkType(mosq, "LIGHTNESS");
	json.StartObject();
	json.Key("CMD");
	json.String("DEVICE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_ID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("PROPERTIES");
	json.StartArray();
	//HUE
	json.StartObject();
	json.Key("ID");
	json.Int(typeId1);
	json.Key("VALUE");
	json.Int(H);
	json.EndObject();
	//SATURATION
	json.StartObject();
	json.Key("ID");
	json.Int(typeId2);
	json.Key("VALUE");
	json.Int(S);
	json.EndObject();
	//LIGHTNESS
	json.StartObject();
	json.Key("ID");
	json.Int(typeId3);
	json.Key("VALUE");
	json.Int(L);
	json.EndObject();

	json.EndArray();
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);

	int timer = setTime();
	int day = setDay();

	string sql_R = "INSERT OR REPLACE INTO DeviceAttributeValue(DeviceId, DeviceUnicastId, DeviceAttributeId, Value, UpdateDay, UpdateTime)"\
	"values('"+toString(CONTROLSL)+"',"+toString(adr)+","+toString(typeId1)+","+toString(H)+", "+toString(day)+", "+toString(timer)+");";
	DBSQL(mosq, sql_R);

	string sql_G = "INSERT OR REPLACE INTO DeviceAttributeValue(DeviceId, DeviceUnicastId, DeviceAttributeId, Value, UpdateDay, UpdateTime)"\
	"values('"+toString(CONTROLSL)+"',"+toString(adr)+","+toString(typeId2)+","+toString(S)+", "+toString(day)+", "+toString(timer)+");";
	DBSQL(mosq, sql_G);

	string sql_B = "INSERT OR REPLACE INTO DeviceAttributeValue(DeviceId, DeviceUnicastId, DeviceAttributeId, Value, UpdateDay, UpdateTime)"\
	"values('"+toString(CONTROLSL)+"',"+toString(adr)+","+toString(typeId3)+","+toString(L)+", "+toString(day)+", "+toString(timer)+");";
	DBSQL(mosq, sql_B);
}

void ADDGROUP(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int GROUP = document["ADDGROUP"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("GROUP");
	json.Key("TYPE");
	json.String("DEVICE_RESPONSE");
	json.Key("DATA");
	json.StartObject();
	json.Key("GROUP_ID");
	CONTROLSL = "";
	string sql11 = "SELECT GroupingId FROM GROUPING WHERE GroupUnicastId = "+toString(GROUP)+";";
	DBCHECKGHA(mosq, "CONTROL", sql11);
	cout<<CONTROLSL<<endl;
	json.String(ToCharGHA(CONTROLSL));

	json.Key("GROUP_UNICAST_ID");
	json.Int(GROUP);

	CONTROLSL = "";
	string sql22 = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr)+";";
	DBCHECKGHA(mosq, "CONTROL", sql22);

	json.Key("DEVICE_ID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("STATUS");
	json.String("SUCCESS");
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void DELGROUP(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int GROUP = document["DELGROUP"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("GROUP");
	json.Key("TYPE");
	json.String("REMOVE_DEVICE");
	json.Key("DATA");
	json.StartObject();
	json.Key("GROUP_ID");
	string sql11 = "SELECT GroupingId FROM GROUPING WHERE GroupUnicastId = "+toString(GROUP)+";";
	DBCHECKGHA(mosq, "CONTROL", sql11);
	json.String(ToCharGHA(CONTROLSL));

	json.Key("GROUP_UNICAST_ID");
	json.Int(GROUP);

	string sql22 = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr)+";";
	DBCHECKGHA(mosq, "CONTROL", sql22);

	json.Key("DEVICE_ID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("STATUS");
	json.String("SUCCESS");
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void CALLSCENE(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int value = document["CALLSCENE"].GetInt();
	string sql = "SELECT EventTriggerId FROM EventTrigger WHERE SceneUnicastID = "+ toString(value) + ";";
	DBCHECKGHA(mosq, "CONTROL", sql);
	json.StartObject();
	json.Key("CMD");
	json.String("EVENT_TRIGGER");
	json.Key("DATA");
	json.StartObject();
	json.Key("EVENT_TRIGGER_ID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_ID");
	string DEVICE_ID = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr)+";";
	DBCHECKGHA(mosq, "CONTROL", DEVICE_ID);
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void ADDSCENE(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int value = document["ADDSCENE"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("EVENT_TRIGGER_OUTPUT_DEVICE_MAPPING");
	json.Key("TYPE");
	json.String("ADD");
	json.Key("DATA");
	json.StartObject();
	json.Key("EVENT_TRIGGER_ID");
	string sql = "SELECT EventTriggerId FROM EventTrigger WHERE SceneUnicastID  = "+ toString(value) + ";";
	DBCHECKGHA(mosq, "CONTROL", sql);
	json.String(ToCharGHA(CONTROLSL));
	string sql1 = "SELECT GroupId FROM EventTrigger WHERE SceneUnicastID  = "+ toString(value) + ";";
	DBCHECKGHA(mosq, "CONTROL", sql1);
	json.Key("GUID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("SCENE_UNICAST_ID");
	json.Int(value);
	json.Key("STATUS");
	json.String("SUCCESS");
	json.Key("DEVICE");
	json.StartArray();
	json.StartObject();
	json.Key("DEVICE_ID");
	string sql2 = "SELECT DeviceId FROM Device WHERE DeviceUnicastId  = "+ toString(adr) + ";";
	DBCHECKGHA(mosq, "CONTROL", sql2);
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.EndObject();
	json.EndArray();
	json.EndObject();
	json.EndObject();


	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void DELSCENE(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	int value = document["DELSCENE"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("EVENT_TRIGGER_OUTPUT_DEVICE_MAPPING");
	json.Key("TYPE");
	json.String("DELETE");
	json.Key("DATA");
	json.StartObject();
	json.Key("EVENT_TRIGGER_ID");
	string sql = "SELECT EventTriggerId FROM EventTrigger WHERE SceneUnicastID  = "+ toString(value) + ";";
	DBCHECKGHA(mosq, "CONTROL", sql);
	json.String(ToCharGHA(CONTROLSL));
	string sql1 = "SELECT GroupId FROM EventTrigger WHERE SceneUnicastID  = "+ toString(value) + ";";
	DBCHECKGHA(mosq, "CONTROL", sql1);
	json.Key("GUID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("SCENE_UNICAST_ID");
	json.Int(value);
	json.Key("STATUS");
	json.String("SUCCESS");
	json.Key("DEVICE");
	json.StartArray();
	json.StartObject();
	json.Key("DEVICE_ID");
	string sql2 = "SELECT DeviceId FROM Device WHERE DeviceUnicastId  = "+ toString(adr) + ";";
	DBCHECKGHA(mosq, "CONTROL", sql2);
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.EndObject();
	json.EndArray();
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void LUX(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("DEVICE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("LUX");
	json.Int(document["LUX"].GetInt());
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void PIR(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("DEVICE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("PIR");
	json.Int(document["PIR"].GetInt());
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void POWER(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("DEVICE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("POWER");
	json.Int(document["POWER"].GetInt());
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void RESETNODE(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	CONTROLSL ="";
	string sql = "SELECT DeviceId FROM Device WHERE DeviceUnicastId = "+toString(adr)+";";
	DBCHECKGHA(mosq, "CONTROL", sql);
	json.StartObject();
	json.Key("CMD");
	json.String("RESET_NODE");
	json.Key("DATA");
	json.StartArray();
	json.StartObject();
	json.Key("DEVICE_ID");
	json.String(ToCharGHA(CONTROLSL));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.EndObject();
	json.EndArray();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);

	string sqldel = "DELETE FROM DEVICE WHERE DeviceUnicastId = " + toString(adr) + ";";
	DBSQL(mosq, sqldel);
}

void SETSCENEFORREMOTE(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("SET_SCENE_FOR_REMOTE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_ID");
	string DEVICE_ID = getGUID(mosq, adr);
	json.String(ToCharGHA(DEVICE_ID));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("COMPAIRISON_OPERATOR_ID");
	json.Int(1);

	string checkScene = "SELECT EventTriggerId FROM EventTrigger WHERE SceneUnicastID ="+toString(document["SCENEID"].GetInt())+";";
	CONTROLSL="";
	DBCHECKGHA(mosq, "CONTROL", checkScene);
	json.Key("EVENT_TRIGGER_ID");
	json.String(ToCharGHA(CONTROLSL));

	json.Key("DEVICE_ATTRIBUTE_ID");
	ADRSL =-1;
	string button = document["BUTTONID"].GetString();
	string attNew1 = "SELECT DeviceAttributeID FROM DeviceAttribute WHERE Code ='"+button+"';";
	DBCHECKGHA(mosq, "ADR", attNew1);
	int attID = ADRSL;
	json.Int(attID);


	json.Key("DEVICE_ATTRIBUTE_VALUE");
	json.StartObject();
	json.Key("BUTTON_VALUE");
	json.String(ToCharGHA(document["BUTTONID"].GetString()));
	json.Key("MODE_VALUE");
	json.Int(document["MODEID"].GetInt());
	json.EndObject();
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);

	string addsql = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue(EventTriggerId,DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue)"\
		" values ('"+CONTROLSL+"','"+DEVICE_ID+"', "+toString(adr)+", "+toString(attID)+",1,"+toString(document["MODEID"].GetInt())+");";
	cout<<addsql<<endl;
	DBSQL(mosq, addsql);
}

void DELSCENEFORREMOTE(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("SET_SCENE_FOR_REMOTE");
	json.Key("TYPE");
	json.String("DELETE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_ID");
	string DEVICE_ID = getGUID(mosq, adr);
	json.String(ToCharGHA(DEVICE_ID));
	json.Key("DEVICE_ATTRIBUTE_VALUE");
	json.StartObject();
	json.Key("BUTTON_VALUE");
	json.String(ToCharGHA(document["BUTTONID"].GetString()));
	json.Key("MODE_VALUE");
	json.Int(document["MODEID"].GetInt());
	json.EndObject();
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void SETSCENEFORSENSOR(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("SET_SCENE_FOR_SENSOR");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_ID");
	string DEVICE_ID = getGUID(mosq, adr);
	json.String(ToCharGHA(DEVICE_ID));
	json.Key("DEVICE_UNICAST_ID");
	json.Int(adr);
	json.Key("EVENT_TRIGGER_ID");
	string checkScene = "SELECT EventTriggerId FROM EventTrigger WHERE SceneUnicastID ="+toString(document["SCENEID"].GetInt())+";";
	CONTROLSL="";
	DBCHECKGHA(mosq, "CONTROL", checkScene);
	json.String(ToCharGHA(CONTROLSL));
	if(document.HasMember("LIGHT_SENSOR")){
		int a = checkType(mosq, "LUX");
		json.Key("LIGHT_SENSOR");
		json.StartObject();
		const Value& LIGHT_SENSOR = document["LIGHT_SENSOR"];
		int CONDITION = LIGHT_SENSOR["CONDITION"].GetInt();
		if(LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
			int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();
			int HIGHT_LUX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();
			json.Key("LOW_LUX");
			json.Int(LOW_LUX);
			json.Key("HIGHT_LUX");
			json.Int(HIGHT_LUX);
			json.Key("COMPAIRISON_OPERATOR_ID");
			json.Int(CONDITION);
			json.Key("DEVICE_ATTRIBUTE_ID");
			json.Int(a);

			string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
					"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue, DeviceAttributeValueMAX)"
					"values ('"+CONTROLSL+"', '"+DEVICE_ID+"', "+toString(adr)+", "+toString(a)+","+toString(CONDITION)+","+toString(LOW_LUX)+","+toString(HIGHT_LUX)+")";
			DBSQL(mosq, add2);
		}
		if(LIGHT_SENSOR.HasMember("LOW_LUX") && !LIGHT_SENSOR.HasMember("HIGHT_LUX")){
			int LOW_LUX = LIGHT_SENSOR["LOW_LUX"].GetInt();
			json.Key("LOW_LUX");
			json.Int(LOW_LUX);
			json.Key("COMPAIRISON_OPERATOR_ID");
			json.Int(CONDITION);
			json.Key("DEVICE_ATTRIBUTE_ID");
			json.Int(a);

			string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
					"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue)"
					"values ('"+CONTROLSL+"', '"+DEVICE_ID+"', "+toString(adr)+", "+toString(a)+","+toString(CONDITION)+","+toString(LOW_LUX)+")";
			DBSQL(mosq, add2);
		}
		if(!LIGHT_SENSOR.HasMember("LOW_LUX") && LIGHT_SENSOR.HasMember("HIGHT_LUX")){
			int HIGHT_LUX = LIGHT_SENSOR["HIGHT_LUX"].GetInt();
			json.Key("HIGHT_LUX");
			json.Int(HIGHT_LUX);
			json.Key("COMPAIRISON_OPERATOR_ID");
			json.Int(CONDITION);
			json.Key("DEVICE_ATTRIBUTE_ID");
			json.Int(a);

			string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
					"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValueMAX)"
					"values ('"+CONTROLSL+"', '"+DEVICE_ID+"', "+toString(adr)+", "+toString(a)+","+toString(CONDITION)+","+toString(HIGHT_LUX)+")";
			DBSQL(mosq, add2);
		}
		json.EndObject();
	}

	if(document.HasMember("PIR_SENSOR")){
		int a = checkType(mosq, "PIR");
		const Value& PIR_SENSOR = document["PIR_SENSOR"];
		int PIR = PIR_SENSOR["PIR"].GetInt();

		json.Key("PIR_SENSOR");
		json.StartObject();
		json.Key("PIR");
		json.Int(PIR);
		json.Key("COMPAIRISON_OPERATOR_ID");
		json.Int(1);
		json.Key("DEVICE_ATTRIBUTE_ID");
		json.Int(a);
		json.EndObject();

		string add2 = "INSERT OR REPLACE INTO EventTriggerInputDeviceSetupValue "
				"(EventTriggerId, DeviceId, DeviceUnicastId, DeviceAttributeId, CromparisonOperatorId, DeviceAttributeValue)"
				"values ('"+CONTROLSL+"', '"+DEVICE_ID+"', "+toString(adr)+", "+toString(a)+",1,"+toString(PIR)+")";
		DBSQL(mosq, add2);
	}
	json.EndObject();
	json.EndObject();

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

void DELSCENEFORSENSOR(struct mosquitto *mosq, char* jobj){
	Document document;
	document.Parse(jobj);
	StringBuffer sendToApp;
	Writer<StringBuffer> json(sendToApp);
	int adr = document["ADR"].GetInt();
	json.StartObject();
	json.Key("CMD");
	json.String("SET_SCENE_FOR_SENSOR");
	json.Key("TYPE");
	json.String("DELETE");
	json.Key("DATA");
	json.StartObject();
	json.Key("DEVICE_ID");
	string checkDevice = "SELECT DeviceUnicastId FROM Device WHERE DeviceId ="+toString(adr)+";";
	CONTROLSL="";
	DBCHECKGHA(mosq, "CONTROL", checkDevice);
	string Device = CONTROLSL;
	json.String(ToCharGHA(Device));
	json.Key("EVENT_TRIGGER_ID");
	string checkScene = "SELECT EventTriggerId FROM EventTrigger WHERE SceneUnicastID ="+toString(document["SCENEID"].GetInt())+";";
	CONTROLSL="";
	DBCHECKGHA(mosq, "CONTROL", checkScene);
	string Scene = CONTROLSL;
	json.String(ToCharGHA(Scene));
	json.EndObject();
	json.EndObject();

	string del = "DELETE FROM EventTriggerInputDeviceSetupValue EventTriggerId='"+Scene+"' and DeviceId ='"+Device+"' and DeviceUnicastId ="+toString(adr)+";";
	DBSQL(mosq, del);

	cout << sendToApp.GetString() << endl;
	string s = sendToApp.GetString();
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSendAPP(mosq, sendT);
}

//----------------------------------------------------------------------------------------

char* checkControlCode(struct mosquitto *mosq, int idControl){
	string sql = "SELECT Code FROM DeviceAttribute WHERE DeviceAttributeId = " + toString(idControl) +";";
	DBCHECKGHA(mosq, "CONTROL", sql);
	string Check = CONTROLSL;
	char * control = new char[Check.length()+1];
	strcpy(control, Check.c_str());
	return control;
}

int checkType(struct mosquitto *mosq, string Code){
	int typeId;
	ADRSL = -1;
	string sql = "select DeviceAttributeID from DeviceAttribute where Code ='"+Code+"';";
	DBCHECKGHA(mosq, "ADR", sql);
	typeId = ADRSL;
	return typeId;
}

string getGUID(struct mosquitto *mosq, int adr){
	CONTROLSL = "";
	string guid = "SELECT DeviceId from Device WHERE DeviceUnicastId = "+toString(adr)+";";
	DBCHECKGHA(mosq, "CONTROL", guid);
	return CONTROLSL;
}

int getUnicast(struct mosquitto *mosq, string control){
	ADRSL = -1;
	string guid = "SELECT DeviceUnicastId from Device WHERE  DeviceId= '"+control+"';";
	DBCHECKGHA(mosq, "ADR", guid);
	return ADRSL;
}

int setTime(){
	time_t now = time(0);
	tm *ltm = localtime(&now);
	//gán biến
	//Giờ
	int h = ltm->tm_hour;
	int m = ltm->tm_min;

	//setup Time
	if(m == 60){
		h ++;
		m = m - 60;
	}

	int timeNow = h*60 + m;
	cout<<"Giờ : " <<toString(h) + ":" + toString(m)<<" "<<"GHA"<<endl;

	return timeNow;
}

int setDay(){
	time_t now = time(0);
	tm *ltm = localtime(&now);
	//gán biến

	//ngày
	int day = ltm->tm_mday;
	int month = 1 + ltm->tm_mon;
	int year = 1900 + ltm->tm_year;
	string d,m;

	if(day < 10){
		d = "0" + toString(day);
	}else {
		d = toString(day);
	}
	if(month < 10){
		m = "0" + toString(month);
	}else {
		m = toString(month);
	}

	string timeNow = toString(year) + m + d;
	cout<<"Ngày: "<<timeNow<<" "<<"GHA"<<endl;
	int x;
	stringstream geek(timeNow);
	geek >> x;
	return x;
}
