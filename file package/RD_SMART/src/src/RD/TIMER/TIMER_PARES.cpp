/*
 * TIMER_PARES.cpp
 *
 *  Created on: Apr 14, 2021
 *      Author: trthang
 */




#include "TIMER_PARES.hpp"
#include "../../RD/MQTT/MQTT.hpp"
//#include <stack>


template<typename T>
string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

char* ToCharTimer(string s) {
	char *sendChar = new char[s.length() + 1];
	strcpy(sendChar, s.c_str());
	return sendChar;
}

int TimerOver;
string weekDayss;

//---------------------------

int *arrStart;
//int *arrEnd;
//int *arr;
int lengtStart;

//char * cEndAt;
char * cStartAt;


StringBuffer sendGW;
Writer<StringBuffer> jsonS(sendGW);

char* CheckType;
int ADRTIMER;

int thayDoi;


//stack <int> Device;
//stack <int> Group;


/*-----------------------------------------------DATABASE---------------------------------------------------------*/
static int RULEDEVICE(void *data, int argc, char **argv, char **azColName){

	int times = atoi((const char*) argv[3]);
	StringBuffer sendGW;
	Writer<StringBuffer> jsonS(sendGW);
	char* control = checkControl(mosq, atoi((const char*) argv[1]));
	jsonS.StartObject();
	int adr = atoi((const char*) argv[0]);
	if(toString(control).compare("CCT") == 0){
		adr ++;
	}
	jsonS.Key("ADR");
	jsonS.Int(adr);
	jsonS.Key(control);
	jsonS.Int(atoi((const char*) argv[2]));
	if(times > 0 && toString(control).compare("CCT") != 0){
		jsonS.Key("TIME");
		jsonS.Int(times);
	}
	jsonS.EndObject();

	string s = sendGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
	return 0;
}

static int RULEGROUP(void *data, int argc, char **argv, char **azColName){

	int times = atoi((const char*) argv[3]);
	StringBuffer sendGW;
	Writer<StringBuffer> jsonS(sendGW);
	char* control = checkControl(mosq, atoi((const char*) argv[1]));
	jsonS.StartObject();
	int adr = atoi((const char*) argv[0]);
	jsonS.Key("ADR");
	jsonS.Int(adr);
	jsonS.Key(control);
	jsonS.Int(atoi((const char*) argv[2]));
	if(times > 0 && toString(control).compare("CCT") != 0){
		jsonS.Key("TIME");
		jsonS.Int(times);
	}
	jsonS.EndObject();

	string s = sendGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
	return 0;
}

static int RULESCENE(void *data, int argc, char **argv, char **azColName){

	StringBuffer sendGW;
	Writer<StringBuffer> jsonS(sendGW);
	jsonS.StartObject();
	int adr = atoi((const char*) argv[0]);
	int times = atoi((const char*) argv[1]);
	cout<<adr<<"vvvv"<<times<<endl;
	jsonS.Key("CALLSCENE");
	jsonS.Int(adr);
	if(times > 0){
		jsonS.Key("TIME");
		jsonS.Int(times);
	}
	else
	{
		jsonS.Key("TIME");
		jsonS.Int(0);
	}
	jsonS.EndObject();

	string s = sendGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
	return 0;
}

static int CHECK_SCENE(void *data, int argc, char **argv, char **azColName){

	int a = atoi((const char*) argv[0]);
	StringBuffer sendGW;
	Writer<StringBuffer> jsonS(sendGW);
	jsonS.StartObject();
	jsonS.Key("CALLSCENE");
	jsonS.Int(a);
	jsonS.Key("TIME");
	jsonS.Int(0);
	jsonS.EndObject();

	string s = sendGW.GetString();
	cout<<s<<endl;
	char * sendT = new char[s.length()+1];
	strcpy(sendT, s.c_str());
	MqttSend(mosq, sendT);
	return 0;
}

static int CHECK_RULE(void *data, int argc, char **argv, char **azColName){
	string a = toString((const char*) argv[0]);

	string DEVICE = "SELECT EventTriggerOutputDeviceSetupValue.DeviceUnicastId, EventTriggerOutputDeviceSetupValue.DeviceAttributeId, EventTriggerOutputDeviceSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
			"FROM EventTriggerOutputDeviceSetupValue INNER JOIN EventTrigger "
			"ON EventTrigger.EventTriggerID = EventTriggerOutputDeviceSetupValue.EventTriggerId "
			" WHERE EventTriggerOutputDeviceSetupValue.EventTriggerId = '"+a+"' ORDER BY EventTriggerOutputDeviceSetupValue.DeviceAttributeId ;";
	DBContext(mosq, "RULEDEVICE", DEVICE);

	string GROUPING = "SELECT EventTriggerOutputGroupingSetupValue.GroupUnicastId, EventTriggerOutputGroupingSetupValue.DeviceAttributeId, EventTriggerOutputGroupingSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
			"FROM EventTriggerOutputGroupingSetupValue INNER JOIN EventTrigger "
			"ON EventTrigger.EventTriggerID = EventTriggerOutputGroupingSetupValue.EventTriggerId "
			" WHERE EventTriggerOutputGroupingSetupValue.EventTriggerId = '"+a+"' ORDER BY EventTriggerOutputGroupingSetupValue.DeviceAttributeId DESC;";
	DBContext(mosq, "RULEGROUP", GROUPING);

	string SCENE = "SELECT EventTriggerOutputSceneMapping.SceneUnicastID, EventTrigger.FADE_IN FROM EventTriggerOutputSceneMapping INNER JOIN EventTrigger "
			"ON EventTrigger.EventTriggerID = EventTriggerOutputSceneMapping.EventTriggerId "
			"WHERE EventTriggerOutputSceneMapping.EventTriggerId = '"+a+"';";
	DBContext(mosq, "RULESCENE", SCENE);
	return 0;
}

static int LUXPIR(void *data, int argc, char **argv, char **azColName){
	string a = toString((const char*) argv[0]);
	int check = atoi((const char*) argv[1]);
	if(check == 0){
		string DEVICE = "SELECT EventTriggerOutputDeviceSetupValue.DeviceUnicastId, EventTriggerOutputDeviceSetupValue.DeviceAttributeId, EventTriggerOutputDeviceSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
					"FROM EventTriggerOutputDeviceSetupValue INNER JOIN EventTrigger "
					"ON EventTrigger.EventTriggerID = EventTriggerOutputDeviceSetupValue.EventTriggerId "
					" WHERE EventTriggerOutputDeviceSetupValue.EventTriggerId = '"+a+"'  ORDER BY EventTriggerOutputDeviceSetupValue.DeviceAttributeId DESC;";
			DBContext(mosq, "RULEDEVICE", DEVICE);

		string GROUPING = "SELECT EventTriggerOutputGroupingSetupValue.GroupUnicastId, EventTriggerOutputGroupingSetupValue.DeviceAttributeId, EventTriggerOutputGroupingSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
					"FROM EventTriggerOutputGroupingSetupValue INNER JOIN EventTrigger "
					"ON EventTrigger.EventTriggerID = EventTriggerOutputGroupingSetupValue.EventTriggerId "
					" WHERE EventTriggerOutputGroupingSetupValue.EventTriggerId = '"+a+"' ORDER BY EventTriggerOutputGroupingSetupValue.DeviceAttributeId DESC;";
			DBContext(mosq, "RULEGROUP", GROUPING);

		string SCENE = "SELECT EventTriggerOutputSceneMapping.SceneUnicastID, EventTrigger.FADE_IN FROM EventTriggerOutputSceneMapping INNER JOIN EventTrigger "
					"ON EventTrigger.EventTriggerID = EventTriggerOutputSceneMapping.EventTriggerId "
					"WHERE EventTriggerOutputSceneMapping.EventTriggerId = '"+a+"';";
			DBContext(mosq, "RULESCENE", SCENE);
	}else if (check == 1) {
		int timer1 = setTimer();
		string weekDay = setDayTimer();

		string sqlStart = "SELECT StartAt FROM EventTrigger WHERE LogicalOperatorID = 1 AND EventTriggerId = '"+a+"';";
		DBContext(mosq, "CHECKTYPE", sqlStart);
		int S = tachTimer(CheckType);

		string sqlEnd = "SELECT EndAt FROM EventTrigger WHERE LogicalOperatorID = 1 AND EventTriggerId = '"+a+"';";
		DBContext(mosq, "CHECKTYPE", sqlEnd);
		int E = tachTimer(CheckType);
		cout<<timer1 <<"\t"<<S<<"\t"<<E<<endl;
		if(timer1 >= S && timer1 <= E){
			string DEVICE = "SELECT EventTriggerOutputDeviceSetupValue.DeviceUnicastId, EventTriggerOutputDeviceSetupValue.DeviceAttributeId, EventTriggerOutputDeviceSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
						"FROM EventTriggerOutputDeviceSetupValue INNER JOIN EventTrigger "
						"ON EventTrigger.EventTriggerID = EventTriggerOutputDeviceSetupValue.EventTriggerId "
						" WHERE EventTriggerOutputDeviceSetupValue.EventTriggerId = '"+a+"'  ORDER BY EventTriggerOutputDeviceSetupValue.DeviceAttributeId DESC;";
				DBContext(mosq, "RULEDEVICE", DEVICE);

			string GROUPING = "SELECT EventTriggerOutputGroupingSetupValue.GroupUnicastId, EventTriggerOutputGroupingSetupValue.DeviceAttributeId, EventTriggerOutputGroupingSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
						"FROM EventTriggerOutputGroupingSetupValue INNER JOIN EventTrigger "
						"ON EventTrigger.EventTriggerID = EventTriggerOutputGroupingSetupValue.EventTriggerId "
						" WHERE EventTriggerOutputGroupingSetupValue.EventTriggerId = '"+a+"' ORDER BY EventTriggerOutputGroupingSetupValue.DeviceAttributeId DESC;";
				DBContext(mosq, "RULEGROUP", GROUPING);

			string SCENE = "SELECT EventTriggerOutputSceneMapping.SceneUnicastID, EventTrigger.FADE_IN FROM EventTriggerOutputSceneMapping INNER JOIN EventTrigger "
						"ON EventTrigger.EventTriggerID = EventTriggerOutputSceneMapping.EventTriggerId "
						"WHERE EventTriggerOutputSceneMapping.EventTriggerId = '"+a+"';";
				DBContext(mosq, "RULESCENE", SCENE);
		}
	}
	return 0;
}

static int CallRULE(void *data, int argc, char **argv, char **azColName){
	for(int i = 0; i<argc; i++){
		string a;
		a = toString((const char*) argv[i]);

		if(a.compare("SCENE") == 0){
			ADRTIMER=-1;
			string sqlID = "SELECT SceneUnicastID FROM EventTrigger WHERE StartAt = '"+ parthTime(TimerOver) + "' AND EventTriggerTypeId = 1;";
			DBContext(mosq, "CHECK_SCENE", sqlID);

			cout<<sqlID<<endl;
		}else if(a.compare("RULE") == 0){
			string sql = "SELECT EventTriggerId FROM EventTrigger "\
					"WHERE EventTrigger.StartAt = '" + parthTime(TimerOver) + "' AND EventTriggerTypeId = 2 ;";
			cout<<sql<<endl;
			DBContext(mosq, "CHECK_RULE", sql);
			cout<<"checkrulexong"<<endl;
		}
	}
	return 0;
}

static int CallbackSQLStart(void *data, int argc, char **argv, char **azColName){
	string a;
	for(int i = 0; i<argc; i++){
		a = toString(argv[i]);
		if(!a.compare("") == 0){
			jsonS.Int(tachTimer(ToCharTimer(a)));
		}
	}
	return 0;
}

//static int CallbackSQLEnd(void *data, int argc, char **argv, char **azColName){
//	string a;
//	for(int i = 0; i<argc; i++){
//		a = toString(argv[i]);
//		if(!a.compare("") == 0){
//			jsonS.Int(tachTimer(ToCharTimer(a)));
//		}
//	}
//	return 0;
//}

static int CallbackSQL(void *data, int argc, char **argv, char **azColName){
	for(int i = 0; i<argc; i++){
		string a = toString((const char*) argv[i]);
		CheckType = ToCharTimer(a);
	}
	return 0;
}

static int CallbackSQLID(void *data, int argc, char **argv, char **azColName){
	for(int i = 0; i<argc; i++){
		ADRTIMER = atoi((const char*) argv[i]);
	}
	return 0;
}

int DBContext(struct mosquitto *mosq, string control, string sql) {
	sqlite3* DB;
	int exit = 0;
	do {
			exit = sqlite3_open("/root/rd.Sqlite", &DB);
		} while (exit != SQLITE_OK);
	char* messaggeError;


	if(control == "STARTAT"){
		cStartAt = "";
		jsonS.Reset(sendGW);
		sendGW.Clear();
		//add timer join Json
		jsonS.StartObject();
		jsonS.Key("STARTAT");
		jsonS.StartArray();

		exit = sqlite3_exec(DB, sql.c_str(), CallbackSQLStart, 0, &messaggeError);

		jsonS.EndArray();
		jsonS.EndObject();

		string Start = sendGW.GetString();
		cStartAt = new char[Start.length()+1];
		cout<<Start<<endl;
		strcpy(cStartAt, Start.c_str());
//	}else if(control == "ENDAT") {
//		cEndAt = "";
//		jsonS.Reset(sendGW);
//		sendGW.Clear();
//		//add timer join Json
//		jsonS.StartObject();
//		jsonS.Key("ENDAT");
//		jsonS.StartArray();
//
//		exit = sqlite3_exec(DB, sql.c_str(), CallbackSQLEnd, 0, &messaggeError);
//
//		jsonS.EndArray();
//		jsonS.EndObject();
//
//		string End = sendGW.GetString();
//		cEndAt = new char[End.length()+1];
//		strcpy(cEndAt, End.c_str());
	}else if(control == "CHECKTYPE") {
		exit = sqlite3_exec(DB, sql.c_str(), CallbackSQL, 0, &messaggeError);
	}else if(control == "ADR") {
		exit = sqlite3_exec(DB, sql.c_str(), CallbackSQLID, 0, &messaggeError);
	}else if(control == "CALLRULE") {
		exit = sqlite3_exec(DB, sql.c_str(), CallRULE, 0, &messaggeError);
	}else if(control == "CHECK_SCENE") {
		exit = sqlite3_exec(DB, sql.c_str(), CHECK_SCENE, 0, &messaggeError);
	}else if(control == "CHECK_RULE") {
		exit = sqlite3_exec(DB, sql.c_str(), CHECK_RULE, 0, &messaggeError);
	}else if(control == "RULEDEVICE") {
		exit = sqlite3_exec(DB, sql.c_str(), RULEDEVICE, 0, &messaggeError);
	}else if(control == "RULEGROUP") {
		exit = sqlite3_exec(DB, sql.c_str(), RULEGROUP, 0, &messaggeError);
	}else if(control == "RULESCENE") {
		exit = sqlite3_exec(DB, sql.c_str(), RULESCENE, 0, &messaggeError);
	}else if (control == "LUXPIR") {
		exit = sqlite3_exec(DB, sql.c_str(), LUXPIR, 0, &messaggeError);
	}


	if (exit != SQLITE_OK) {
		sqlite3_free(messaggeError);
	}
	else
		sqlite3_close(DB);

	return (0);
}


//---------------------------------------------------------------------------------------CALL EVENT


int Timer_AHG(struct mosquitto *mosq, char * jobj){ // @suppress("No return")
	Document document;
	document.Parse(jobj);

	string cmd = document["CMD"].GetString();
	if(cmd.compare("EVENT_TRIGGER") == 0){
		if(document.HasMember("TYPE")){
			pthread_t tmp_thread;
			tmp_thread = pthread_self();
			thayDoi = 1;
//			delete arr;
			cout<<"RESET"<<endl;
			sleep(1);
			pthread_create(&tmp_thread, NULL, CallEvent, NULL);
		}
	}
	return 0;
}

int BUTTON(struct mosquitto *mosq, char * jobj){ // @suppress("No return")
	Document document;
	document.Parse(jobj);

	if(document.HasMember("CMD")){
		string cmd = document["CMD"].GetString();
		if (cmd.compare("BUTTON") == 0) {
			int ADR = document["ADR"].GetInt();
			string BUTTONID = document["BUTTONID"].GetString();
			int MODEID = document["MODEID"].GetInt();
			int SCENEID = document["SCENEID"].GetInt();
	//		int SRGBID = document["SRGBID"].GetInt();
			int att = checkTypeAtt(mosq, BUTTONID);
			if(SCENEID == 0){
				string sql = "SELECT EventTriggerId FROM EventTriggerInputDeviceSetupValue WHERE"
						" DeviceUnicastId = "+toString(ADR)+" AND DeviceAttributeId = "+toString(att)+" AND DeviceAttributeValue = "+toString(MODEID)+";";
				CheckType = "";
				DBContext(mosq, "CHECKTYPE", sql);
				string EventTriggerId = toString(CheckType);
				string sqlcheck = "SELECT Code FROM EventTriggerType INNER JOIN EventTrigger ON EventTriggerType.EventTriggerTypeId = EventTrigger.EventTriggerTypeId"
						" WHERE EventTrigger.EventTriggerId = '"+EventTriggerId+"';";
				CheckType = "";
				DBContext(mosq, "CHECKTYPE", sqlcheck);
				if(toString(CheckType).compare("RULE") == 0){
					string DEVICE = "SELECT EventTriggerOutputDeviceSetupValue.DeviceUnicastId, EventTriggerOutputDeviceSetupValue.DeviceAttributeId, EventTriggerOutputDeviceSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
								"FROM EventTriggerOutputDeviceSetupValue INNER JOIN EventTrigger "
								"ON EventTrigger.EventTriggerID = EventTriggerOutputDeviceSetupValue.EventTriggerId "
								" WHERE EventTriggerOutputDeviceSetupValue.EventTriggerId = '"+EventTriggerId+"'  ORDER BY EventTriggerOutputDeviceSetupValue.DeviceAttributeId DESC;";
						DBContext(mosq, "RULEDEVICE", DEVICE);

					string GROUPING = "SELECT EventTriggerOutputGroupingSetupValue.GroupUnicastId, EventTriggerOutputGroupingSetupValue.DeviceAttributeId, EventTriggerOutputGroupingSetupValue.DeviceAttributeValue, EventTrigger.FADE_IN "
								"FROM EventTriggerOutputGroupingSetupValue INNER JOIN EventTrigger "
								"ON EventTrigger.EventTriggerID = EventTriggerOutputGroupingSetupValue.EventTriggerId "
								" WHERE EventTriggerOutputGroupingSetupValue.EventTriggerId = '"+EventTriggerId+"' ORDER BY EventTriggerOutputGroupingSetupValue.DeviceAttributeId DESC;";
						DBContext(mosq, "RULEGROUP", GROUPING);

					string SCENE = "SELECT EventTriggerOutputSceneMapping.SceneUnicastID, EventTrigger.FADE_IN FROM EventTriggerOutputSceneMapping INNER JOIN EventTrigger "
								"ON EventTrigger.EventTriggerID = EventTriggerOutputSceneMapping.EventTriggerId "
								"WHERE EventTriggerOutputSceneMapping.EventTriggerId = '"+EventTriggerId+"';";
						DBContext(mosq, "RULESCENE", SCENE);
				}
			}
		}
	}else if (document.HasMember("ADR") && document.HasMember("LUX")) {
		int LUX = document["LUX"].GetInt();
		int Adr = document["ADR"].GetInt();

		ADRTIMER = -1;
		string adrLux = "SELECT DeviceAttributeID FROM DeviceAttribute WHERE Code = 'LUX';";
		DBContext(mosq, "ADR", adrLux);

		string EQUAL = "SELECT EventTriggerInputDeviceSetupValue.EventTriggerId, LogicalOperatorID FROM EventTriggerInputDeviceSetupValue INNER JOIN EventTrigger "
				" ON EventTriggerInputDeviceSetupValue.EventTriggerId = Eventtrigger.EventtriggerID "
				" WHERE DeviceUnicastId = "+toString(Adr)+""
				" AND CromparisonOperatorId = 1 AND DeviceAttributeId = "+toString(ADRTIMER)+" AND DeviceAttributeValue = "+toString(LUX)+";";
		DBContext(mosq, "LUXPIR", EQUAL);

		string LESS = "SELECT EventTriggerInputDeviceSetupValue.EventTriggerId, LogicalOperatorID FROM EventTriggerInputDeviceSetupValue INNER JOIN EventTrigger "
				" ON EventTriggerInputDeviceSetupValue.EventTriggerId = Eventtrigger.EventtriggerID "
				" WHERE DeviceUnicastId = "+toString(Adr)+""
				" AND CromparisonOperatorId = 3 AND DeviceAttributeId = "+toString(ADRTIMER)+" AND DeviceAttributeValue > "+toString(LUX)+";";
		DBContext(mosq, "LUXPIR", LESS);

		string GREATER = "SELECT EventTriggerInputDeviceSetupValue.EventTriggerId, LogicalOperatorID FROM EventTriggerInputDeviceSetupValue INNER JOIN EventTrigger "
				" ON EventTriggerInputDeviceSetupValue.EventTriggerId = Eventtrigger.EventtriggerID "
				" WHERE DeviceUnicastId = "+toString(Adr)+""
				" AND CromparisonOperatorId = 5 AND DeviceAttributeId = "+toString(ADRTIMER)+" AND DeviceAttributeValue < "+toString(LUX)+";";
		DBContext(mosq, "LUXPIR", GREATER);

		string MEDIAL = "SELECT EventTriggerInputDeviceSetupValue.EventTriggerId, LogicalOperatorID FROM EventTriggerInputDeviceSetupValue INNER JOIN EventTrigger "
				" ON EventTriggerInputDeviceSetupValue.EventTriggerId = Eventtrigger.EventtriggerID "
				" WHERE DeviceUnicastId = "+toString(Adr)+" AND CromparisonOperatorId = 7 AND DeviceAttributeId = "+toString(ADRTIMER)+" AND DeviceAttributeValue < "+toString(LUX)+" AND DeviceAttributeValueMAX > "+toString(LUX)+";";
		DBContext(mosq, "LUXPIR", MEDIAL);

	}else if (document.HasMember("ADR") && document.HasMember("PIR")){
		int PIR = document["PIR"].GetInt();
		int Adr = document["ADR"].GetInt();

		ADRTIMER = -1;
		string adrPIR = "SELECT DeviceAttributeID FROM DeviceAttribute WHERE Code = 'PIR';";
		DBContext(mosq, "ADR", adrPIR);

		string EQUAL = "SELECT EventTriggerInputDeviceSetupValue.EventTriggerId, LogicalOperatorID FROM EventTriggerInputDeviceSetupValue INNER JOIN EventTrigger "
				" ON EventTriggerInputDeviceSetupValue.EventTriggerId = Eventtrigger.EventtriggerID "
				" WHERE DeviceUnicastId = "+toString(Adr)+" AND CromparisonOperatorId = 1 AND DeviceAttributeId = "+toString(ADRTIMER)+" AND DeviceAttributeValue = "+toString(PIR)+";";
		DBContext(mosq, "LUXPIR", EQUAL);
	}
	return 0;
}

void* CallEvent(void *argv){
	cout<<"Bắt đầu !"<<endl;
	sleep(1);
	while(true){
		//set Time Now
		TimerOver = 0;
		weekDayss = setDayTimer();

		cout<<"Check array !"<<endl;
		string sqlStart = "SELECT StartAt FROM EventTrigger WHERE LogicalOperatorID = -1;";
		DBContext(mosq, "STARTAT", sqlStart);
//		string sqlEnd = "SELECT EndAt FROM EventTrigger WHERE LogicalOperatorID = 0;";
//		DBContext(mosq, "ENDAT", sqlEnd);



//		string sqlStart = "SELECT StartAt FROM EventTrigger WHERE " + weekDayss +" = " + '1' + " AND LogicalOperatorID = 0;";
//		DBContext(mosq, "STARTAT", sqlStart);
//		string sqlEnd = "SELECT EndAt FROM EventTrigger WHERE " + weekDayss +" = " + '1' + " AND LogicalOperatorID = 0;";
//		DBContext(mosq, "ENDAT", sqlEnd);

		//Add luồng Start và End vào arr
		readJson(cStartAt);
//		readJson(cEndAt);

		XoaCacPhanTuTrungNhau(arrStart,lengtStart);
//		XoaCacPhanTuTrungNhau(arrEnd,lengtEnd);
//		int lengtArr = lengtStart + lengtEnd;
//		gopArr(arrStart, arrEnd, lengtStart, lengtEnd);

		TimerOver = setTimer();
		if(lengtStart > 0 && TimerOver < arrStart[lengtStart - 1]){
			TimerOver = setTimer();
			cout<<"Hôm nay là:  "<<weekDayss<<endl;
			cout<<"Mảng có :"<<lengtStart<<endl;
			sleep(1);
			for(int i = 0; i< lengtStart; i++){
				TimerOver = setTimer();
				if(TimerOver <= arrStart[lengtStart-1] && TimerOver <= arrStart[i]){
					while (TimerOver < arrStart[i]){
						cout<<"ngủ : "<<arrStart[i] - TimerOver<<"s"<<endl;
						sleep(1);
						TimerOver = setTimer();
						if(thayDoi == 1){
							thayDoi = 0;
							pthread_exit(NULL);
						}
					}
					for(int j = 0; j < lengtStart; j++){
						if(arrStart[i] == arrStart[j]){
							TimerOver = arrStart[i];
							string sql = "SELECT EventTriggerType.code FROM EventTriggerType INNER JOIN EventTrigger "\
									"ON EventTriggerType.EventTriggerTypeId = EventTrigger.EventTriggerTypeId "\
									"WHERE EventTrigger.StartAt = '" + parthTime(arrStart[i]) + "';";
							DBContext(mosq, "CALLRULE", sql);

						}
					}
				}
			}
		}else if(lengtStart > 0 && TimerOver > arrStart[lengtStart - 1]){
			while(TimerOver > arrStart[lengtStart - 1]){
				sleep(1);
			}
		}else {
			while(lengtStart == 0){
				TimerOver = setTimer();
				if(thayDoi == 1){
					thayDoi = 0;
					pthread_exit(NULL);
				}
				sleep(1);
			}
		}
	}
}

int tachTimer(char* a){
	int dem = 0;
	char h[23] = {0};
	char m[23] = {0};

	int timer;

	for(int i = 0; i < (int)strlen(a); i++){
		if(dem == 0){
			for(int j =0; j < (int)strlen(a); j++){
				if(a[j] == ':'){
					dem ++;
					i = j;
					break;
				}else{
					h[j] = a[j];
				}
			}
		}else if(dem == 1){
			for(int k = 0; k < (int)strlen(a); k++){
				if(a[k+i] == '\0'){
					i = strlen(a);
					break;
				}else{
					m[k] = a[k + i];
				}
			}
		}
	}

	timer = atoi(h)* 3600 + atoi(m)* 60;

	return timer;
}

string parthTime(int a){
	int h = a/3600;
	int m = (a%3600)/60;
	string timerCheck = toString(h) + ":" + toString(m);

	return timerCheck;
}

void sapXepArr(int arr[], int lengt){
	for (int i = 0; i < lengt; i++) {
		for (int j = i + 1; j < lengt; j++) {
			if (arr[i] > arr[j]) {
				// Nếu arr[i] > arr[j] thì hoán đổi giá trị của arr[i] và arr[j]
				int temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}
}

void readJson(char* s){
	Document document;
	document.Parse(s);
	if(document.HasMember("STARTAT")){
		const Value& data = document["STARTAT"];
		if(lengtStart > 0){
			delete[] arrStart;
		}
		lengtStart = data.Size();
		arrStart = new int[data.Size()];
		for (rapidjson::SizeType i = 0; i < data.Size(); i++){
			arrStart[i] = data[i].GetInt();
		}
		sapXepArr(arrStart, data.Size());
	}
//	else if(document.HasMember("ENDAT")){
//		const Value& data = document["ENDAT"];
//		delete[] arrEnd;
//		lengtEnd = data.Size();
//		arrEnd = new int[data.Size()];
//		for (rapidjson::SizeType i = 0; i < data.Size(); i++){
//			arrEnd[i] = data[i].GetInt();
//		}
//		sapXepArr(arrEnd, data.Size());
//	}
}

//void gopArr(int arrS[], int arrE[], int n, int m){
//	arr = new int[n + m];
//	for(int i = 0; i < (n + m); i++){
//		if(i < n){
//			arr[i] = arrS[i];
//		}else {
//			arr[i] = arrE[i-n];
//		}
//	}
//	sapXepArr(arr, n + m);
//}

int setTimer(){
	int timeNow;
	time_t now = time(0);
	tm *ltm = localtime(&now);
	//gán biến
	int h = ltm->tm_hour;
	int m = ltm->tm_min;
	int s = 1 + ltm->tm_sec;
	//setup Time
	if(m == 60){
		h ++;
		m = m - 60;
	}else if( s == 60 ){
		m ++;
		s = s - 60;
	}
	timeNow = h * 3600 + m * 60 + s;
	return timeNow;
}

string setDayTimer(){
	string weekDays;

	time_t now = time(0);
	tm *ltm = localtime(&now);

	int w = 1 + ltm->tm_wday;
	//Day in Week
	switch(w){
		case 1:
			weekDays = "EachSunday";
			break;
		case 2:
			weekDays = "EachMonday";
			break;
		case 3:
			weekDays = "EachTuesday";
			break;
		case 4:
			weekDays = "EachWednesday";
			break;
		case 5:
			weekDays = "EachThursday";
			break;
		case 6:
			weekDays = "EachFirday";
			break;
		case 7:
			weekDays = "EachSaturday";
			break;
		default:
			break;
	}
	return weekDays;
}

void Xoa1PhanTu(int a[], int &n, int ViTriXoa){
    for(int i = ViTriXoa; i < n; i++){
        a[i] = a[i + 1];
    }
    n--;
}

void XoaCacPhanTuTrungNhau(int a[], int &n)
{
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            if(a[i] == a[j]){
                Xoa1PhanTu(a, n, j);
                j--;
            }
        }
    }
}

char* checkControlTimer(struct mosquitto *mosq, int idControl){
	string sql = "SELECT Code FROM DeviceAttribute WHERE DeviceAttributeId = " + toString(idControl) +";";
	DBContext(mosq, "CHECKTYPE", sql);
	string a = toString(CheckType);
	char * control = new char[a.length()+1];
	strcpy(control, a.c_str());

	return control;
}

int checkTypeAtt(struct mosquitto *mosq, string Code){
	int typeId;
	ADRTIMER = -1;
	string sql = "select DeviceAttributeID from DeviceAttribute where Code ='"+Code+"';";
	DBContext(mosq, "ADR", sql);
	typeId = ADRTIMER;
	return typeId;
}






















