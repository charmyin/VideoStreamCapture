#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "header/bufferOperate.h"
#include "cjson/cJSON.h"
#include "header/timeOperate.h"
#include <sys/types.h>
#include <sys/wait.h>

//Send socket data
// headData: struts data, contains size of jsonData
// jsonData: json string data, parameters upload
// handler: socket handler
int sendSocketData(struct tcpRequire headData, char *jsonData, int sfd){
  	unsigned char buffSend[20];
  	//json string length
  	if(jsonData==0)
  		headData.jsonSize=0;
	else
		headData.jsonSize=strlen(jsonData);
  	getSendDataInBinary(buffSend, headData);
  	//send head
  	int r;
  	r=send(sfd, &buffSend, 20, 0);
  	if(r==-1){
  		return -1;
  	}
  	//send json parameters
  	if(jsonData!=0){
		r=send(sfd, jsonData, headData.jsonSize, 0);
		if(r==-1)
			return -1;
  	}
  	return 0;
}

int receiveSocketStruct(struct tcpRequire *returnStruct, int sfd){
	  int r;
	//unsigned char returnedBuff[20];
	  struct tcpRequire tempStruct;
	  r=recv(sfd, &(returnStruct->firstInt) ,4, MSG_WAITALL);
	 // printf("&returnStruct->firstInt is %08x;\n", tempStruct.firstInt);
	  r=recv(sfd, &(returnStruct->secondInt) ,4, MSG_WAITALL);
	  //printf("&returnStruct->secondInt is %08x;\n", tempStruct.secondInt);
	  r=recv(sfd, &(returnStruct->thirdInt) ,4, MSG_WAITALL);
	  //printf("&returnStruct->thirdInt is %08x;\n", tempStruct.thirdInt);
	  r=recv(sfd, &(returnStruct->fourthInt) ,4, MSG_WAITALL);
	  //printf("&returnStruct->fourthInt is %08x;\n", tempStruct.fourthInt);
	  r=recv(sfd, &(returnStruct->jsonSize) ,4, MSG_WAITALL);
	 //printf("&returnStruct->fifthInt is %08x;\n", tempStruct.jsonSize);
	  //unsigned char bufftemp[returnStruct.jsonSize];
	 // memcpy(returnStruct, &tempStruct, sizeof(tempStruct));
	  //printf("sizeof char=%08x;%08x;%08x;%08x;%08x;\n", returnStruct.firstInt,returnStruct.secondInt,returnStruct.thirdInt,returnStruct.fourthInt,returnStruct.jsonSize);
	/*  r=recv(sfd, buff ,returnStruct->jsonSize, MSG_WAITALL);
	  memcpy(buff, tempBuff, sizeof(tempBuff));*/
	  return r;
}
//Get json data after struct data
//size: size of json string data
//buff: json string data
//sfd: socket file descriptor
int receiveSocketJson(int size, unsigned char *buff, int sfd){
	return  recv(sfd, buff, size, MSG_WAITALL);
}



int main(){
  //connect handler; vsfd is used to receive video data
  int sfd, ffd, vsfd;
  //r1 is used to receive video databuff
  int r, r1;
  int len;
  int fileSize;//File size of received;
  int recvSize=0; // Number show how much bytes has been recevied; 23Bytes in 78Bytes;
  char buf[1514];
  char filename[100];
  struct sockaddr_in dr;

  sfd=socket(AF_INET, SOCK_STREAM, 0);
  if(sfd==-1){
    printf("socket error %m\n");
    exit(-1);
  }
  printf("建立socket服务器成功!\n");

  dr.sin_family=AF_INET;
  dr.sin_port=htons(34568);
  dr.sin_addr.s_addr=inet_addr("192.168.109.15");
  r=connect(sfd, (struct sockaddr*)&dr, sizeof(dr));
  if(r==-1){
    printf("connect error: %m\n");
    close(sfd);
    exit(-1);
  }
  printf("绑定地址成功!\n");

  printf("listen successfully~\n");

  printf("Accept successfully~\n");

   //prepare struct data
  	struct tcpRequire testStruct;
  	testStruct.firstInt=0x000000ff;
  	//r=send(sfd, &testStruct.firstInt, 4, 0);
  	testStruct.secondInt=0x00000000;
  	//r=send(sfd, &testStruct.secondInt, 4, 0);
  	testStruct.thirdInt=0x0;
  	//r=send(sfd, &testStruct.thirdInt, 4, 0);
  	testStruct.fourthInt=0x03e80000;
  	//r=send(sfd, &testStruct.fourthInt, 4, 0);

  	char tempJson[] = "{ \"EncryptType\" : \"MD5\", \"LoginType\" : \"DVRIPWeb\", \"PassWord\" : \"6QNMIQGe\", \"UserName\" : \"admin\" }\n";
  	//send prepared data to ipc
	if(sendSocketData(testStruct, tempJson, sfd)==-1){
		printf("Something has wrong on Sending User name and password~");
		return 0;
	}


	//Receive and analysis the data
  struct tcpRequire returnStruct;
  receiveSocketStruct(&returnStruct, sfd);
  unsigned char buff[returnStruct.jsonSize];
  if(receiveSocketJson(returnStruct.jsonSize, buff, sfd)==-1){
	printf("Something has wrong on Receiving first data~");
	return 0;
  }
 // free(buff);

  //printf("size of returned data: %d\n",returnStruct.jsonSize);
  int countBuff;
  //for(countBuff=0; countBuff<128; countBuff++)
 printf("Received %d string is : %s;\n", countBuff, buff);

  // Get SessionID
	cJSON *json, *jsonSessionIDObj;
	json=cJSON_Parse(buff);
	jsonSessionIDObj = cJSON_GetObjectItem(json, "SessionID");
	//Get origin sessionID
	printf("session id is : %s\n", jsonSessionIDObj->valuestring);
	char *sessionID =malloc(sizeof(sessionID));
	strcpy(sessionID, jsonSessionIDObj->valuestring);

    int sessionIDNum;
	sscanf(sessionID, "%x", &sessionIDNum);

  //r=connect(sfd, (struct sockaddr*)&dr, sizeof(dr));

/************************************Send Name SystemInfo*******************************************/

    //prepare struct data to request data
	struct tcpRequire secondStruct;
	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=sessionIDNum;
	secondStruct.thirdInt=0x0;
	secondStruct.fourthInt=0x03fc0000;

	cJSON *jsonDst, *jsonSessionIDObjDst;
	//free(tempJson);
  	char *tempDstJson = "{ \"Name\" : \"SystemInfo\", \"SessionID\" : \"0x00000040\" }\n";

  	//Pay the value to send json
  	jsonDst= cJSON_Parse(tempDstJson);
  	jsonSessionIDObjDst = cJSON_GetObjectItem(jsonDst, "SessionID");
  	jsonSessionIDObjDst->valuestring=malloc(sizeof(sessionID));
  	strcpy(jsonSessionIDObjDst->valuestring,sessionID);

	char *requestString=cJSON_PrintUnformatted(jsonDst);

	printf("Send Name SystemInfo: %s\n",requestString);

	//Release json obj
	cJSON_Delete(json);
	cJSON_Delete(jsonDst);


	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestString, sfd)==-1){
		printf("Something has wrong on Sending Name SystemInfo~");
		return 0;
	}
	//Receive the channel info and so on
   receiveSocketStruct(&returnStruct, sfd);
   unsigned char buff2[returnStruct.jsonSize];
   if(receiveSocketJson(returnStruct.jsonSize, buff2, sfd)==-1){
	  printf("Something has wrong on Receiving first data~");
	  return 0;
   }

 printf("Received string  SystemInfo is : %s;\n", buff2);

  	/************************************Send KeepAlive*******************************************/

  	secondStruct.firstInt=0x000000ff;
	//r=send(sfd, &testStruct.firstInt, 4, 0);
	secondStruct.secondInt=sessionIDNum;
	//r=send(sfd, &testStruct.secondInt, 4, 0);
	secondStruct.thirdInt=0x0;
	//r=send(sfd, &testStruct.thirdInt, 4, 0);
	secondStruct.fourthInt=0x03ee0000;
	//r=send(sfd, &testStruct.fourthInt, 4, 0);

	//free(tempJson);
  	cJSON *jsonKeepalive, *jsonSessionIDKeepalive;

	char *tempKeepaliveJson = "{ \"Name\" : \"KeepAlive\", \"SessionID\" : \"0x00000040\" }\n";

	//Pay the value to send json
	jsonKeepalive= cJSON_Parse(tempKeepaliveJson);
	jsonSessionIDKeepalive = cJSON_GetObjectItem(jsonKeepalive, "SessionID");
	jsonSessionIDKeepalive->valuestring=malloc(sizeof(sessionID));
	strcpy(jsonSessionIDKeepalive->valuestring,sessionID);

	char *requestKeepAliveString=cJSON_PrintUnformatted(jsonKeepalive);

	printf("Send KeepAlive %s\n",requestKeepAliveString);

	//Release json obj
	cJSON_Delete(jsonKeepalive);
	//cJSON_Delete(jsonSessionIDKeepalive);

	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestKeepAliveString, sfd)==-1){
		printf("Something has wrong on Sending KeepAlive~");
		return 0;
	}

	//Receive the channel info and so on
   receiveSocketStruct(&returnStruct, sfd);
   unsigned char buff3[returnStruct.jsonSize];
   if(receiveSocketJson(returnStruct.jsonSize, buff3, sfd)==-1){
	  printf("Something has wrong on Receiving first data~");
	  return 0;
   }

   printf("Received string KeepAlive is : %s\n", buff3);

/************************************Time setting***********************************/

  	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=sessionIDNum;
	secondStruct.thirdInt=0x00000001;
	secondStruct.fourthInt=0x06360000;

  	cJSON *jsonSetTime, *jsonSessionIDSetTime, *jsonOPTimeSettingNoRTCSetTime;
  	char * formatRealTime = now();

	char *tempSetTimeJson = "{ \"Name\" : \"OPTimeSettingNoRTC\", \"OPTimeSettingNoRTC\" : \"20140313 08:03:07\", \"SessionID\" : \"0x89\" }\n";

	//Pay the value to send json
	jsonSetTime= cJSON_Parse(tempSetTimeJson);
	jsonSessionIDSetTime = cJSON_GetObjectItem(jsonSetTime, "SessionID");
	jsonSessionIDSetTime->valuestring = malloc(sizeof(sessionID));
	strcpy(jsonSessionIDSetTime->valuestring,sessionID);
	//jsonSessionIDSetTime->valuestring=sessionID;

	jsonOPTimeSettingNoRTCSetTime = cJSON_GetObjectItem(jsonSetTime, "OPTimeSettingNoRTC");
	jsonOPTimeSettingNoRTCSetTime->valuestring = malloc(sizeof(formatRealTime));
	strcpy(jsonOPTimeSettingNoRTCSetTime->valuestring,formatRealTime);
	//jsonOPTimeSettingNoRTCSetTime->valuestring=formatRealTime;

	char *requestSetTimeString=cJSON_PrintUnformatted(jsonSetTime);

	printf("Time setting %s\n",requestSetTimeString);

	//Release json obj
	cJSON_Delete(jsonSetTime);

	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestSetTimeString, sfd)==-1){
		printf("Something has wrong on Sending Time setting~");
		return 0;
	}

	/***************************Send name & session id again*****************************/

	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=sessionIDNum;
	secondStruct.thirdInt=0x0;
	secondStruct.fourthInt=0x05dc0000;

	//free(tempJson);
	tempDstJson = "{ \"Name\" : \"\", \"SessionID\" : \"0x00000040\" }\n";

	//Pay the value to send json
	jsonDst= cJSON_Parse(tempDstJson);
	jsonSessionIDObjDst = cJSON_GetObjectItem(jsonDst, "SessionID");
	jsonSessionIDObjDst->valuestring= malloc(sizeof(sessionID));
	strcpy(jsonSessionIDObjDst->valuestring, sessionID);

	requestString=cJSON_PrintUnformatted(jsonDst);

	printf("Send name & session id again : %s\n",requestString);

	//Release json obj
	cJSON_Delete(jsonDst);

	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestString, sfd)==-1){
		printf("Something has wrong on Sending Name SystemInfo~");
		return 0;
	}

	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=sessionIDNum;
	secondStruct.thirdInt=0x0;
	secondStruct.fourthInt=0x061a0000;
	//send prepared data to ipc
	requestString=0;
	if(sendSocketData(secondStruct, requestString, sfd)==-1){
		printf("Something has wrong on Sending Name SystemInfo~");
		return 0;
	}


	//Receive the last info
   receiveSocketStruct(&returnStruct, sfd);
   unsigned char buff4[returnStruct.jsonSize];
   if(receiveSocketJson(returnStruct.jsonSize, buff4, sfd)==-1){
	  printf("Something has wrong on Receiving first data~");
	  return 0;
   }

   printf("Received string Last 1 is : %s\n", buff4);

	//Receive the last info
  receiveSocketStruct(&returnStruct, sfd);
  unsigned char buff5[returnStruct.jsonSize];
  if(receiveSocketJson(returnStruct.jsonSize, buff5, sfd)==-1){
	  printf("Something has wrong on Receiving first data~");
	  return 0;
  }

  printf("Received string Last 2 is : %s\n", buff5);


/*	//Receive the last info
receiveSocketStruct(&returnStruct, sfd);
unsigned char buff7[returnStruct.jsonSize];
if(receiveSocketJson(returnStruct.jsonSize, buff7, sfd)==-1){
	  printf("Something has wrong on Receiving first data~");
	  return 0;
}*/

//printf("Received string Last 3 is : %s\n", buff7);


if(fork()==0){

      int sfd2;
	  struct sockaddr_in dr2;

	  sfd2=socket(AF_INET, SOCK_STREAM, 0);
	  if(sfd2==-1){
	    printf("socket error %m\n");
	    exit(-1);
	  }
	  printf("建立socket服务器成功!\n");

	  dr2.sin_family=AF_INET;
	  dr2.sin_port=htons(34568);
	  dr2.sin_addr.s_addr=inet_addr("192.168.109.15");
	  r=connect(sfd2, (struct sockaddr*)&dr2, sizeof(dr2));
	  if(r==-1){
	    printf("Second connect error: %m\n");
	    close(sfd2);
	    exit(-1);
	  }
	  printf("Second connect绑定地址成功!\n");
	  /************************************Send OPMonitor in child process*******************************************/
	    secondStruct.firstInt=0x000000ff;
	   	//r=send(sfd, &testStruct.firstInt, 4, 0);
	   	secondStruct.secondInt=sessionIDNum;
	   	//r=send(sfd, &testStruct.secondInt, 4, 0);
	   	secondStruct.thirdInt=0x0;
	   	//r=send(sfd, &testStruct.thirdInt, 4, 0);
	   	secondStruct.fourthInt=0x05850000;
	   	//r=send(sfd, &testStruct.fourthInt, 4, 0);

	   	char *OPMonitorJsonString = "{ \"Name\" : \"OPMonitor\", \"OPMonitor\" : { \"Action\" : \"Claim\", \"Parameter\" : { \"Channel\" : 0, \"CombinMode\" : \"NONE\", \"StreamType\" : \"Main\", \"TransMode\" : \"TCP\" } }, \"SessionID\" : \"0x43\" }\n";

	  	cJSON *jsonOPMonitor, *jsonSessionIDOPMonitor;

		//Pay the value to send json
	  	jsonOPMonitor= cJSON_Parse(OPMonitorJsonString);
	  	jsonSessionIDOPMonitor = cJSON_GetObjectItem(jsonOPMonitor, "SessionID");
	  	jsonSessionIDOPMonitor->valuestring=malloc(sizeof(sessionID));
		strcpy(jsonSessionIDOPMonitor->valuestring,sessionID);

		char *requestSessionIDOPMonitor=cJSON_PrintUnformatted(jsonOPMonitor);

		printf("Send OPMonitor in child process :: %s\n",requestSessionIDOPMonitor);

		//Release json obj
		cJSON_Delete(jsonOPMonitor);

	   	//printf("Send OPMonitor in child process %s\n",OPMonitorJsonString);

	   	//send prepared data to ipc
	   	if(sendSocketData(secondStruct, requestSessionIDOPMonitor, sfd2)==-1){
	   		printf("Something has wrong on Sending child OPMonitor~");
	   		return 0;
	   	}

		printf("Child receive 1:");
	     //Receive the channel info and so on
	      receiveSocketStruct(&returnStruct, sfd2);
	      unsigned char buff6[returnStruct.jsonSize];
	      if(receiveSocketJson(returnStruct.jsonSize, buff6, sfd2)==-1){
	   	  printf("Something has wrong on Receiving first data child OPMonitor~");
	   	  return 0;
	      }

	      printf("Received string OPMonitor in child is : %s\n", buff6);
          sleep(1);
	      //创建文件
	      int  ffd=open("/home/charmyin/hello.h264", O_RDWR|O_CREAT, 0666);

	       float j=0;
	       ////异常处理
	       int i=1;
	       //循环接收文件数据

	       while(i<1000){
	     	  r=recv(sfd2, buf, 1514, MSG_WAITALL);
	     	 // printf(" %\n", buf);
	     	 write(ffd, buf, 1514);
	     	/*  if(r==-1){
	     		  printf("Receive wrong~%d\n", i);
	     		  break;
	     	  }*/
	     	  i++;
	     	  //if(i==2)
	     		//  break;
	       }
	       fflush(stdout);
	       close(ffd);
	       /*while(1){
	         r=recv(cfd, &len, sizeof(len), MSG_WAITALL);
	         if(len==0){
	           break;
	         }
	         r=recv(cfd, buf, len, MSG_WAITALL);

	         write(ffd, buf, len);
	         recvSize+=len;
	         float j=((float)recvSize)*100/fileSize;

	         //printf the percentage progress: 87%;
	         if((int)j!=(int)i){
	     //    	printf("recvSize=%d;\n",recvSize);
	     //    	printf("i=%.0f; j=%.0f \n;", i, j);
	         	i=j;
	     		putchar('\b');
	     		putchar('\b');
	     		putchar('\b');
	     		putchar('\b');
	     		printf("%3d%%",(int)j);
	     		fflush(stdout);
	         }
	       }*/
	     /*  close(ffd);*/

  }else{
	  sleep(1);
	  printf("-------------------------------------------\n");
	  /************************************Send OPMonitor in parent  process*******************************************/
	secondStruct.firstInt=0x000000ff;
	//r=send(sfd, &testStruct.firstInt, 4, 0);
	secondStruct.secondInt=sessionIDNum;
	//r=send(sfd, &testStruct.secondInt, 4, 0);
	secondStruct.thirdInt=0x0;
	//r=send(sfd, &testStruct.thirdInt, 4, 0);
	secondStruct.fourthInt=0x05820000;
	//r=send(sfd, &testStruct.fourthInt, 4, 0);

	char *OPMonitorJsonStringMain = "{ \"Name\" : \"OPMonitor\", \"OPMonitor\" : { \"Action\" : \"Start\", \"Parameter\" : { \"Channel\" : 0, \"CombinMode\" : \"NONE\", \"StreamType\" : \"Main\", \"TransMode\" : \"TCP\" } }, \"SessionID\" : \"0x43\" }\n";

	cJSON *jsonOPMonitorMain, *jsonSessionIDOPMonitorMain;

	//Pay the value to send json
	jsonOPMonitorMain= cJSON_Parse(OPMonitorJsonStringMain);
	jsonSessionIDOPMonitorMain = cJSON_GetObjectItem(jsonOPMonitorMain, "SessionID");
	jsonSessionIDOPMonitorMain->valuestring=malloc(sizeof(sessionID));
	strcpy(jsonSessionIDOPMonitorMain->valuestring,sessionID);

	char *requestSessionIDOPMonitorMain=cJSON_PrintUnformatted(jsonOPMonitorMain);

	printf("Send OPMonitor in father  process :: %s\n",requestSessionIDOPMonitorMain);

	//Release json obj
	cJSON_Delete(jsonOPMonitorMain);

	//printf("Send OPMonitor in child process %s\n",OPMonitorJsonString);

	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestSessionIDOPMonitorMain, sfd)==-1){
		printf("Something has wrong on Sending father OPMonitor~");
		return 0;
	}

	printf("Parent receive 1:\n");
	//Receive the channel info and so on
	  receiveSocketStruct(&returnStruct, sfd);
	  unsigned char buff8[returnStruct.jsonSize];
	  if(returnStruct.jsonSize==0){
		  printf("Empty thing-----------\n");
	  }else  if(receiveSocketJson(returnStruct.jsonSize, buff8, sfd)==-1){
		  printf("Something has wrong on Receiving first data OPMonitor~");
		  return 0;
	  }

	  printf("Received string OPMonitorMain parent is : %s\n", buff8);
	  /*printf("This is parent\n");
	  printf("Parent receive 2:\n");
	  //Receive the channel info and so on
	  	  receiveSocketStruct(&returnStruct, sfd);
	  	  unsigned char buff9[returnStruct.jsonSize];
	  	  if(receiveSocketJson(returnStruct.jsonSize, buff9, sfd)==-1){
	  		  printf("Something has wrong on Receiving first data OPMonitor~");
	  		  return 0;
	  	  }
	  	  printf("Received string OPMonitorMain parent is : %s\n", buff9);
	  	  printf("This is parent\n");*/

	  int *status;
	  wait(status);
	  printf("Child status is %d\n", *status);
  	  close(sfd);
  	  printf("Over~");
  }

  return 0;
}
