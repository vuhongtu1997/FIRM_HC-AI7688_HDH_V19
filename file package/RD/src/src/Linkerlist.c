#include "Linkerlist.h"
#include "JsonProcess.h"
#include "ShareMessage.h"
#include "slog.h"

typedef struct buffer_wait *vrts_buff;
vrts_buff head;

vrts_buff Create_buffer(char * message){
	int length = strlen(message);
	char *create_buffer = (char *)malloc(length + 1);
	strcpy(create_buffer,message);
	*(create_buffer + length) = '\0';
	vrts_buff newdata;
	newdata = (vrts_buff)malloc(sizeof(struct buffer_wait));
	newdata->message = create_buffer;
	newdata->next = NULL;
	return newdata;
}

vrts_buff AddTail(vrts_buff headA, char * message)
{
	vrts_buff temp,p;
	temp=Create_buffer(message);
	if(head == NULL){
		head = temp;
	}
	else{
		p=head;
		while(p->next != NULL){
			p=p->next;
		}
		p->next = temp;
	}
	return head;
}

vrts_buff DellHead(vrts_buff head){
	vrts_buff newhead;
	if(head != NULL){
		newhead = head->next;
		free(head->message);
		free(head);
	}
	return newhead;
}

void ShowLL(vrts_buff head){
	if(head == NULL){
		printf("linkerlist NULL");
	}
	else if(head->next == NULL){
		printf("\tData in linker list: %s\n",head->message);
	}
	else {
		vrts_buff p = head;
		for(p=head;p->next != NULL; p= p->next){
			printf("\tData in linker list: %s\n",p->message);
		}
		printf("\tData in linker list: %s\n",p->message);
	}
}
/*
void *LinkerList_Thread(void *argv)
{
	while(1){
		pthread_mutex_lock(&vrpth_LinkerList);
		printf("LOCK LINKERLIST %d\n",pthread_rwlock_rdlock(&rwlock_ll));

		if(head != NULL){
			get_start_time = true;
			while(!flag_check_rsp){
				if(get_start_time){
					get_start_time= false;
					start_t=clock();
				}
				end_t=clock();
				if((double)(end_t-start_t) >=450000){
					get_start_time = true;
					struct json_object * jobj= json_tokener_parse(head->message);
					Json_Parse(jobj);
					time_Send_setup++;
					if(time_Send_setup == 3){
						time_Send_setup = 0;
						pthread_mutex_lock(&vrpth_SHAREMESS_FlagCheckRsp);
						printf("LOCK CHECK RSP %d\n",pthread_mutex_lock(& vrpth_SHAREMESS_FlagCheckRsp));
						flag_check_rsp = true;
						pthread_mutex_unlock(&vrpth_SHAREMESS_FlagCheckRsp);
						printf("ULOCK CHECK RSP %d\n",pthread_mutex_lock(& vrpth_SHAREMESS_FlagCheckRsp));

					}
				}
			}
			head=DellHead(head);
			printf("DELETE LINKERLIST");
			ShowLL(head);
		}
		pthread_mutex_unlock(&vrpth_LinkerList);
		printf("UNLOCK LINKERLIST %d\n",pthread_mutex_unlock(& vrpth_LinkerList));
		usleep(1000);
	}
	return NULL;
}
*/

