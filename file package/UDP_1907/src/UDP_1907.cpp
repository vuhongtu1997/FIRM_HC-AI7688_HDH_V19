
//============================================================================
// Name        : UDP18_06.cpp
// Author      : rd
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <list>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <json-c/json.h>
#include <cstddef>

#define SCAN_HC "SCAN_HC"
#define HC_SCAN_WIFI "HC_SCAN_WIFI"
#define PORT 8181

using namespace std;

list<string> wifi_data;
list<string> rsp_wifi_data;
list<string> rsp_ip;

string msg_rsp;
string ip;
string mac;

bool flag_encryption = FALSE;

const char *PASSWORD,  *ENCRYPTION;
const char * SSID;

void process_data(string msg_rsp);

char* stringToChar(string s) {
	char *sendChar = new char[s.length() + 1];
	strcpy(sendChar, s.c_str());
	return sendChar;
}

void getMacAddress(char *uc_Mac)
{
	struct ifreq s;
	unsigned char *mac = NULL;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	strcpy(s.ifr_name, "eth0");
	if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
		mac = (unsigned char*) s.ifr_addr.sa_data;
	}
	sprintf((char*) uc_Mac, (const char*) "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	puts(uc_Mac);
}

string getIP() {
	msg_rsp = "";
	string string_ip_1;
	string string_ip_2;
	string string_ip_3;
	FILE *file;
	stringstream msg;
	char msg_line[2048]={0};
	file = popen("ip -4 addr show ${link_name} | sed -Ene \'s/^.*inet ([0-9.]+)\\/.*$/\\1/p\'", "r");
//	file = popen("iwinfo wlan0 scan", "r");
	if(file==NULL)
	{
		puts("lỗi");
		exit(1);
	}
	fgets(msg_line, 100, file);
	msg_rsp += msg_line;
	while(1)
	{
		fgets(msg_line, 100, file);
		if(feof(file))
		{
			break;
		}
		msg_rsp += msg_line;
	}
	pclose(file);
	cout<<msg_rsp<<endl;
	int count = 0;
	const char * char_msg_rsp = msg_rsp.c_str();
	puts(char_msg_rsp);
	for(unsigned int i = 0; i < strlen(char_msg_rsp); i++)
	{
		if(char_msg_rsp[i] == '\n')
		{
			count ++;
		}
	}
	printf("----%d----\n", count);
	if(count==3)
	{
		int ip_index = 0;
		int count_line_index=0;
		for(unsigned int i = 0; i < strlen(char_msg_rsp); i++)
		{
			if(char_msg_rsp[i] == '\n')
			{
				count_line_index ++;
			}
			if(count_line_index == 0 && char_msg_rsp[i] != '\n')
			{
				string_ip_1 = string_ip_1 + char_msg_rsp[i];
			}
			if(count_line_index == 1 && char_msg_rsp[i] != '\n')
			{
				string_ip_2 = string_ip_2 + char_msg_rsp[i];
			}
			if(count_line_index == 2 && char_msg_rsp[i] != '\n')
			{
				string_ip_3 = string_ip_3 + char_msg_rsp[i];
				ip_index ++;
			}
		}
		if(string_ip_1.compare("127.0.0.1")!=0 && string_ip_1.compare("10.10.10.1")!=0 && string_ip_1.compare("")!=0)
		{
			return string_ip_1;
		}
		if(string_ip_2.compare("127.0.0.1")!=0 && string_ip_2.compare("10.10.10.1")!=0 && string_ip_2.compare("")!=0)
		{
			return string_ip_2;
		}
		if(string_ip_3.compare("127.0.0.1")!=0 && string_ip_3.compare("10.10.10.1")!=0 && string_ip_3.compare("")!=0)
		{
			return string_ip_3;
		}
	}

	if(count==2)
	{
		char char_ip[20]={0};
		int ip_index = 0;
		int count_line_index=0;
		for(unsigned int i = 0; i < strlen(char_msg_rsp); i++)
		{
			if(char_msg_rsp[i] == '\n')
			{
				count_line_index ++;
			}
			if(count_line_index == 0 && char_msg_rsp[i] != '\n')
			{
				string_ip_1 = string_ip_1 + char_msg_rsp[i];
			}
			if(count_line_index == 1 && char_msg_rsp[i] != '\n')
			{
				string_ip_2 = string_ip_2 + char_msg_rsp[i];
			}
		}
		if(string_ip_1.compare("127.0.0.1")!=0 && string_ip_1.compare("")!=0)
		{
			return string_ip_1;
		}
		if(string_ip_2.compare("127.0.0.1")!=0 && string_ip_2.compare("")!=0)
		{
			return string_ip_2;
		}
	}
}

void openfile(char* command)
{
	FILE *file;
	msg_rsp = "";
	stringstream msg;
	char msg_line[100]={0};
	file = popen(command, "r");
	if(file==NULL)
	{
		puts("lỗi");
		exit(1);
	}
	fgets(msg_line, 100, file);
	msg_rsp += msg_line;
	while(1)
	{
		fgets(msg_line, 100, file);
		if(feof(file))
		{
			break;
		}
		msg_rsp += msg_line;
	}
	pclose(file);
}

void connect_wifi(char * key)
{
	if ((strcmp(key, "SSID")) == 0) {
		char command[100];
		openfile("uci del network.wan.ifname");
		sprintf(command, "uci set wireless.wifinet1.ssid='%s'", SSID);
		openfile("uci del wireless.wifinet1");
		openfile("uci set wireless.wifinet1=wifi-iface");
		openfile(command);
		openfile("uci set wireless.wifinet1.mode='sta'");
		openfile("uci set wireless.wifinet1.network='wan'");
		openfile("uci set wireless.wifinet1.device='radio0'");
	}
	if ((strcmp(key, "PASSWORD")) == 0) {
		char command[100];
		sprintf(command, "uci set wireless.wifinet1.key='%s'", PASSWORD);
		openfile(command);
	}
	if ((strcmp(key, "ENCRYPTION")) == 0) {
		char command[100];
//		FILE *file;
		if(strcmp(ENCRYPTION, "none")==0)
		{
			sprintf(command, "uci set wireless.wifinet1.encryption='%s'", ENCRYPTION);
			openfile(command);
			sleep(30);
			ip=getIP();
			cout << ip << endl;
			if(strcmp(ip.c_str(),"192.168.1.1")==0)
			{
				openfile("uci del wireless.wifinet1");
				openfile("uci set wireless.wifinet1=wifi-iface");
				openfile("uci commit wireless");
				openfile("uci commit network");
				openfile("wifi");
				openfile("/etc/init.d/network restart");
			}
			openfile("wifi");
			openfile("/etc/init.d/network restart");
		}
		if(strcmp(ENCRYPTION, "none")!=0)
		{
			char msg_line[100]={0};
			openfile("uci set wireless.wifinet1.encryption='psk2'");
			openfile("uci commit wireless");
			openfile("uci commit network");
			openfile("wifi");
			openfile("/etc/init.d/network restart");
			sleep(30);
			ip=getIP();
			cout << ip << endl;
			if(strcmp(ip.c_str(),"192.168.1.1")==0)
			{
				openfile("uci del wireless.wifinet1");
				openfile("uci commit network");
				openfile("uci commit wireless");
				openfile("wifi");
				openfile("/etc/init.d/network restart");
			}
			openfile("uci commit network");
			openfile("/etc/init.d/network restart");
		}
	}
}

void scan_wifi()
{
	msg_rsp = "";
	FILE *file;
	stringstream msg;
	char msg_line[2048]={0};
	file = popen("iwinfo wlan0 scan", "r");
	if(file==NULL)
	{
		puts("lỗi");
		exit(1);
	}
	fgets(msg_line, 100, file);
	msg_rsp += msg_line;
	while(1)
	{
		fgets(msg_line, 100, file);
		if(feof(file))
		{
			break;
		}
		msg_rsp += msg_line;
	}
	pclose(file);
	process_data(msg_rsp);
}

void process_data(string msg_rsp)
{
	string BSS = "Cell ";
	size_t pos = 0;
	string wifi;
	while ((pos = msg_rsp.find(BSS)) != string::npos)
	{
		wifi = msg_rsp.substr(0, pos);
		msg_rsp.erase(0,pos+BSS.length());
		wifi_data.push_front(wifi);
	}
	for (list<string>::iterator it = wifi_data.begin(); it !=wifi_data.end(); it++)
	{
		const char * it_char = (*it).c_str();
		puts(it_char);
		int line_index = 0;
		if (strlen(it_char) >= 10)
		{
			struct json_object * object;
			char SSID[200] = {0};
			char QUALITY[50] = {0};
			char ENCRYPTION[50] = {0};
			unsigned int Authentication = 0;
			unsigned int Authentication1 = 0;
			const char *sub;
			const char *sub1;
			sub1 = strstr(it_char,"/70");
			sub = strstr(it_char,"Quality");
			if (sub)
			{
				Authentication = sub-it_char;
			}
			if (sub1)
			{
				Authentication1 = sub1-it_char;
			}
			int QUALITY_index = 0;
			int SSID_index = 0;
			int ENCRYPTION_index = 0;
			for (unsigned int i = 0; i <strlen(it_char); i++)
			{
				if (it_char[i]=='\n')
				{
					line_index ++;
				}
				if (line_index == 1 && it_char[i]!= 0x22 )
				{
					if (SSID_index > 17)
					{
						SSID[SSID_index-18] = it_char[i];
						SSID_index ++;
					}
					else
					{
						SSID_index ++;
					}
				}
				if (line_index == 3 && i >= Authentication && i < Authentication1)
				{
					if (QUALITY_index > 8)
					{
						QUALITY[QUALITY_index-9] = it_char[i];
						QUALITY_index++;
					}
					else
					{
						QUALITY_index++;
					}
				}
				if (line_index == 4)
				{
					if (ENCRYPTION_index > 22)
					{
						ENCRYPTION[ENCRYPTION_index-23] = it_char[i];
						ENCRYPTION_index++;
					}
					else
					{
						ENCRYPTION_index++;
					}
				}
			}
			int int_QUALITY = 0;
			int_QUALITY = atoi(QUALITY);
			int_QUALITY = int_QUALITY*100/70;
			const char* char_QUALITY;
			string tmp = to_string(int_QUALITY);
			char_QUALITY = tmp.c_str();
			object = json_object_new_object();
			json_object_object_add(object, "CMD", json_object_new_string("HC_RESPONE"));
			json_object_object_add(object, "SSID", json_object_new_string(SSID));
			json_object_object_add(object, "QUALITY", json_object_new_string(char_QUALITY));
			json_object_object_add(object, "ENCRYPTION", json_object_new_string(ENCRYPTION));
			const char * rsp;
			rsp = json_object_to_json_string(object);
			puts(rsp);
			rsp_wifi_data.push_front(rsp);
			printf("----%d----\n", int_QUALITY);
			puts(SSID);
			puts(ENCRYPTION);
		}
	}
}

int main() {
	struct json_object *jobj;
	int sock, ret, count;
	fd_set readfd;
	unsigned int addr_len;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
	{
		perror("socket error\n");
		return -1;
	}

	addr_len = sizeof(struct sockaddr_in);

	memset((void*) &server_addr, 0, addr_len);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	ret = bind(sock, (struct sockaddr*) &server_addr, addr_len);
	if (ret < 0)
	{
		perror("bind error\n");
		return -1;
	}

	while(1)
	{
		FD_ZERO(&readfd);
		FD_SET(sock, &readfd);

		ret = select(sock + 1, &readfd, NULL, NULL, 0);
		if (ret > 0)
		{
			if (FD_ISSET(sock, &readfd))
			{
				char buffer[1024] = {0};
				count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*) &client_addr, &addr_len);
				puts(buffer);
				enum json_tokener_error error;
				jobj = json_tokener_parse_verbose(buffer, &error);
				if( error != json_tokener_success)
				{
					cout << "lỗi" << endl;
				}
				else
				{
					enum json_type type;
					const char *cmd;
					json_object_object_foreach(jobj, key, val)
					{
						type = json_object_get_type(val);
						switch (type)
						{
							case json_type_string:
								if(strcmp(key,"CMD") == 0)
								{
									cmd = json_object_get_string(json_object_object_get(jobj,"CMD"));
									if( strcmp(cmd,SCAN_HC) == 0)
									{
										ip=getIP();
										cout << ip << endl;
										char mac[20] = { 0 };
										char char_hostname[256]={0};
										getMacAddress(mac);
										gethostname(char_hostname, sizeof(char_hostname));
										cout << string(char_hostname) <<endl;

										struct json_object * object;
										object = json_object_new_object();
										json_object_object_add(object, "CMD", json_object_new_string("HC_RESPONE"));
										json_object_object_add(object, "IP", json_object_new_string(ip.c_str()));
										json_object_object_add(object, "HOSTNAME", json_object_new_string(char_hostname));
										json_object_object_add(object, "MAC", json_object_new_string(mac));
										const char * rsp;
										rsp = json_object_to_json_string(object);
										puts(rsp);
										count = sendto(sock, rsp, strlen(rsp), 0, (struct sockaddr*) &client_addr, addr_len);
										cout << rsp << endl;
									}
									else if( strcmp(cmd,HC_SCAN_WIFI) == 0)
									{
										scan_wifi();
										for (list<string>::iterator it = rsp_wifi_data.begin(); it !=rsp_wifi_data.end(); it++)
										{
											count = sendto(sock, stringToChar(*it), strlen(stringToChar(*it)), MSG_CONFIRM, (struct sockaddr*) &client_addr, addr_len);
										}
									}
								}
								SSID = json_object_get_string(json_object_object_get(jobj,"SSID"));
								PASSWORD = json_object_get_string(json_object_object_get(jobj,"PASSWORD"));
								ENCRYPTION = json_object_get_string(json_object_object_get(jobj,"ENCRYPTION"));
								connect_wifi(key);
							break;
							default:
							break;
						}
					}
				}
			}
		}
	}
	return 0;
}
