/*
 * TIMER_PARES.hpp
 *
 *  Created on: Apr 14, 2021
 *      Author: trthang
 */

#ifndef RD_TIMER_TIMER_PARES_HPP_
#define RD_TIMER_TIMER_PARES_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <string>
#include <mosquitto.h>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <ctime>

#include "../../rapidjson/document.h"
#include "../../rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;



extern pthread_t tmp_thread;


void* CallEvent(void *argv);

void* killThread(void *argv);


//--------------------------------------------------------------------Timer

int Timer_AHG(struct mosquitto *mosq, char * jobj);

int BUTTON(struct mosquitto *mosq, char * jobj);

int tachTimer(char* a);

void readJson(char* s);

void sapXepArr(int arr[], int lengt);

int checkTypeAtt(struct mosquitto *mosq, string Code);

void gopArr(int arrS[], int arrE[], int n, int m);

string parthTime(int a);

int setTimer();

string setDayTimer();

void XoaCacPhanTuTrungNhau(int a[], int &n);

char* checkControlTimer(struct mosquitto *mosq, int idControl);

int DBContext(struct mosquitto *mosq, string control, string sql);




#endif /* RD_TIMER_TIMER_PARES_HPP_ */
