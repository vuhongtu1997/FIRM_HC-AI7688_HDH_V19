/*
 * Json.c
 */

#include "JsonProcess.h"
#include "MQTT.h"
#include "Light.h"
#include "Provision.h"
#include "slog.h"

uint16_t valueObject[20];
uint16_t numObject,scene_id;
jsonstring vrts_Json_String;
defineCmd flagDefineCmd;
pthread_t vrts_System_TestSend;

bool arr_json = false;
bool flag_addscene = false;
bool flag_delscene = false;

bool check_resetnode = false;
uint16_t adr_dv[100];
int i, arraylen, lst_count_id;
bool time_check = false;

char flagSecond=0;


uint16_t Transition(uint16_t parTime){
	uint16_t transition;
	switch (parTime){
	case 0:
		transition = 0x0000;
		break;
	case 1:
		transition = 0x007e;
		break;
	case 5:
		transition = 0x009e;
		break;
	case 10:
		transition = 0x00c1;
		break;
	case 20:
		transition = 0x00c2;
		break;
	case 30:
		transition = 0x00c3;
		break;
	}
	return transition;
}
/*
 * TODO: xử lý chuỗi json phức tạp không dùng delay
 */
void JsonControl(json_object *jobj,char *key){
	if(strcmp(key,"ADR")==0){
		if(check_resetnode)
		{
			check_resetnode = false;
			 FunctionPer(HCI_CMD_GATEWAY_CMD, ResetNode_typedef, vrts_Json_String.adr, NULL8, NULL8, \
					 NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, NULL16, 12);
		}
	}
	if(strcmp(key,"ONOFF")==0){
		 flagDefineCmd = onoff_enum;
		//  vrts_Json_String.minutes 		= (json_object_get_int(json_object_object_get(jobj,"TIME")));
		//  FunctionPer(HCI_CMD_GATEWAY_CMD,ControlOnOff_typedef,vrts_Json_String.adr ,NULL8, vrts_Json_String.onoff, \
		// 		 NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16,Transition(vrts_Json_String.minutes),16);
		//  usleep(400000);
		//  vrts_Json_String.minutes = 0;
	 }
	 if(strcmp(key,"CCT")==0){
		flagDefineCmd = cct_enum;
		// vrts_Json_String.minutes 		= (json_object_get_int(json_object_object_get(jobj,"TIME")));
		// FunctionPer(HCI_CMD_GATEWAY_CMD, CCT_Set_typedef, vrts_Json_String.adr, \
		// 		NULL8, NULL8, NULL16,Percent2ParamCCT(vrts_Json_String.cct), NULL16, NULL16,NULL16, NULL16, NULL16, Transition(vrts_Json_String.minutes),19);
		// usleep(400000);
		// vrts_Json_String.minutes = 0;
	 }
	 if(strcmp(key,"DIM")==0){
		flagDefineCmd = dim_enum;
		// vrts_Json_String.minutes = (json_object_get_int(json_object_object_get(jobj,"TIME")));
		// FunctionPer(HCI_CMD_GATEWAY_CMD, Lightness_Set_typedef, vrts_Json_String.adr, NULL8, NULL8, \
		// 	 Percent2ParamDIM(vrts_Json_String.dim), NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, Transition(vrts_Json_String.minutes),17);
		// usleep(400000);
		// vrts_Json_String.minutes = 0;
	 }
	 if(strcmp(key,"HOURS")==0){
		 flagDefineCmd = hours_enum;
	 }
	 if(strcmp(key,"MINUTES")==0){
		 flagDefineCmd = minutes_enum;
	 }
	 if(strcmp(key,"SECONDS")==0){
		 flagDefineCmd = seconds_enum;
		 flagSecond= 1;
	 }
	 if(strcmp(key,"ADDGROUP")==0){
		 flagDefineCmd = addgroup_enum;
		 check_add_or_del_group= true;
		 uint8_t time_Send = 0;
		 for(i=0; i<arraylen; i++){
			 //if(flag_SendCmd_Done){
				 FunctionPer(HCI_CMD_GATEWAY_CMD, AddGroup_typedef, adr_dv[i], vrts_Json_String.addgroup , NULL8, NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, NULL16,18);
				 usleep(400000);
			 //}
		 }
	 }
	 if(strcmp(key,"DELGROUP")==0){
		 flagDefineCmd = delgroup_enum;
		 check_add_or_del_group= false;
		 for(i=0; i<arraylen; i++){
			 FunctionPer(HCI_CMD_GATEWAY_CMD, DelGroup_typedef, adr_dv[i], vrts_Json_String.delgroup, NULL8, NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, NULL16,18);
			 usleep(400000);
		 }
	 }
	 if(strcmp(key,"ADDSCENE")==0){
		 flagDefineCmd = addscene_enum;
		 scene_id = vrts_Json_String.addscene;
		 flag_addscene = true;
		 check_add_or_del_scene= true;
		 for(i=0;i<arraylen;i++){
			 FunctionPer(HCI_CMD_GATEWAY_CMD, AddSence_typedef, adr_dv[i], NULL8, NULL8, NULL16, NULL16, scene_id, NULL16,NULL16, NULL16, NULL16, NULL16,14);
			 usleep(400000);
		 }
	 }
	 if(strcmp(key,"CALLSCENE")==0){
		flagDefineCmd = callscene_enum;
		// vrts_Json_String.minutes = (json_object_get_int(json_object_object_get(jobj,"TIME")));
		// FunctionPer(HCI_CMD_GATEWAY_CMD, CallSence_typedef, NULL8, NULL8, NULL8, NULL16, NULL16, vrts_Json_String.callscene, \
		// 	 NULL16,NULL16, NULL16, NULL16,Transition(vrts_Json_String.minutes), 17);
		// usleep(400000);
		// vrts_Json_String.minutes = 0;
	 }
	 if(strcmp(key,"DELSCENE")==0){
		 flagDefineCmd = delscene_enum;
		 check_add_or_del_scene= false;
		 for(i=0; i<arraylen; i++){
			 FunctionPer(HCI_CMD_GATEWAY_CMD, DelSence_typedef, adr_dv[i], NULL8, NULL8, NULL16, NULL16,\
					 vrts_Json_String.delscene, NULL16,NULL16, NULL16, NULL16, NULL16,14);
			 usleep(400000);
		 }
	 }
	 if(strcmp(key,"HUE")==0){
		flagDefineCmd = hue_eum;
		// vrts_Json_String.minutes = (json_object_get_int(json_object_object_get(jobj,"TIME")));
		// FunctionPer(HCI_CMD_GATEWAY_CMD, HSL_Set_typedef, vrts_Json_String.adr, NULL8, NULL8, NULL16,\
		// 	 NULL16, NULL16, NULL16,vrts_Json_String.lightness, vrts_Json_String.hue, \
		// 	 vrts_Json_String.saturation, Transition(vrts_Json_String.minutes),21);
		// usleep(400000);
		// vrts_Json_String.minutes = 0;
	 }
	if(strcmp(key,"TIME") == 0){
		 switch (flagDefineCmd){
		 	case onoff_enum:
		 	 FunctionPer(HCI_CMD_GATEWAY_CMD,ControlOnOff_typedef,vrts_Json_String.adr ,NULL8, vrts_Json_String.onoff, \
		 					 NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16,Transition(vrts_Json_String.minutes),16);
		 			 usleep(400000);
		 			 break;
		 	case cct_enum:
		 	FunctionPer(HCI_CMD_GATEWAY_CMD, CCT_Set_typedef, vrts_Json_String.adr, \
		 			NULL8, NULL8, NULL16,Percent2ParamCCT(vrts_Json_String.cct), NULL16, NULL16,NULL16, NULL16, NULL16, Transition(vrts_Json_String.minutes),19);
		 	usleep(400000);
		 	 break;
		 	case dim_enum:
		 	FunctionPer(HCI_CMD_GATEWAY_CMD, Lightness_Set_typedef, vrts_Json_String.adr, NULL8, NULL8, \
		 		 Percent2ParamDIM(vrts_Json_String.dim), NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, Transition(vrts_Json_String.minutes),17);
		 	usleep(400000);
		 	 break;
		 	case callscene_enum:
		 	FunctionPer(HCI_CMD_GATEWAY_CMD, CallSence_typedef, NULL8, NULL8, NULL8, NULL16, NULL16, vrts_Json_String.callscene, \
		 		 NULL16,NULL16, NULL16, NULL16,Transition(vrts_Json_String.minutes), 17);
		 	usleep(400000);
		 	break;
		 	case hue_eum:
				FunctionPer(HCI_CMD_GATEWAY_CMD, HSL_Set_typedef, vrts_Json_String.adr, NULL8, NULL8, NULL16,\
					 NULL16, NULL16, NULL16,vrts_Json_String.lightness, vrts_Json_String.hue, \
					 vrts_Json_String.saturation, Transition(vrts_Json_String.minutes),21);
				usleep(400000);
		 	}
		 	vrts_Json_String.minutes = 0;
		 	flagDefineCmd = null_enum;
	 }
	 else
	 {
		 vrts_Json_String.minutes = 0;
		 if((strcmp(key,"ONOFF") != 0) && (strcmp(key,"CCT") != 0) && (strcmp(key,"DIM") != 0) && \
		 (strcmp(key,"CALLSCENE") != 0) && (strcmp(key,"HUE") !=0 )){
			 switch (flagDefineCmd){
			 	case onoff_enum:
			 	 FunctionPer(HCI_CMD_GATEWAY_CMD,ControlOnOff_typedef,vrts_Json_String.adr ,NULL8, vrts_Json_String.onoff, \
			 					 NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16,Transition(vrts_Json_String.minutes),16);
			 			 usleep(400000);
			 			 break;
			 	case cct_enum:
			 	FunctionPer(HCI_CMD_GATEWAY_CMD, CCT_Set_typedef, vrts_Json_String.adr, \
			 			NULL8, NULL8, NULL16,Percent2ParamCCT(vrts_Json_String.cct), NULL16, NULL16,NULL16, NULL16, NULL16, Transition(vrts_Json_String.minutes),19);
			 	usleep(400000);
			 	 break;
			 	case dim_enum:
			 	FunctionPer(HCI_CMD_GATEWAY_CMD, Lightness_Set_typedef, vrts_Json_String.adr, NULL8, NULL8, \
			 		 Percent2ParamDIM(vrts_Json_String.dim), NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, Transition(vrts_Json_String.minutes),17);
			 	usleep(400000);
			 	 break;
			 	case callscene_enum:
			 	FunctionPer(HCI_CMD_GATEWAY_CMD, CallSence_typedef, NULL8, NULL8, NULL8, NULL16, NULL16, vrts_Json_String.callscene, \
			 		 NULL16,NULL16, NULL16, NULL16,Transition(vrts_Json_String.minutes), 17);
			 	usleep(400000);
			 	break;
			 	case hue_eum:
					FunctionPer(HCI_CMD_GATEWAY_CMD, HSL_Set_typedef, vrts_Json_String.adr, NULL8, NULL8, NULL16,\
						 NULL16, NULL16, NULL16,vrts_Json_String.lightness, vrts_Json_String.hue, \
							vrts_Json_String.saturation, Transition(vrts_Json_String.minutes),21);
					usleep(400000);
			 	}
			 	vrts_Json_String.minutes = 0;
			 	flagDefineCmd = null_enum;
		 }
	 }
	 if(strcmp(key,"CMD")==0){
		 if(strcmp(vrts_Json_String.cmd,"SCAN")==0){
			slog_print(SLOG_INFO, 1, "<provision>Provision start");
			MODE_PROVISION=true;
			pthread_create(&vrts_System_TestSend,NULL, ProvisionThread, NULL);
		 }
		 else if(strcmp(vrts_Json_String.cmd,"STOP")==0){
			slog_print(SLOG_INFO, 1, "<provision>Provision stop");
			MODE_PROVISION=false;
			ControlMessage(3, OUTMESSAGE_ScanStop);
			//pthread_cancel(tmp);
			flag_selectmac     = false;
			flag_getpro_info   = false;
			flag_getpro_element= false;
			flag_provision     = false;
			flag_mac           = true;
			flag_check_select_mac  = false;
			flag_done          = true;
			flag_set_type = false;
			ControlMessage(3, OUTMESSAGE_ScanStop);
			struct json_object *object;
			object = json_object_new_object();
			json_object_object_add(object, "CMD", json_object_new_string("STOP"));
			char *rsp;
			rsp = json_object_to_json_string(object);
			mosquitto_publish(mosq, NULL, "RD_STATUS", strlen(rsp),rsp,  qos, 0);
			slog_info("(mqtt)Message_send:%s",rsp);

			//for gpio
//			flag_blink = false;
//			led_pin_off(gpio[LED_BLE_PIN_INDEX]);
		 }
		 else if(strcmp(vrts_Json_String.cmd,"RESETNODE")==0){
			 enum json_type type;
			 int valueArray[100];
			 json_object *array_object = json_object_object_get(jobj,"ADR");
			 type = json_object_get_type(array_object);
			 if(type == json_type_array){
				 int arrayLength = json_object_array_length(array_object);
				 int i;
				 for(i=0;i<arrayLength;i++){
					 json_object * value_Object = json_object_array_get_idx(array_object, i);
					 type = json_object_get_type(value_Object);
					 if(type == json_type_int){
						 valueArray[i]= json_object_get_int(value_Object);
						 //printf("Array[%d] = %d\n",i,valueArray[i]);
						 FunctionPer(HCI_CMD_GATEWAY_CMD, ResetNode_typedef, valueArray[i], NULL8, NULL8, \
								 NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, NULL16, 12);
						 usleep(400000);
					 }
				 }
			 }
		 }
		 else if(strcmp(vrts_Json_String.cmd,"SETSCENEFORREMOTE") == 0){
			 vrts_Json_String.adr        	= (json_object_get_int(json_object_object_get(jobj,"ADR")));
			 vrts_Json_String.buttonid      = (json_object_get_string(json_object_object_get(jobj,"BUTTONID")));
			 uint8_t buttonId_int;
			 if(strcmp(vrts_Json_String.buttonid,"BUTTON_1")==0){
				 buttonId_int =1;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_2")==0){
				 buttonId_int =2;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_3")==0){
				 buttonId_int =3;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_4")==0){
				 buttonId_int =4;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_5")==0){
				 buttonId_int =5;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_6")==0){
				 buttonId_int =6;
			 }
			 vrts_Json_String.modeid        = (json_object_get_int(json_object_object_get(jobj,"MODEID")));
			 vrts_Json_String.srgbID        = (json_object_get_int(json_object_object_get(jobj,"SRGBID")));
			 vrts_Json_String.sceneID       = (json_object_get_int(json_object_object_get(jobj,"SCENEID")));
				Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForRemote_vendor_typedef, vrts_Json_String.adr, NULL16, buttonId_int,\
						vrts_Json_String.modeid, NULL8, NULL16, NULL16, NULL16,\
						NULL16, vrts_Json_String.sceneID, vrts_Json_String.sceneID, vrts_Json_String.srgbID, NULL8, NULL8, NULL8,31);
				usleep(400000);
		 }
		 else if(strcmp(vrts_Json_String.cmd,"DELSCENEFORREMOTE") == 0){
			 vrts_Json_String.adr        	= (json_object_get_int(json_object_object_get(jobj,"ADR")));
			 vrts_Json_String.buttonid      = (json_object_get_string(json_object_object_get(jobj,"BUTTONID")));
			 vrts_Json_String.modeid        = (json_object_get_int(json_object_object_get(jobj,"MODEID")));
			 uint8_t buttonId_int;
			 if(strcmp(vrts_Json_String.buttonid,"BUTTON_1")==0){
				 buttonId_int =1;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_2")==0){
				 buttonId_int =2;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_3")==0){
				 buttonId_int =3;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_4")==0){
				 buttonId_int =4;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_5")==0){
				 buttonId_int =5;
			 }
			 else if(strcmp(vrts_Json_String.buttonid,"BUTTON_6")==0){
				 buttonId_int =6;
			 }
			 puts("1");
				Function_Vendor(HCI_CMD_GATEWAY_CMD, DelSceneForRemote_vendor_typedef, vrts_Json_String.adr, NULL16, buttonId_int,\
						vrts_Json_String.modeid, NULL8, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, NULL8, NULL8, NULL8, NULL8,31);
				puts("2");
				usleep(400000);
		 }
		 else if(strcmp(vrts_Json_String.cmd,"SETSCENEFORSENSOR")==0){
			 vrts_Json_String.adr        		= (json_object_get_int(json_object_object_get(jobj,"ADR")));
			 vrts_Json_String.sceneforsensor 	= (json_object_get_int(json_object_object_get(jobj,"SCENEID")));
			 vrts_Json_String.srgbID 			= (json_object_get_int(json_object_object_get(jobj,"SRGBID")));
			 vrts_Json_String.type 				= (json_object_get_int(json_object_object_get(jobj,"TYPE")));
			 if(vrts_Json_String.type == 0){
				 vrts_Json_String.lightsensor = json_object_object_get(jobj,"LIGHT_SENSOR");
				 vrts_Json_String.condition = (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"CONDITION")));
				 if(vrts_Json_String.condition == 3 || vrts_Json_String.condition == 4 ){
					 uint16_t parCondition = 256;
					 vrts_Json_String.low_lux 	= (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"LOW_LUX")));
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
							 parCondition, vrts_Json_String.low_lux, NULL16, NULL16, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
				 else if (vrts_Json_String.condition == 5 || vrts_Json_String.condition == 6){
					 uint16_t parCondition = 768;
					 vrts_Json_String.low_lux = (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"LOW_LUX")));
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
					 		parCondition, vrts_Json_String.low_lux, NULL16 , NULL16, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
				 else if(vrts_Json_String.condition == 7){
					 uint16_t parCondition = 1024;
					 vrts_Json_String.low_lux = (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"LOW_LUX")));
					 vrts_Json_String.hight_lux = (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"HIGHT_LUX")));
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
							parCondition, vrts_Json_String.low_lux, vrts_Json_String.hight_lux, NULL16, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
				 else if(vrts_Json_String.condition == 1){
					 uint16_t parCondition = 512;
					 vrts_Json_String.low_lux 	= (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"LOW_LUX")));
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
							 parCondition, vrts_Json_String.low_lux, NULL16, NULL16, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
			 }
			 else if(vrts_Json_String.type == 1){
				 vrts_Json_String.pir = json_object_object_get(jobj,"PIR_SENSOR");
				 vrts_Json_String.motion = (json_object_get_int(json_object_object_get(vrts_Json_String.pir,"PIR")));
				 if(vrts_Json_String.motion == 1){
					 uint16_t parCondition = 1;
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 					parCondition, NULL16, NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
				 if(vrts_Json_String.motion == 0){
					 uint16_t parCondition = 2;
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 					parCondition, NULL16, NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
			 }
			 else if(vrts_Json_String.type == 2){
				 vrts_Json_String.lightsensor = json_object_object_get(jobj,"LIGHT_SENSOR");
				 vrts_Json_String.pir  		  = json_object_object_get(jobj,"PIR_SENSOR");
				 vrts_Json_String.condition = (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"CONDITION")));
				 vrts_Json_String.low_lux 	= (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"LOW_LUX")));
				 vrts_Json_String.hight_lux = (json_object_get_int(json_object_object_get(vrts_Json_String.lightsensor,"HIGHT_LUX")));
				 vrts_Json_String.motion = (json_object_get_int(json_object_object_get(vrts_Json_String.pir,"PIR")));
				 if((vrts_Json_String.condition == 3 || vrts_Json_String.condition == 4) && (vrts_Json_String.motion == 0)){
					 uint16_t parCondition = 258;
				 	 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 		 					parCondition, vrts_Json_String.low_lux, NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor,NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
				 	usleep(400000);
				 }
				 else if((vrts_Json_String.condition == 5 || vrts_Json_String.condition == 6) && (vrts_Json_String.motion == 0)){
					 uint16_t parCondition = 770;
				 	 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 		 					parCondition,vrts_Json_String.low_lux , NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
				 	usleep(400000);
				 }
				 else if(vrts_Json_String.motion == 0 && (vrts_Json_String.condition == 1)){
					 uint16_t parCondition = 514;
				 	 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 		 					parCondition,vrts_Json_String.low_lux , NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
				 	usleep(400000);
				 }
				 else if(vrts_Json_String.motion == 0 && vrts_Json_String.condition == 7){
					 uint16_t parCondition = 1026;
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
											parCondition,vrts_Json_String.low_lux , vrts_Json_String.hight_lux, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
				 else if((vrts_Json_String.condition == 3 || vrts_Json_String.condition == 4) && (vrts_Json_String.motion == 1)){
					 uint16_t parCondition = 257;
				 	 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 		 					parCondition, vrts_Json_String.low_lux, NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
				 	usleep(400000);
				 }
				 else if((vrts_Json_String.condition == 5 || vrts_Json_String.condition == 6) && (vrts_Json_String.motion == 1)){
					 uint16_t parCondition = 769;
				 	 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 		 					parCondition,vrts_Json_String.low_lux , NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
				 	usleep(400000);
				 }
				 else if(vrts_Json_String.motion == 1 && (vrts_Json_String.condition == 1)){
					 uint16_t parCondition = 513;
				 	 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
				 		 					parCondition,vrts_Json_String.low_lux , NULL16, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
				 	usleep(400000);
				 }
				 else if(vrts_Json_String.motion == 1 && vrts_Json_String.condition == 7){
					 uint16_t parCondition = 1025;
					 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
											parCondition,vrts_Json_String.low_lux , vrts_Json_String.hight_lux, ACTION_TIME, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 26);
					 usleep(400000);
				 }
			 }
			 else if(vrts_Json_String.type == 3){
				 vrts_Json_String.doorsensor = json_object_object_get(jobj, "DOOR_SENSOR");
				 vrts_Json_String.door_value = json_object_get_int(json_object_object_get(vrts_Json_String.doorsensor,"DOOR_VALUE"));
				 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForDoorSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8, vrts_Json_String.door_value, NULL16, \
						 NULL16, NULL16, NULL16, vrts_Json_String.sceneforsensor, NULL16, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 21);
			 }
		 }
		 else if(strcmp(vrts_Json_String.cmd,"DELSCENEFORSENSOR")==0){
			 vrts_Json_String.adr = json_object_get_int(json_object_object_get(jobj,"ADR"));
			 vrts_Json_String.sceneforsensor= json_object_get_int(json_object_object_get(jobj,"SCENEID"));
			 uint16_t parCondition = 1280;
			 Function_Vendor(HCI_CMD_GATEWAY_CMD, DelSceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8,NULL8, \
		 					parCondition, NULL16, NULL16, NULL16, vrts_Json_String.sceneforsensor, NULL16, NULL8, NULL8, NULL8, NULL8, 19);
			 usleep(400000);
		 }
		 else if(strcmp(vrts_Json_String.cmd,"DELHC") ==0 ){
			 ControlMessage(3, reset_GW);
			 slog_print(SLOG_INFO, 1, "RESET_GATEWAY...");
			 sleep(5);
			 FILE *file;
			 char filename[]= "device_key.txt";
			 if((file = fopen(filename,"r"))){
				 remove(filename);
			 }
			 slog_print(SLOG_INFO, 1, "RESET_DONE");
			 GWIF_Init();
		 }
	 }
	 if(strcmp(key,"UPDATE")==0){
		 flagDefineCmd = update_enum;
		 FunctionPer(HCI_CMD_GATEWAY_CMD, UpdateLight_typedef, vrts_Json_String.adr, NULL8, NULL8, NULL16, NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, NULL16, 12);
		 usleep(400000);
	 }
	 if((strcmp(key,"STT")==0)){
		Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForSensor_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8,\
				NULL8, vrts_Json_String.stt, vrts_Json_String.condition, vrts_Json_String.low_lux, vrts_Json_String.hight_lux,\
				vrts_Json_String.action, vrts_Json_String.sceneID, vrts_Json_String.sceneID, vrts_Json_String.srgbID, NULL8, NULL8, NULL8,31);
		usleep(400000);
	 }
	 if((strcmp(key,"SETSCENERGB")==0)){
		 Function_Vendor(HCI_CMD_GATEWAY_CMD, SceneForRGB_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8, NULL8, NULL8, \
				 NULL16, NULL16, NULL16, NULL16, NULL16, vrts_Json_String.appID, vrts_Json_String.srgbID, NULL8, NULL8, NULL8, 23);
		 usleep(400000);
	 }
	 if((strcmp(key,"CALLSCENERGB")==0)){
		Function_Vendor(HCI_CMD_GATEWAY_CMD, CallSceneRgb_vendor_typedef, NULL16, NULL16, NULL8,NULL8, NULL8, NULL16,\
				NULL16, NULL16,NULL16, NULL16, vrts_Json_String.callsceneRGB, NULL8, NULL8, NULL8, NULL8,23);
		usleep(400000);
	 }
	 if((strcmp(key,"CALLMODERGB")==0)){
		Function_Vendor(HCI_CMD_GATEWAY_CMD, CallModeRgb_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8,NULL8, NULL8, NULL16,\
				NULL16, NULL16,NULL16, NULL16, NULL16, vrts_Json_String.callmodeRGB, NULL8, NULL8, NULL8,23);
		usleep(400000);
	 }
	 if((strcmp(key,"DELSCENERGB")==0)){
		Function_Vendor(HCI_CMD_GATEWAY_CMD, DelSceneRgb_vendor_typedef, vrts_Json_String.adr, NULL16, NULL8,NULL8, NULL8, NULL16,\
				NULL16, NULL16,NULL16, NULL16, vrts_Json_String.delsceneRGB, NULL8, NULL8, NULL8, NULL8,23);
		usleep(400000);
	 }
	 if((strcmp(key,"SAVEGATEWAY")==0)){
		 Function_Vendor(HCI_CMD_GATEWAY_CMD, SaveGateway_vendor_typedef, vrts_Json_String.adr, NULL16,\
				 NULL8, NULL8, NULL8, NULL16, NULL16, NULL16, NULL16, NULL16, NULL16, NULL8, NULL8, NULL8, NULL8,17);
		 usleep(400000);
	 }
	 if((strcmp(key,"TYPEDEVICESCAN")==0)){
		 Function_Vendor(HCI_CMD_GATEWAY_CMD, AskTypeDevice_vendor_typedef, vrts_Json_String.adr, NULL16,\
				 NULL8, NULL8, NULL8, NULL16, NULL16, NULL16, NULL16, NULL16, NULL16, NULL8, NULL8, NULL8, NULL8,17);
		 usleep(400000);
	 }
	 if((strcmp(key,"SETTYPEDEVICE")==0)){
		 Function_Vendor(HCI_CMD_GATEWAY_CMD, SetTypeDevice_vendor_typedef, vrts_Json_String.adr, NULL16, \
				 NULL8, NULL8, NULL8, NULL16, NULL16, NULL16, NULL16, NULL16, NULL16, NULL8,\
				 vrts_Json_String.type, vrts_Json_String.attrubute, vrts_Json_String.application,28);
		 usleep(400000);
	 }
}
void json_value(json_object *jobj)
{
	enum json_type type;
	type = json_object_get_type(jobj);
	switch (type)
	{
		case json_type_int:
			adr_dv[i] = json_object_get_int(jobj);
			break;
	}
}

int json_parse_array( json_object *jobj, char *key)
{
	enum json_type type;

	json_object *jarray = jobj;
	if(key)
	{
		jarray = json_object_object_get(jobj, key);
	}

	arraylen = json_object_array_length(jarray);
	//printf("Array Length: %d\n",arraylen);
	json_object * jvalue;

	for (i=0; i< arraylen; i++)
	{
		jvalue = json_object_array_get_idx(jarray, i);
		type = json_object_get_type(jvalue);
		if (type != json_type_object)
		{
			json_value(jvalue);
		}
		else if(type == json_type_object)
		{
			//vrts_Json_String.id        		= (json_object_get_int(json_object_object_get(jvalue,"id")));
//			vrts_Json_String.cct 			= (json_object_get_int(json_object_object_get(jvalue,"CCT")));
//			vrts_Json_String.dim 			= (json_object_get_int(json_object_object_get(jvalue,"DIM")));
//			FunctionPer(HCI_CMD_GATEWAY_CMD, CCT_Set_typedef, vrts_Json_String.id+1, NULL8, NULL8, NULL16,Percent2ParamCCT(vrts_Json_String.cct), NULL16, NULL16,NULL16, NULL16, NULL16, 17);sleep(1);
//			FunctionPer(HCI_CMD_GATEWAY_CMD, Lightness_Set_typedef, vrts_Json_String.id, NULL8, NULL8, Percent2ParamDIM(vrts_Json_String.dim), NULL16, NULL16, NULL16,NULL16, NULL16, NULL16, 14);sleep(1);
			//FunctionPer(HCI_CMD_GATEWAY_CMD, AddSence_typedef, vrts_Json_String.id, NULL8, NULL8, NULL16, NULL16, scene_id, NULL16,NULL16, NULL16, NULL16, 14);//sleep(1);
		}
	}
	return 1;
}

/*
 * TODO: cần xử lý kiểu object(object)
 */
void Json_Parse(json_object * jobj)
{
	enum json_type type;
		 json_object_object_foreach(jobj, key, val) {
			 type= json_object_get_type(val);
			 switch(type){
							 case json_type_int:
								 vrts_Json_String.adr        	= (json_object_get_int(json_object_object_get(jobj,"ADR")));
								 if(strcmp(key,"TIME")==0){
									 vrts_Json_String.minutes 		= (json_object_get_int(json_object_object_get(jobj,"TIME")));
									 time_check = true;
									 puts("have transition");
								 }
								 vrts_Json_String.onoff 		= (json_object_get_int(json_object_object_get(jobj,"ONOFF")));
								 vrts_Json_String.cct 			= (json_object_get_int(json_object_object_get(jobj,"CCT")));
								 vrts_Json_String.dim 			= (json_object_get_int(json_object_object_get(jobj,"DIM")));
								 vrts_Json_String.hours 		= (json_object_get_int(json_object_object_get(jobj,"HOURS")));
								 vrts_Json_String.seconds		= (json_object_get_int(json_object_object_get(jobj,"SECONDS")));
								 vrts_Json_String.addgroup 		= (json_object_get_int(json_object_object_get(jobj,"ADDGROUP")));
								 vrts_Json_String.delgroup 		= (json_object_get_int(json_object_object_get(jobj,"DELGROUP")));
								 vrts_Json_String.addscene 		= (json_object_get_int(json_object_object_get(jobj,"ADDSCENE")));
								 vrts_Json_String.callscene 	= (json_object_get_int(json_object_object_get(jobj,"CALLSCENE")));
								 vrts_Json_String.delscene 		= (json_object_get_int(json_object_object_get(jobj,"DELSCENE")));
								 vrts_Json_String.hue 			= (json_object_get_int(json_object_object_get(jobj,"HUE")));
								 vrts_Json_String.saturation	= (json_object_get_int(json_object_object_get(jobj,"SATURATION")));
								 vrts_Json_String.lightness 	= (json_object_get_int(json_object_object_get(jobj,"LIGHTNESS")));
								 vrts_Json_String.resetnode 	= (json_object_get_int(json_object_object_get(jobj,"RESET")));
//								 vrts_Json_String.start 		= (json_object_get_int(json_object_object_get(jobj,"START")));
//								 vrts_Json_String.stop 			= (json_object_get_int(json_object_object_get(jobj,"STOP")));
								 vrts_Json_String.update 		= (json_object_get_int(json_object_object_get(jobj,"UPDATE")));
								 vrts_Json_String.header        = (json_object_get_int(json_object_object_get(jobj,"HEADER")));
								 vrts_Json_String.stt           = (json_object_get_int(json_object_object_get(jobj,"STT")));
								 vrts_Json_String.action        = (json_object_get_int(json_object_object_get(jobj,"ACTION")));
								 vrts_Json_String.condition     = (json_object_get_int(json_object_object_get(jobj,"CONDITION")));
								 vrts_Json_String.low_lux       = (json_object_get_int(json_object_object_get(jobj,"LOW_LUX")));
								 vrts_Json_String.hight_lux     = (json_object_get_int(json_object_object_get(jobj,"HIGHT_LUX")));
								 vrts_Json_String.buttonid      = (json_object_get_int(json_object_object_get(jobj,"BUTTONID")));
								 vrts_Json_String.modeid        = (json_object_get_int(json_object_object_get(jobj,"MODEID")));
								 vrts_Json_String.srgbID        = (json_object_get_int(json_object_object_get(jobj,"SRGBID")));
								 vrts_Json_String.appID         = (json_object_get_int(json_object_object_get(jobj,"APPID")));
								 vrts_Json_String.sceneID       = (json_object_get_int(json_object_object_get(jobj,"SCENEID")));

								 vrts_Json_String.sceneforremote= (json_object_get_int(json_object_object_get(jobj,"SCENEFORREMOTE")));
								 vrts_Json_String.sceneforsensor= (json_object_get_int(json_object_object_get(jobj,"SCENEFORSENSOR")));

								 vrts_Json_String.setsceneRGB   = (json_object_get_int(json_object_object_get(jobj,"SETSCENERGB")));
								 vrts_Json_String.callsceneRGB  = (json_object_get_int(json_object_object_get(jobj,"CALLSCENERGB")));
								 vrts_Json_String.callmodeRGB  = (json_object_get_int(json_object_object_get(jobj,"CALLMODERGB")));
								 vrts_Json_String.delsceneRGB  = (json_object_get_int(json_object_object_get(jobj,"DELSCENERGB")));

								 vrts_Json_String.savegateway   = (json_object_get_int(json_object_object_get(jobj,"SAVEGATEWAY")));
								 vrts_Json_String.settypedevice = (json_object_get_int(json_object_object_get(jobj,"SETTYPEDEVICE")));
								 vrts_Json_String.typedevicescan = (json_object_get_int(json_object_object_get(jobj,"TYPEDEVICESCAN")));
								 vrts_Json_String.type      = (json_object_get_int(json_object_object_get(jobj,"TYPE")));
								 vrts_Json_String.attrubute       = (json_object_get_int(json_object_object_get(jobj,"ATTRUBUTE")));
								 vrts_Json_String.application   = (json_object_get_int(json_object_object_get(jobj,"APPLICATION")));
								 JsonControl(jobj,key);
								 break;
							 case json_type_string:
								 vrts_Json_String.cmd 	= (json_object_get_string(json_object_object_get(jobj,"CMD")));
								 JsonControl(jobj, key);
								 break;
							 case json_type_array:
									//printf("type: json_type_array\n");
									json_parse_array(jobj, key);
									break;
			 }
			 //JsonControl(key);
		 }
}

/*
 * creat a object json with a key and a value (type value: int, string, object)
 */
void add_component_to_obj(json_object *j, void* com){
    json_component *dt = (json_component*)com;
    switch (dt->type){
    case json_type_int:
    	json_object_object_add(j, dt->key, json_object_new_int(dt->value));
    	break;
    case json_type_string:
    	json_object_object_add(j, dt->key, json_object_new_string(dt->value));
    	break;
    case json_type_array:
    	//json_object_object_add(j, dt->key, json_object_new_array(dt->value));
    	break;
    case json_type_boolean:
    	json_object_object_add(j, dt->key, json_object_new_boolean(dt->value));
    	break;
    case json_type_object:
    	json_object_object_add(j, dt->key,(dt->value));
    	break;
    }
    return;
}
/*
 * creat object json, include json_component
 * Check mqtt push json
 */
json_object* create_json_obj_from(void (*modelFunc)(json_object*, void*), int num_of, send_mqtt mqtt,...){
    va_list args_list;
    json_object *jobj = json_object_new_object();
    typedef void *com;

    va_start(args_list, num_of);
    int i;
    for(i = 0; i< num_of; i++){
        add_component_to_obj(jobj, va_arg(args_list, com));
    }
    va_end(args_list);
    if(mqtt == mqtt_push){
		char *str = json_object_to_json_string(jobj);
		mosquitto_publish(mosq, NULL, TP_STATUS, strlen(str), str, qos, 0);
		slog_info("(mqtt)Message_send:%s",str);
    }
    return jobj;

}
