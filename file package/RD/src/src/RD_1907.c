/*
 ============================================================================
 Name        : GatewayManager.c
 Author      : viet anh
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "GateInterface.h"
#include "Provision.h"
#include "MQTT.h"
#include "SensorLight.h"
#include "slog.h"
#include "Light.h"

pthread_t vrts_System_ThreadGWIF;
pthread_t vrts_System_TestSend;
pthread_t vrts_System_MQTT;
pthread_t vrts_System_Time;
pthread_t vrts_System_Gpio;


SLogConfig slgCfg;


/*z
 * Ham xu ly chinh. Thuc thi:
 * - Goi cac tac vu tuong ung cua chuong trinh
 * PENDING: Tiep tuc bo sung them cac Task khac
 */
int main(void) {

	puts("--------------Start system-----------------");
	slog_init("logfile", SLOG_FLAGS_ALL, 1);
	slog_config_get(&slgCfg);
	slgCfg.eColorFormat = SLOG_COLOR_DISABLE;
	slgCfg.nToFile = 1;
//	slgCfg.nToScreen = 0;
	slgCfg.nTraceTid = 1;
	slog_config_set(&slgCfg);
	flag_mac=true;
	flag_done=true;
	//pthread_create(&vrts_System_TestSend,NULL, ProvisionThread, NULL);
    pthread_create(&vrts_System_ThreadGWIF, NULL, GWINF_Thread, NULL);
    pthread_create(&vrts_System_MQTT,   NULL,   MQTT_Thread,    NULL);
//    pthread_create(&vrts_System_Time,NULL,Time_Thread, NULL);
//    pthread_create(&tmp1,NULL,Led_Thread,NULL);


	//pthread_join(vrts_System_TestSend, NULL);
    pthread_join(vrts_System_ThreadGWIF, NULL);
//    pthread_join(vrts_System_MQTT,NULL);
//    pthread_join(vrts_System_Time, NULL);
    //pthread_join(vrts_System_Gpio,NULL);

	while(1){
	}
	return EXIT_SUCCESS;
}
