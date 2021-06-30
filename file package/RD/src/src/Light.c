/*
 * Light.c
 */

#include "Light.h"
#include "GateInterface.h"
#include "Provision.h"
#include "SensorLight.h"
#include "slog.h"



char *pHeaderCmd= "cmd";
cmdcontrol_t vrts_CMD_STRUCTURE;
cmdcontrol_vendor vrts_CMD_STRUCTURE_VENDOR;
uint8_t parRetry_cnt = 0x02;
uint8_t parRsp_Max = 0x01;
uint8_t parFuture = 0;

bool flag_saveGW 			= false;
bool flag_typeDEV			= false;
bool flag_heartbeat			= false;

void ResetNode(uint16_t uniAdrReset)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrReset & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrReset>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = NODE_RESET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1]= (NODE_RESET >> 8) & 0xFF;
}
void Lightness_Get(uint16_t adrLightnessGet)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = adrLightnessGet & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (adrLightnessGet>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = LIGHTNESS_GET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHTNESS_GET>>8) & 0xFF;
}
void Lightness_Set(uint16_t uniAdrSetDim, uint16_t valueLightness, uint16_t transition)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrSetDim & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrSetDim>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = LIGHTNESS_SET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHTNESS_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = valueLightness & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (valueLightness>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[2] = 0;
	vrts_CMD_STRUCTURE.para[3] = transition & 0xFF;
	vrts_CMD_STRUCTURE.para[4] = (transition>>8) & 0xFF;
}
void CCT_Get(uint16_t adrCCTGet)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = adrCCTGet & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (adrCCTGet>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = LIGHT_CTL_TEMP_GET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHT_CTL_TEMP_GET>>8) & 0xFF;
}
void CCT_Set(uint16_t uniAdrSetCCT, uint16_t valueCCT,uint8_t transition)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrSetCCT & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrSetCCT>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = LIGHT_CTL_TEMP_SET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHT_CTL_TEMP_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = valueCCT & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (valueCCT>>8) & 0xFF;
	char i;
	for (i=2;i<=4;i++)
	{
		vrts_CMD_STRUCTURE.para[i] = 0x00;
	}
	vrts_CMD_STRUCTURE.para[5] = transition & 0xFF;
	vrts_CMD_STRUCTURE.para[6] = (transition>>8) & 0xFF;
}
void AddGroup(uint16_t uniAdrAddGroup,uint16_t adrGroup)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrAddGroup & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrAddGroup>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] =  CFG_MODEL_SUB_ADD & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (CFG_MODEL_SUB_ADD>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = uniAdrAddGroup & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (uniAdrAddGroup>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[2] = adrGroup & 0xFF;
	vrts_CMD_STRUCTURE.para[3] = (adrGroup>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[4] = 0x00;
	vrts_CMD_STRUCTURE.para[5] = 0x10;
}
void DelGroup(uint16_t uniAdrAddGroup,uint8_t adrGroup)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrAddGroup & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrAddGroup>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] =  CFG_MODEL_SUB_DEL & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (CFG_MODEL_SUB_DEL>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = uniAdrAddGroup & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (uniAdrAddGroup>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[2] = adrGroup;
	vrts_CMD_STRUCTURE.para[3] = 0xC0;
	vrts_CMD_STRUCTURE.para[4] = 0x00;
	vrts_CMD_STRUCTURE.para[5] = 0x10;
}
void AddSence(uint16_t uniAdrSence, uint16_t senceID)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrSence & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrSence>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = SCENE_STORE & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (SCENE_STORE>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = senceID & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (senceID>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[2]= vrts_CMD_STRUCTURE.para[3]= vrts_CMD_STRUCTURE.para[4]= 0;
}
void CallSence(uint16_t senceId, uint16_t transition)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = SCENE_RECALL & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (SCENE_RECALL>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = senceId & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (senceId>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[2] = 0;
	vrts_CMD_STRUCTURE.para[3] = transition & 0xFF;
	vrts_CMD_STRUCTURE.para[4] = (transition>>8) & 0xFF;
}
void DelSence(uint16_t uniAdrDelSence, uint16_t senceId)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrDelSence & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrDelSence>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = SCENE_DEL & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (SCENE_DEL>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = senceId & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (senceId>>8) & 0xFF;
}
void ControlOnOff(uint16_t uniAdrControlOnOff,uint8_t statuOnOff,uint16_t transition)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrControlOnOff & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrControlOnOff>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] =  LIGHTOPCODE_ONOFF  & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHTOPCODE_ONOFF >>8) & 0xFF;
	if(statuOnOff == 1){
		vrts_CMD_STRUCTURE.para[0] = statuOnOff;
//		flag_blink = false;
	}
	if(statuOnOff == 0){
		vrts_CMD_STRUCTURE.para[0] = statuOnOff;
//		flag_blink = true;
	}
	vrts_CMD_STRUCTURE.para[1] = 0;
	vrts_CMD_STRUCTURE.para[2] = transition & 0xFF;
	vrts_CMD_STRUCTURE.para[3] = (transition>>8) & 0xFF;
}
void HSL_Get(uint16_t adrHSLGet)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = adrHSLGet & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (adrHSLGet>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = LIGHTOPCODE_SELECT & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHTOPCODE_SELECT>>8) & 0xFF;
}
void HSL_Set(uint16_t uniAdrHSL, uint16_t h, uint16_t s, uint16_t l, uint16_t transition)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrHSL & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrHSL >>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = LIGHT_HSL_SET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHT_HSL_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = l & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = (l>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[2] = h & 0xFF;
	vrts_CMD_STRUCTURE.para[3] = (h>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[4] = s & 0xFF;
	vrts_CMD_STRUCTURE.para[5] = (s>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[6] = 0;
	vrts_CMD_STRUCTURE.para[7] = transition & 0xFF;
	vrts_CMD_STRUCTURE.para[8] = (transition>>8) & 0xFF;
}
/*
 * TODO: Update status of light
 */
void UpdateLight()
{
	vrts_CMD_STRUCTURE.adr_dst[0] = 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = LIGHTOPCODE_UPDATE & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (LIGHTOPCODE_UPDATE>>8)& 0xFF;
}

/*
 * TODO: Set timepoll for lightsensor
 */
void SetTimePoll(uint16_t uniAdrSensor, uint16_t timePoll)
{
	vrts_CMD_STRUCTURE.adr_dst[0] = uniAdrSensor & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (uniAdrSensor>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] =  SENSOR_DESCRIP_GET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (SENSOR_DESCRIP_GET>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = (timePoll>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[1] = timePoll & 0xFF;
	vrts_CMD_STRUCTURE.para[2] = parFuture;
}

/*
 * Ham ControlMessage de nhan chuoi can truyen
 * TODO: gan struct vao chuoi xong chuyen uart
 *
 */
void FunctionPer(uint16_t cmd,\
				functionTypeDef Func,\
				uint16_t unicastAdr,\
				uint16_t adrGroup,\
				uint8_t parStatusOnOff,\
				uint16_t parLightness,\
				uint16_t parCCT,\
				uint16_t parSenceId,\
				uint16_t parTimePoll,\
				uint16_t parL,
				uint16_t parH,
				uint16_t parS,
				uint16_t transition,
				uint8_t cmdLength)
{
	vrts_CMD_STRUCTURE.HCI_CMD_GATEWAY[0] = cmd & 0xFF;
	vrts_CMD_STRUCTURE.HCI_CMD_GATEWAY[1] = (cmd>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode00[0] = 0;
	vrts_CMD_STRUCTURE.opCode00[1] = 0;
	vrts_CMD_STRUCTURE.opCode00[2] = 0;
	vrts_CMD_STRUCTURE.opCode00[3] = 0;
	vrts_CMD_STRUCTURE.retry_cnt = parRetry_cnt;
	vrts_CMD_STRUCTURE.rsp_max = 0;
	if(Func == ResetNode_typedef){
		ResetNode(unicastAdr);
	}
	else if(Func == Lightness_Get_typedef){
		Lightness_Get(unicastAdr);
	}
	else if(Func == AddGroup_typedef){
		AddGroup(unicastAdr, adrGroup);
	}
	else if(Func == DelGroup_typedef){
		DelGroup(unicastAdr, adrGroup);
	}
	else if(Func == ControlOnOff_typedef){
		ControlOnOff(unicastAdr,parStatusOnOff,transition);
	}
	else if (Func == SetTimePoll_typedef){
		SetTimePoll(unicastAdr, parTimePoll);
	}
	else if (Func == CCT_Set_typedef)
	{
		CCT_Set(unicastAdr,parCCT,transition);
	}
	else if (Func == Lightness_Set_typedef){
		Lightness_Set(unicastAdr, parLightness,transition);
	}
	else if (Func == UpdateLight_typedef){
		UpdateLight();
	}
	else if(Func == AddSence_typedef){
		AddSence(unicastAdr, parSenceId);
	}
	else if(Func == DelSence_typedef){
		DelSence(unicastAdr, parSenceId);
	}
	else if(Func == CallSence_typedef){
		CallSence(parSenceId, transition);
	}
	else if(Func == HSL_Set_typedef){
		HSL_Set(unicastAdr, parH, parS, parL, transition);
	}
	else if(Func == CCT_Get_typedef){
		CCT_Get(unicastAdr);
	}
	else if(Func == HSL_Get_typedef){
		HSL_Get(unicastAdr);
	}

	uint8_t *tempDataUart;
	tempDataUart = (uint8_t *)&vrts_CMD_STRUCTURE;
	ControlMessage(cmdLength, tempDataUart);
}
void HeartBeat(uint16_t cmd, uint16_t drsHeartbeat, uint16_t srcHeartbeat, uint8_t countLog, uint8_t periodLog, uint8_t tll, uint16_t feature, uint16_t cmdLength)
{
	vrts_CMD_STRUCTURE.HCI_CMD_GATEWAY[0]= cmd & 0xFF;
	vrts_CMD_STRUCTURE.HCI_CMD_GATEWAY[1]= (cmd>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode00[0] = 0;
	vrts_CMD_STRUCTURE.opCode00[1] = 0;
	vrts_CMD_STRUCTURE.opCode00[2] = 0;
	vrts_CMD_STRUCTURE.opCode00[3] = 0;
	vrts_CMD_STRUCTURE.retry_cnt = parRetry_cnt;
	vrts_CMD_STRUCTURE.rsp_max = 0;
	vrts_CMD_STRUCTURE.adr_dst[0] = drsHeartbeat & 0xFF;
	vrts_CMD_STRUCTURE.adr_dst[1] = (drsHeartbeat>>8) & 0xFF;
	vrts_CMD_STRUCTURE.opCode[0] = HEARTBEAT_PUB_SET & 0xFF;
	vrts_CMD_STRUCTURE.opCode[1] = (HEARTBEAT_PUB_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[0] = srcHeartbeat & 0XFF;
	vrts_CMD_STRUCTURE.para[1] = (srcHeartbeat>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[2] = countLog;
	vrts_CMD_STRUCTURE.para[3] = periodLog;
	vrts_CMD_STRUCTURE.para[4] = tll;
	vrts_CMD_STRUCTURE.para[5] = feature & 0xFF;
	vrts_CMD_STRUCTURE.para[6] = (feature>>8) & 0xFF;
	vrts_CMD_STRUCTURE.para[7]= vrts_CMD_STRUCTURE.para[8]= 0;

	uint8_t *tempDataUart;
	tempDataUart = (uint8_t *)&vrts_CMD_STRUCTURE;
	ControlMessage(cmdLength, tempDataUart);
	flag_heartbeat = true;
}


/*Cmd control use opcode vendor*/
void SetSceneForRemote(uint16_t addressremote, uint8_t buttonId, uint8_t modeId, uint16_t sceneId, uint16_t appID, uint8_t SrgbID)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0]= addressremote & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1]= (addressremote>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0]= RD_OPCODE_SCENE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1]= (VENDOR_ID) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2]= (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0]= (HEADER_SCENE_REMOTE_SET) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1]= HEADER_SCENE_REMOTE_SET>>8 & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2]= buttonId;
	vrts_CMD_STRUCTURE_VENDOR.para[3]= modeId;
	vrts_CMD_STRUCTURE_VENDOR.para[4]= sceneId & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[5]= (sceneId>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[6]= appID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[7]= (appID >>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[8]= SrgbID;
	int i;
	for(i=0; i<7; i++){
		vrts_CMD_STRUCTURE.para[i+9]= 0;
	}
}
void DelSceneForRemote(uint16_t addressremote, uint8_t buttonId, uint8_t modeId){
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0]= addressremote & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1]= (addressremote>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0]= RD_OPCODE_SCENE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1]= (VENDOR_ID) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2]= (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0]= (HEADER_SCENE_REMOTE_DEL) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1]= HEADER_SCENE_REMOTE_DEL>>8 & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2]= buttonId;
	vrts_CMD_STRUCTURE_VENDOR.para[3]= modeId;
}
void SetSceneForSensor(uint16_t addressSensor, uint16_t sceneID, uint16_t condition, uint16_t low_lux,\
		uint16_t hight_lux, uint8_t srgbID)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = addressSensor & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (addressSensor>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] = RD_OPCODE_SCENE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] = (VENDOR_ID) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_SCENE_SENSOR_SET) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_SCENE_SENSOR_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = sceneID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3] = (sceneID >>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[4] = (condition>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[5] = condition & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[6] = low_lux & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[7] = (low_lux>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[8] = hight_lux & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[9] = (hight_lux>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[10] = srgbID;
}
void DelSceneForSensor(uint16_t addressSensor, uint16_t sceneID)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = addressSensor & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (addressSensor>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] = RD_OPCODE_SCENE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] = VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_SCENE_SENSOR_DEL) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_SCENE_SENSOR_DEL>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = sceneID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3] =(sceneID>>8) & 0xFF;
}
void SetSceneForDoorSensor(uint16_t addressDoorSensor, uint16_t sceneID, uint8_t status, uint8_t srgbID){
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = addressDoorSensor & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (addressDoorSensor>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] = RD_OPCODE_SCENE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] = VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0]	=	HEADER_SCENE_DOOR_SENSOR_SET & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] 	= 	(HEADER_SCENE_DOOR_SENSOR_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2]	=	sceneID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3]	= 	(sceneID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[4]	=	status & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[5]	=	srgbID & 0xFF;
}
void DelSceneForDoorSensor(uint16_t addressDoorSensor, uint16_t sceneID){
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = addressDoorSensor & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (addressDoorSensor>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] = RD_OPCODE_SCENE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] = VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_SCENE_DOOR_SENSOR_DEL) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_SCENE_DOOR_SENSOR_DEL>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = sceneID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3] =(sceneID>>8) & 0xFF;
}
void SetSceneForTempHumSensor()
{

}
void DelSceneForTempHumSensor()
{

}
void SetSceneForSwitch4()
{

}
void DelSceneForSwitch4()
{

}
void SetSceneForRGB(uint16_t pAdrRgb, uint16_t pAppID, uint8_t pSrgbID)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = pAdrRgb & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (pAdrRgb>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] =  RD_OPCODE_SCENE_SEND;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] =  VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_SCENE_SET) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_SCENE_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = pAppID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3]=(pAppID>>8)& 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[4] = pSrgbID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[5] = vrts_CMD_STRUCTURE_VENDOR.para[6]=vrts_CMD_STRUCTURE_VENDOR.para[7]=0x00;
}
void CallSceneRgb(uint16_t appID)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] =  RD_OPCODE_SCENE_SEND;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] =  VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_SCENE_CALL_SCENE_RGB) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_SCENE_CALL_SCENE_RGB>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = appID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3]=(appID>>8)& 0xFF;
	int i;
	for(i=0;i<4;i++){
		vrts_CMD_STRUCTURE_VENDOR.para[i+4]= 0x00;
	}
}
void CallModeRgb(uint16_t adrCallModeRgb, uint8_t SrgbID)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = adrCallModeRgb & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (adrCallModeRgb>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] =  RD_OPCODE_SCENE_SEND;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] =  VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_SCENE_CALL_MODE) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_SCENE_CALL_MODE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = vrts_CMD_STRUCTURE_VENDOR.para[3] = 0x00;
	vrts_CMD_STRUCTURE_VENDOR.para[4] = SrgbID;
	int i;
	for(i=0;i<3;i++){
		vrts_CMD_STRUCTURE_VENDOR.para[i+5]= 0x00;
	}
}
void DelSceneRgb(uint16_t adrDelSceneRgb, uint16_t appID)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = adrDelSceneRgb & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (adrDelSceneRgb>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] =  RD_OPCODE_SCENE_SEND;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] =  VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_SCENE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_SCENE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_SCENE_DEL) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_SCENE_DEL>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = appID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3] = (appID>>8) & 0xFF;
	int i;
	for(i=0;i<4;i++){
		vrts_CMD_STRUCTURE_VENDOR.para[i+4]= 0x00;
	}
}


void AskTypeDevice(uint16_t adr)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = adr & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (adr>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] = RD_OPCODE_TYPE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] = VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_TYPE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_TYPE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_TYPE_ASK) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_TYPE_ASK>>8) & 0xFF;
	int i;
	for(i=0;i<11;i++){
		vrts_CMD_STRUCTURE_VENDOR.para[i+2] = 0x00;
	}
}
void SetTypeDevice(uint16_t adrSetTypeDevice,uint8_t type, uint8_t attrubute, uint8_t application)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = adrSetTypeDevice & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (adrSetTypeDevice>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] = RD_OPCODE_TYPE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] = VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_TYPE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_TYPE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_TYPE_SET) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_TYPE_SET>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[2] = type & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[3] = attrubute & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[4] = application & 0xFF;
	int i;
	for(i=0;i<8;i++){
		vrts_CMD_STRUCTURE_VENDOR.para[i+5] = 0x00;
	}
}
void SaveGateway(uint16_t adrSaveGateway)
{
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[0] = adrSaveGateway & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.adr_dst[1] = (adrSaveGateway>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[0] = RD_OPCODE_TYPE_SEND & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[1] = VENDOR_ID & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode[2] = (VENDOR_ID>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[0] = STATUS_CMD_TYPE & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.status_cmd[1] = (STATUS_CMD_TYPE>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[0] = (HEADER_TYPE_SAVEGW) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.para[1] = (HEADER_TYPE_SAVEGW>>8) & 0xFF;
	int i;
	for(i=0;i<11;i++){
		vrts_CMD_STRUCTURE_VENDOR.para[i+2] = 0x00;
	}
}
void Function_Vendor(uint16_t cmd,\
		functionTypeDef Func_vendor,\
		uint16_t adr,\
		uint16_t header,\
		uint8_t buttonID,\
		uint8_t modeID,\
		uint8_t stt,\
		uint16_t condition,\
		uint16_t low_lux,\
		uint16_t hight_lux,\
		uint16_t action,\
		uint16_t sceneID,\
		uint16_t appID,\
		uint8_t srgbID,\
		uint8_t type,\
		uint8_t attrubute,\
		uint8_t application,\
		uint16_t cmdLength
		)
{
	vrts_CMD_STRUCTURE_VENDOR.HCI_CMD_GATEWAY[0]= cmd & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.HCI_CMD_GATEWAY[1]= (cmd>>8) & 0xFF;
	vrts_CMD_STRUCTURE_VENDOR.opCode00[0] = vrts_CMD_STRUCTURE_VENDOR.opCode00[1]= \
			vrts_CMD_STRUCTURE_VENDOR.opCode00[2]= vrts_CMD_STRUCTURE_VENDOR.opCode00[3]=0;
	vrts_CMD_STRUCTURE_VENDOR.retry_cnt = parRetry_cnt;
	vrts_CMD_STRUCTURE_VENDOR.rsp_max = 0;
	if(Func_vendor == SceneForRemote_vendor_typedef){
		SetSceneForRemote(adr, buttonID, modeID, sceneID, appID, srgbID);
	}
	else if(Func_vendor == DelSceneForRemote_vendor_typedef){
		DelSceneForRemote(adr, buttonID, modeID);
	}
	else if(Func_vendor == SceneForSensor_vendor_typedef){
		SetSceneForSensor(adr,sceneID, condition, low_lux, hight_lux, srgbID);
	}
	else if(Func_vendor == DelSceneForSensor_vendor_typedef){
		DelSceneForSensor(adr, sceneID);
	}
	else if(Func_vendor == SceneForDoorSensor_vendor_typedef){
		SetSceneForDoorSensor(adr, sceneID, stt, srgbID);
	}
	else if(Func_vendor == DelSceneForDoorSensor_vendor_typedef){
		DelSceneForDoorSensor(adr, sceneID);
	}
	else if(Func_vendor == SceneForRGB_vendor_typedef){
		SetSceneForRGB(adr, appID, srgbID);
	}
	else if(Func_vendor == CallSceneRgb_vendor_typedef){
		CallSceneRgb(appID);
	}
	else if(Func_vendor == CallModeRgb_vendor_typedef){
		CallModeRgb(adr, srgbID);
	}
	else if(Func_vendor == DelSceneRgb_vendor_typedef){
		DelSceneRgb(adr, appID);
	}
	else if(Func_vendor == SaveGateway_vendor_typedef){
		flag_saveGW = true;
		SaveGateway(adr);
	}
	else if(Func_vendor == AskTypeDevice_vendor_typedef){
		flag_typeDEV = true;
		AskTypeDevice(adr);
	}
	else if(Func_vendor == SetTypeDevice_vendor_typedef){
		flag_typeDEV = true;
		SetTypeDevice(adr,type,attrubute,application);
	}

	uint8_t *tempDataUart;
	tempDataUart = (uint8_t *)&vrts_CMD_STRUCTURE_VENDOR;
	ControlMessage(cmdLength, tempDataUart);
}
uint16_t Percent2ParamCCT(uint8_t percent)
{
	return ((percent*192)+800);
}
uint8_t Param2PrecentCCT(uint16_t param){
	return ((param-800)/192);
}
uint16_t Percent2ParamDIM(uint8_t percent)
{
	return ((percent*65535)/100);
}
uint8_t Param2PrecentDIM(uint16_t param){
	return ((param*100)/65535);
}
uint16_t Percent2ParamHSL(uint8_t percent){
	return((percent * 65535)/100);
}
uint8_t Param2PercentHSL(uint16_t param){
	return((param * 100)/65535);
}
