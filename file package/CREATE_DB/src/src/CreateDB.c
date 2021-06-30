/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Toggles GPIO's 23 and 24 recursively. Press Ctrl+C to exit
 *
 */

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <json-c/json.h>


struct json_object *jobj, *json;

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s", (const char*)data);
   jobj = json_object_new_object();
   for(i = 0; i<argc; i++){
	   json_object_object_add(jobj, (const char*)azColName[i], json_object_new_string((const char*)argv[i]));
   }

   json_object_array_add(json, json_object_get(jobj));
//   printf("---\n%s\n---\n", json_object_to_json_string(json));
//   printf("%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PRETTY));
   return 0;
}

int main(int argc, char* argv[]) {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql, *sql1,*sql2, *sql3,*sql4, *sql5, *sql6, *sql7, *sql8, *sql9, *sql10, *sql11, *sql12, *sql13, *sql14, *sql15, *sql16, *sql17, *sql18, *sql19, *sql20;
	const char* data = "";


	//JOSON
	json = json_object_new_array();

	//open database
   rc = sqlite3_open("/root/rd.Sqlite", &db);

   //check conect
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }
   //Create SQL

   sql = "CREATE TABLE GroupingDeviceMapping ("\
    "GroupingId VACHAR NOT NULL,"\
    "GroupUnicastId INTEGER NOT NULL,"\
    "DeviceId   VACHAR NOT NULL,"\
    "DeviceUnicastId INTEGER NOT NULL,"\
    "PRIMARY KEY (GroupingId,DeviceId));";

//   sql9 = "insert into GroupingDeviceMapping(GroupingId, DeviceId) values(49160,2);";
//   sql10 = "insert into GroupingDeviceMapping(GroupingId, DeviceId) values(49160,4);";

   sql1 = "CREATE TABLE DeviceAttributeValue ("\
    "DeviceId        VACHAR              NOT NULL,"\
    "DeviceUnicastId              INTEGER NOT NULL,"\
    "DeviceAttributeId              INTEGER NOT NULL,"\
    "Value        INTEGER             ,"\
    "UpdateDay        INTEGER             ,"\
	"UpdateTime        INTEGER             ,"\
    "PRIMARY KEY ("\
	  "  DeviceId,"\
       " DeviceAttributeId));";

   sql2 = "CREATE TABLE GROUPING ("\
	"GroupingId       VARCHAR PRIMARY KEY                         NOT NULL,"\
	"GroupUnicastId       INTEGER ,"\
    "Name       VARCHAR (1000) ,"\
    "CategoryId       INTEGER ,"\
    "CreatedAt  DATETIME,"\
    "UpdatedAt  DATETIME,"\
    "DeletedAt  DATETIME);";

   sql11 = "CREATE TABLE DeviceAttribute ("\
       "DeviceAttributeID INTEGER        PRIMARY KEY                         NOT NULL,"\
       "Code       VARCHAR (1000) ,"\
       "Name  VARCHAR (1000));"\

   "CREATE TABLE EventTriggerType ("\
          "EventTriggerTypeId INTEGER        PRIMARY KEY                         NOT NULL,"\
          "Code       VARCHAR (1000) ,"\
          "Name  VARCHAR (1000));"\
		  "insert into EventTriggerType(EventTriggerTypeId, Code, Name) values(1 , 'SCENE', '');"\
		  "insert into EventTriggerType(EventTriggerTypeId, Code, Name) values(2 , 'RULE', '');";

   sql12 = "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(0 , 'ONOFF', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(1 , 'DIM', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(2 , 'CCT', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(3 , 'HUE', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(4 , 'SATURATION', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(5 , 'LIGHTNESS', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(6 , 'SONG', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(7 , 'BLINK_MODE', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(8 , 'BATTERY', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(9 , 'LUX', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(10 , 'PIR', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(11 , 'BUTTON_1', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(12 , 'BUTTON_2', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(13 , 'BUTTON_3', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(14 , 'BUTTON_4', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(15 , 'BUTTON_5', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(16 , 'BUTTON_6', '');"\
		   "insert into DeviceAttribute(DeviceAttributeID, Code, Name) values(17 , 'ACTIME', '');";

   sql20 = "CREATE TABLE CromparisonOperator ("\
             "CromparisonOperatorId INTEGER        PRIMARY KEY                         NOT NULL,"\
             "Code       VARCHAR (1000) ,"\
             "Name  VARCHAR (1000));"\

		   "insert into CromparisonOperator(CromparisonOperatorId, Code, Name) values(1 , 'EQUAL', '=');"\
   		   "insert into CromparisonOperator(CromparisonOperatorId, Code, Name) values(2 , 'NOT EQUAL', '!=');"\
   		   "insert into CromparisonOperator(CromparisonOperatorId, Code, Name) values(3 , 'LESS', '<');"\
   		   "insert into CromparisonOperator(CromparisonOperatorId, Code, Name) values(4 , 'LESS EQUAL', '<=');"\
   		   "insert into CromparisonOperator(CromparisonOperatorId, Code, Name) values(5 , 'GREATER', '>');"\
   		   "insert into CromparisonOperator(CromparisonOperatorId, Code, Name) values(6 , 'GREATER EQUAL', '>=');";
   	   	   "insert into CromparisonOperator(CromparisonOperatorId, Code, Name) values(7 , 'MEDIAL', '<<');";



   sql3 = "CREATE TABLE EventTriggerOutputDeviceMapping ("\
    "EventTriggerId VARCHAR,"\
    "DeviceId VARCHAR ,"\
	"DeviceUnicastId INTEGER ,"\
	"typerun    INTEGER,"\
	"PRIMARY KEY ("\
		  "  DeviceId,"\
	       " EventTriggerId));";

   sql4 = "CREATE TABLE EventTriggerOutputGroupingMapping ("\
       "EventTriggerId VARCHAR,"\
       "GroupingId VARCHAR ,"\
	   "GroupUnicastId INTEGER ,"\
	   "typerun    INTEGER,"\
   	"PRIMARY KEY ("\
   		  "  GroupingId,"\
   	       " EventTriggerId));";

   sql5 = "CREATE TABLE EventTriggerOutputDeviceSetupValue ("\
    "EventTriggerId       VARCHAR,"\
    "DeviceId             VARCHAR,"\
	"DeviceUnicastId INTEGER ,"\
    "DeviceAttributeId    INTEGER,"\
    "DeviceAttributeValue INTEGER,"\
	"PRIMARY KEY ("\
	   		  "  EventTriggerId,"\
			  "  DeviceId,"\
	   	       " DeviceAttributeId));";

   sql6 = "CREATE TABLE EventTriggerOutputGroupingSetupValue ("\
       "EventTriggerId       VARCHAR,"\
       "GroupingId             VARCHAR,"\
	   "GroupUnicastId INTEGER ,"\
       "DeviceAttributeId    INTEGER,"\
       "DeviceAttributeValue INTEGER,"\
   	"PRIMARY KEY ("\
   	   		  "  EventTriggerId,"\
   			  "  GroupingId,"\
   	   	       " DeviceAttributeId));";

   sql7 = "CREATE TABLE EventTrigger ("\
    "EventTriggerId        VARCHAR PRIMARY KEY NOT NULL,"\
    "GroupId               INTEGER,"\
    "EventTriggerTypeId    BIGINT,"\
	"SceneUnicastID INTEGER,"\
    "Priority              BIGINT,"\
    "Name                  VARCHAR,"\
	"LogicalOperatorID           INTEGER,"\
    "HasTimer              INTEGER,"\
    "StartAt               TIME,"\
    "EndAt                 TIME,"\
	"ValueCreate       INTEGER ,"\
	"StatusID       INTEGER ,"\
    "HasRepeater           INTEGER,"\
    "EachMonday            INTEGER,"\
    "EachTuesday           INTEGER,"\
    "EachWednesday         INTEGER,"\
    "EachThursday          INTEGER,"\
    "EachFriday            INTEGER,"\
    "EachSaturday          INTEGER,"\
    "EachSunday            INTEGER,"\
    "NotificationUsed      INTEGER,"\
    "FADE_IN INTEGER);";


   sql13 = "CREATE TABLE Device ("\
       "DeviceId        VARCHAR PRIMARY KEY NOT NULL,"\
       "DeviceUnicastId               INTEGER,"\
       "AppKey    VARCHAR,"\
       "NetKey              VARCHAR,"\
       "DeviceKey                  VARCHAR,"\
	   "DeviceTypeId                  INTEGER,"\
	   "UpdateDay                  INTEGER,"\
	   "UpdateTime                  INTEGER,"\
       "StatusId              INTEGER,"\
       "Owner INTEGER);";

   sql14 = "CREATE TABLE GROUPID ("\
    "GroupUnicastId INTEGER PRIMARY KEY"\
    "                       NOT NULL,"\
    "GroupingId     VARCHAR,"\
    "ValueCreate    INTEGER"\
	");";

   sql15 = "CREATE TABLE EventTriggerID ("\
       "SceneUnicastID INTEGER PRIMARY KEY"\
       "                       NOT NULL,"\
       "EventTriggerId     VARCHAR,"\
       "ValueCreate    INTEGER"\
   	");";


   sql16 = "CREATE TABLE EventTriggerInputDeviceMapping ("\
   "EventTriggerId VARCHAR,"\
   "DeviceId VARCHAR ,"\
   	"DeviceUnicastId INTEGER ,"\
   	"PRIMARY KEY ("\
   		  "  DeviceId,"\
   	       " EventTriggerId));";

   sql17 = "CREATE TABLE EventTriggerInputGroupingMapping ("\
	  "EventTriggerId VARCHAR,"\
	  "GroupingId VARCHAR ,"\
   	   "GroupUnicastId INTEGER ,"\
	   "PRIMARY KEY ("\
      		  "  GroupingId,"\
      	       " EventTriggerId));";


   sql18 = "CREATE TABLE EventTriggerInputDeviceSetupValue ("\
       "EventTriggerId       VARCHAR,"\
       "DeviceId             VARCHAR,"\
   	"DeviceUnicastId INTEGER ,"\
       "DeviceAttributeId    INTEGER,"\
       "CromparisonOperatorId    INTEGER,"\
       "DeviceAttributeValue INTEGER,"\
	   "DeviceAttributeValueMAX INTEGER,"\
   	"PRIMARY KEY ("\
   	   		  "  EventTriggerId,"\
   			  "  DeviceId,"\
   	   	       " DeviceAttributeId));";
   sql19 = "CREATE TABLE EventTriggerOutputSceneMapping ("\
         "EventTriggerId VARCHAR,"\
         "SceneId VARCHAR ,"\
  	   "SceneUnicastId INTEGER ,"\
		"typerun    INTEGER,"\
     	"PRIMARY KEY ("\
     		  "  SceneId,"\
     	       " EventTriggerId));";

//      sql19 = "CREATE TABLE EventTriggerInputGroupingSetupValue ("\
//          "EventTriggerId       VARCHAR,"\
//          "GroupingId             VARCHAR,"\
//   	   "GroupUnicastId INTEGER ,"\
//          "DeviceAttributeId    INTEGER,"\
//          "DeviceAttributeValue INTEGER,"\
//      	"PRIMARY KEY ("\
//      	   		  "  EventTriggerId,"\
//      			  "  GroupingId,"\
//      	   	       " DeviceAttributeId));";

//   sql8 = "insert into EventTrigger(EventTriggerId, GroupId, EventTriggerTypeId) values(1,5,1);";


//   sql3 = "CREATE TABLE SCENE ("\
//    "SceneId  INTEGER        PRIMARY KEY,"\
//    "RoomId   INTEGER,"\
//    "Name     VARCHAR (1000),"\
//    "Remind   VARCHAR (1000),"\
//    "Timer    VARCHAR (1000),"\
//    "Favorite INTEGER );";
//
//   sql4 = "CREATE TABLE SceneDeviceMapping ("\
//       "SceneId    INTEGER NOT NULL,"\
//       "DeviceId INTEGER NOT NULL,"\
//       "ONOFF      INTEGER,"\
//       "CCT        INTEGER,"\
//       "DIM        INTEGER,"\
//       "PRIMARY KEY ("\
//       "    SceneId,"\
//       "    DeviceId"\
//       "));";
//
//   sql5 = "CREATE TABLE SceneGroupingMapping ("\
//    "SceneId    INTEGER NOT NULL,"\
//    "GroupingId INTEGER NOT NULL,"\
//    "ONOFF      INTEGER,"\
//    "CCT        INTEGER,"\
//    "DIM        INTEGER,"\
//    "PRIMARY KEY ("\
//    "    SceneId,"\
//    "    GroupingId"\
//    "));";



   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql1, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql2, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql3, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql4, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql5, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql6, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql7, callback, (void*)data, &zErrMsg);
//   rc = sqlite3_exec(db, sql8, callback, (void*)data, &zErrMsg);
//   rc = sqlite3_exec(db, sql9, callback, (void*)data, &zErrMsg);
//   rc = sqlite3_exec(db, sql10, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql11, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql12, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql13, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql14, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql15, callback, (void*)data, &zErrMsg);


   rc = sqlite3_exec(db, sql16, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql17, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql18, callback, (void*)data, &zErrMsg);
   rc = sqlite3_exec(db, sql19, callback, (void*)data, &zErrMsg);


   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
	   printf("%s\n", json_object_to_json_string(json));
//	   FILE *fp;
//	   fp = fopen("DemoJson.json", "w");
//	   fputs(json_object_to_json_string(json), fp);
//	   fclose(fp);
   }
   sqlite3_close(db);
   return 0;
}






//INSERT INTO EVENTTRIGGER (EventTriggerId, GroupId, EventTriggerTypeId, StartAt, EndAt) VALUES(4,4,1,'10:0:10','15:0:60');
