/*
 * OpCode.c
 */

#include "OpCode.h"

// for mqtt control process
bool MODE_PROVISION = false;
bool check_add_or_del_group = false ;
bool check_add_or_del_scene = false ;
bool flag_SendCmd_Done = true;

uint16_t TypeConvertID(uint8_t type, uint8_t attrubute, uint8_t application){
	uint16_t id = 65535;
	if((type != 255) && (attrubute != 255) && (application != 255)){
		id = (type*10000 + attrubute*1000 + application);
	}
	return id;
}
