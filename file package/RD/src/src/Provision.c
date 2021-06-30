/*
 *Provision.c
 */

#include "Provision.h"
#include "slog.h"
#include "Light.h"
#include "JsonProcess.h"
#include "MQTT.h"
#include "Linkerlist.h"

pthread_t tmp;
pthread_t vrts_System_Gpio;

unsigned int Timeout_CheckDataBuffer = 0;
unsigned char scanNotFoundDev =0 ;
unsigned int adr_heartbeat;

uint8_t OUTMESSAGE_ScanStop[3]     = {0xE9, 0xFF, 0x01};
uint8_t OUTMESSAGE_ScanStart[3]    = {0xE9, 0xFF, 0x00};
uint8_t OUTMESSAGE_MACSelect[9]    = {0xE9, 0xFF, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t OUTMESSAGE_GetPro[3] 	   = {0xE9, 0xFF, 0x0C};
uint8_t OUTMESSAGE_Provision[28]   = {0};
uint8_t OUTMESSAGE_BindingALl[22]  = {0xe9, 0xff, 0x0b, 0x00, 0x00, 0x00, 0x60, 0x96, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76, 0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48};

uint8_t setpro_internal[]       =   {0xe9, 0xff, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44, 0x01, 0x00};
uint8_t admit_pro_internal[]    =   {0xe9, 0xff, 0x0d, 0x01, 0x00, 0xff, 0xfb, 0xeb, 0xbf, 0xea, 0x06, 0x09, 0x00, 0x52, 0x90, 0x49, 0xf1, 0xf1, 0xbb, 0xe9, 0xeb};// trả về unicast tiếp theo của con đèn cần thêm vào
uint8_t reset_GW[]				=	{0xe9, 0xff, 0x02};
//uint8_t device_key1[50];

bool flag_selectmac     	= false;
bool flag_getpro_info   	= false;
bool flag_getpro_element	= false;
bool flag_provision     	= false;
bool flag_mac           	= true;
bool flag_check_select_mac  = false;
bool flag_done          	= true;
bool flag_setpro            = false;
bool flag_admitpro          = false;
bool flag_checkadmitpro     = true;

bool flag_set_type          = false;
bool flag_checkHB           = false;
bool flag_checkSaveGW		= false;
bool flag_checkTypeDEV 		= false;

uint8_t uuid_json[40]= {0};
uint8_t deviceid_json[40]= {0};
uint8_t netkey_json[40]= {0};
uint8_t appkey_json[40]= {0};

uint16_t unicastId;
void ConvertUuid(uint8_t *uuid, uint8_t *uuid_string)
{
	uint8_t temp[3];
	uint8_t uuid_convert8[12]={0};
	uint8_t uuid_convert4_1[9]={0};
	uint8_t uuid_convert4_2[9]={0};
	uint8_t uuid_convert4_3[9]={0};
	uint8_t uuid_convert12[20]={0};
	uint8_t *gach = "-";
	int i;
	for(i=0; i<4 ; i++){
        if(uuid[i]<= 0x0f){
            sprintf(temp,"0%x",uuid[i]);
        }
        else{
		    sprintf(temp,"%2x",uuid[i]);
        }
        strcat(uuid_convert8,temp);
	}
    strcat(uuid_convert8,gach);
    strcat(uuid_string,uuid_convert8);
 	for(i=4; i<6 ; i++){
        if(uuid[i]<=0x0f){
            sprintf(temp,"0%x",uuid[i]);
        }
        else{
		    sprintf(temp,"%2x",uuid[i]);
        }
        strcat(uuid_convert4_1,temp);
	}
    strcat(uuid_convert4_1,gach);
    strcat(uuid_string,uuid_convert4_1);
    for(i=6; i<8 ; i++){
        if(uuid[i]<=0x0f){
            sprintf(temp,"0%x",uuid[i]);
        }
        else{
		    sprintf(temp,"%2x",uuid[i]);
        }
        strcat(uuid_convert4_2,temp);
	}
    strcat(uuid_convert4_2,gach);
    strcat(uuid_string,uuid_convert4_2);
    for(i=8; i<10 ; i++){
        if(uuid[i]<=0x0f){
            sprintf(temp,"0%x",uuid[i]);
        }
        else{
		    sprintf(temp,"%2x",uuid[i]);
        }
        strcat(uuid_convert4_3,temp);
	}
    strcat(uuid_convert4_3,gach);
    strcat(uuid_string,uuid_convert4_3);
    for(i=10; i<16 ; i++){
        if(uuid[i]<=0x0f){
            sprintf(temp,"0%x",uuid[i]);
        }
        else{
		    sprintf(temp,"%2x",uuid[i]);
        }
        strcat(uuid_convert12,temp);
	}
    strcat(uuid_string,uuid_convert12);
 }
void ControlMessage(uint16_t lengthmessage,uint8_t *message)
{
	unicastId = message[8] | (message[9]<<8);
	pthread_mutex_lock(&vrpth_SHAREMESS_Send2GatewayLock);
	vrb_SHAREMESS_Send2GatewayAvailabe = true;
	vrui_SHAREMESS_Send2GatewayLength = lengthmessage;
	memcpy(vrsc_SHAREMESS_Send2GatewayMessage, message, vrui_SHAREMESS_Send2GatewayLength);
	pthread_mutex_unlock(&vrpth_SHAREMESS_Send2GatewayLock);
	uint8_t tempDataLog[200]="";
	uint8_t temp[4];
	int i;
	for(i=0;i< lengthmessage;i++){
		sprintf(temp,"%x ",message[i]);
		strcat(tempDataLog,temp);
	}
	slog_info("(cmd)%s",tempDataLog);
}
void *ProvisionThread (void *argv )
{
	tmp = pthread_self();
	while(MODE_PROVISION){
		if((flag_done == true) || (Timeout_CheckDataBuffer == 32000))
		{
			scanNotFoundDev++;
			if(scanNotFoundDev==3)
			{
				scanNotFoundDev = 0;
				MODE_PROVISION=false;
				ControlMessage(3, OUTMESSAGE_ScanStop);
				slog_print(SLOG_INFO, 1, "<provision>Provision stop");

				json_component cmd = {"CMD","STOP",json_type_string};
				create_json_obj_from(add_component_to_obj, 1, mqtt_push, &cmd);

				flag_selectmac     = false;
				flag_getpro_info   = false;
				flag_getpro_element= false;
				flag_provision     = false;
				flag_mac           = true;
				flag_check_select_mac  = false;
				flag_done          = true;
				flag_setpro  = false;
				flag_admitpro = false;
				flag_checkadmitpro = true;
				flag_set_type = false;
			}
			else
			{
				flag_done = false;
				Timeout_CheckDataBuffer=0;
				//usleep(500000);
				flag_selectmac     = false;
				flag_getpro_info   = false;
				flag_getpro_element= false;
				flag_provision     = false;
				flag_mac           = true;
				flag_check_select_mac  = false;
				flag_setpro  = false;
				flag_admitpro = false;
				flag_checkadmitpro = true;
				flag_set_type = false;
				ControlMessage(3, OUTMESSAGE_ScanStart);
				slog_print(SLOG_INFO, 1, "<provision>SCAN");
				flag_check_select_mac= true;
			}
		}

		if((flag_selectmac==true) && (flag_mac==true))
		{
			flag_selectmac=false;
			flag_mac=false;
			ControlMessage(9, OUTMESSAGE_MACSelect);
			slog_print(SLOG_INFO, 1, "<provision>SELECTMAC");
			usleep(100000);
			ControlMessage(3, OUTMESSAGE_GetPro);
			slog_print(SLOG_INFO, 1, "<provision>GETPRO");
			flag_checkadmitpro = false;
			//sleep(3);
		}
		if(flag_setpro == true)
		{
			flag_setpro = false;
			srand((int)time(0));
			int random1,random2;
			int i;
			for(i=0;i<16;i++)
			{
				random1=rand()%256;
				random2=rand()%256;
				setpro_internal[i+3]=random1;
				admit_pro_internal[i+5]=random2;
			}
			//srand((int)time(0));
			slog_print(SLOG_INFO, 1, "<provision>SETPRO....");
			ControlMessage(28, setpro_internal);
			slog_print(SLOG_INFO, 1, "<provision>ADMITPRO...");
			ControlMessage(21, admit_pro_internal);
			/*add device key to file*/
			sprintf(deviceid_json,"%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",admit_pro_internal[5],admit_pro_internal[6],admit_pro_internal[7],\
					admit_pro_internal[8],admit_pro_internal[9],admit_pro_internal[10],admit_pro_internal[11],admit_pro_internal[12],admit_pro_internal[13],\
					admit_pro_internal[14],admit_pro_internal[15],admit_pro_internal[16],admit_pro_internal[17], admit_pro_internal[18],\
					admit_pro_internal[19],admit_pro_internal[20]);
//			uint8_t device_key[16];
//			for(i=0;i<16;i++){
//				device_key[i]=admit_pro_internal[i+5];
//			}
//			ConvertUuid(device_key, deviceid_json);
			FILE *file=fopen("/root/device_key.txt","w");
			   if(file == NULL)
			   {
			      printf("Error!");
			      exit(1);
			   }
			   fprintf(file,"%s",deviceid_json);
			   fclose(file);
			/**/
			//usleep(100000);
			flag_checkadmitpro = true;
		}
		if(flag_admitpro == true && flag_checkadmitpro== true)
		{
			flag_admitpro = false;
			flag_checkadmitpro = false;
			slog_print(SLOG_INFO, 1, "<provision>GETPRO");
			ControlMessage(3, OUTMESSAGE_GetPro);
		}
		if((flag_getpro_info == true) && (flag_getpro_element == true))
		{
			flag_getpro_info = false;
			flag_getpro_element = false;
			ControlMessage(28, OUTMESSAGE_Provision);
			slog_print(SLOG_INFO, 1, "<provision>PROVISION");
		}
		if(flag_provision == true)
		{
			flag_provision = false;
			//sleep(1);
//			srand((int)time(0));
//			int random;
//			int i;
//			for(i=0;i<16;i++)
//			{
//				random=rand()%256;
//				OUTMESSAGE_BindingALl[i+6]=random;
//			}
			ControlMessage(22, OUTMESSAGE_BindingALl);
			slog_print(SLOG_INFO, 1, "<provision>BINDING ALL");
			flag_set_type = false;
		}
		if(flag_set_type == true)
		{
			flag_set_type = false;
			HeartBeat(HCI_CMD_GATEWAY_CMD, adr_heartbeat, 1, 255, 11, 5, 7, 21);
			flag_checkHB = false;
			//usleep(500000);
		}
		if(flag_checkHB){
			flag_checkHB = false;
			puts("save gw");
			 Function_Vendor(HCI_CMD_GATEWAY_CMD, SaveGateway_vendor_typedef, adr_heartbeat, NULL16,\
					 NULL8, NULL8, NULL8, NULL16, NULL16, NULL16, NULL16, NULL16, NULL16, NULL8, NULL8, NULL8, NULL8,17);
			 flag_checkSaveGW = false;
			 //usleep(500000);
		}
		if(flag_checkSaveGW){
			flag_checkSaveGW = false;
			Function_Vendor(HCI_CMD_GATEWAY_CMD, AskTypeDevice_vendor_typedef, adr_heartbeat, NULL16,\
					 NULL8, NULL8, NULL8, NULL16, NULL16, NULL16, NULL16, NULL16, NULL16, NULL8, NULL8, NULL8, NULL8,17);
			flag_checkTypeDEV = false;
			 //usleep(500000);
		}
		if(flag_checkTypeDEV){
			flag_checkTypeDEV = false;
			flag_done=true;
			flag_mac=true;
		}
	}
	return NULL;
}
