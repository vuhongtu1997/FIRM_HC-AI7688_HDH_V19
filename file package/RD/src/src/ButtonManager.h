/*
 * ButtonManager.h library process tasks of remote
 *
 */
#ifndef GATEWAYMANAGER_BUTTONMANAGER_H_
#define GATEWAYMANAGER_BUTTONMANAGER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/* format of data respond remote*/
typedef struct remotersp
{
	uint8_t   typeDev[2];
	uint8_t   buttonID;
	uint8_t   modeID;
	uint8_t   senceID[2];
	uint8_t   futureID[2];
}remotersp;
extern remotersp * vrts_Remote_Rsp;

typedef struct screenTouch
{
	uint8_t header[2];
	uint8_t buttonID;
	uint8_t sceneID[2];
	uint8_t aksTime;
	uint8_t future[4];
}screenTouch;
extern screenTouch *vrts_ScreenT_Rsp;

#define BUTTONID0           0x00
#define BUTTONID1           0x01
#define BUTTONID2           0x02
#define BUTTONID3           0x03
#define BUTTONID4           0x04
#define BUTTONID5           0x05

#define MODEIDCLICK         0x01
#define MODEIDDOUBLE        0x02


#define OPCODEREMOTE_CMD     0xA082
#define OPCODEREMOTE_RSP     0xA182

/*
 * Check press on remote (button, mode press)
 *
 * @param rsp data respond remote
 * @param parButtonId buttonid
 * @param parModeId modeid
 * @return true- check correct buttonid and modeid,
 * @return false- check wrong buttonid or modeid
 */
bool IsRemoteSetup(remotersp * rsp,unsigned char parButtonId,unsigned char parModeId);

#ifdef __cplusplus
}
#endif



#endif
