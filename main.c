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
	  printf("&returnStruct->firstInt is %08x;\n", tempStruct.firstInt);
	  r=recv(sfd, &(returnStruct->secondInt) ,4, MSG_WAITALL);
	  printf("&returnStruct->secondInt is %08x;\n", tempStruct.secondInt);
	  r=recv(sfd, &(returnStruct->thirdInt) ,4, MSG_WAITALL);
	  printf("&returnStruct->thirdInt is %08x;\n", tempStruct.thirdInt);
	  r=recv(sfd, &(returnStruct->fourthInt) ,4, MSG_WAITALL);
	  printf("&returnStruct->fourthInt is %08x;\n", tempStruct.fourthInt);
	  r=recv(sfd, &(returnStruct->jsonSize) ,4, MSG_WAITALL);
	  printf("&returnStruct->fifthInt is %08x;\n", tempStruct.jsonSize);
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
  char buf[4];
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

  printf("size of returned data: %d\n",returnStruct.jsonSize);
  int countBuff;
  //for(countBuff=0; countBuff<128; countBuff++)
  printf("Received %d string is : %s;\n", countBuff, buff);

  // Get SessionID
	cJSON *json, *jsonSessionIDObj;
	json=cJSON_Parse(buff);
	jsonSessionIDObj = cJSON_GetObjectItem(json, "SessionID");
	//Get origin sessionID
	printf("session id is : %s\n", jsonSessionIDObj->valuestring);
	char *sessionID = jsonSessionIDObj->valuestring;

  //r=connect(sfd, (struct sockaddr*)&dr, sizeof(dr));

/************************************Send Name SystemInfo*******************************************/

    //prepare struct data to request data
	struct tcpRequire secondStruct;
	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=0x00000040;
	secondStruct.thirdInt=0x0;
	secondStruct.fourthInt=0x03fc0000;

	cJSON *jsonDst, *jsonSessionIDObjDst;
	//free(tempJson);
  	char *tempDstJson = "{ \"Name\" : \"SystemInfo\", \"SessionID\" : \"0x00000040\" }\n";

  	//Pay the value to send json
  	jsonDst= cJSON_Parse(tempDstJson);
  	jsonSessionIDObjDst = cJSON_GetObjectItem(jsonDst, "SessionID");
  	jsonSessionIDObjDst->valuestring=sessionID;

	char *requestString=cJSON_Print(jsonDst);

	printf("%s\n",requestString);

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

   printf("Received string is : %s;\n", buff2);

  	/************************************Send KeepAlive*******************************************/

  	secondStruct.firstInt=0x000000ff;
	//r=send(sfd, &testStruct.firstInt, 4, 0);
	secondStruct.secondInt=0x00000043;
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
	jsonSessionIDKeepalive = cJSON_GetObjectItem(jsonDst, "SessionID");
	jsonSessionIDKeepalive->valuestring=sessionID;

	char *requestKeepAliveString=cJSON_Print(jsonDst);

	printf("%s\n",requestKeepAliveString);

	//Release json obj
	cJSON_Delete(jsonKeepalive);
	cJSON_Delete(jsonSessionIDKeepalive);

	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestKeepAliveString, sfd)==-1){
		printf("Something has wrong on Sending KeepAlive~");
		return 0;
	}

/************************************Time setting***********************************/

  	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=0x00000089;
	secondStruct.thirdInt=0x00000001;
	secondStruct.fourthInt=0x06360000;

	//free(tempJson);
  	cJSON *jsonSetTime, *jsonSessionIDSetTime, *jsonOPTimeSettingNoRTCSetTime;
  	char * formatRealTime = now();

	char *tempSetTimeJson = "{ \"Name\" : \"OPTimeSettingNoRTC\", \"OPTimeSettingNoRTC\" : \"20140313 08:03:07\", \"SessionID\" : \"0x89\" }\n";

	//Pay the value to send json
	jsonSetTime= cJSON_Parse(tempSetTimeJson);
	jsonSessionIDSetTime = cJSON_GetObjectItem(jsonSetTime, "SessionID");
	jsonSessionIDSetTime->valuestring=sessionID;
	jsonOPTimeSettingNoRTCSetTime = cJSON_GetObjectItem(jsonSetTime, "OPTimeSettingNoRTC");
	jsonOPTimeSettingNoRTCSetTime->valuestring=formatRealTime;

	char *requestSetTimeString=cJSON_Print(jsonSetTime);

	printf("%s\n",requestSetTimeString);

	//Release json obj
	cJSON_Delete(jsonSetTime);
	cJSON_Delete(jsonSessionIDSetTime);

	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestSetTimeString, sfd)==-1){
		printf("Something has wrong on Sending Time setting~");
		return 0;
	}

	/***************************Send name & session id again*****************************/

	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=0x00000089;
	secondStruct.thirdInt=0x0;
	secondStruct.fourthInt=0x05dc0000;

	//free(tempJson);
	tempDstJson = "{ \"Name\" : \"\", \"SessionID\" : \"0x00000040\" }\n";

	//Pay the value to send json
	jsonDst= cJSON_Parse(tempDstJson);
	jsonSessionIDObjDst = cJSON_GetObjectItem(jsonDst, "SessionID");
	jsonSessionIDObjDst->valuestring=sessionID;

	requestString=cJSON_Print(jsonDst);

	printf("%s\n",requestString);

	//Release json obj
	cJSON_Delete(json);
	cJSON_Delete(jsonDst);

	//send prepared data to ipc
	if(sendSocketData(secondStruct, requestString, sfd)==-1){
		printf("Something has wrong on Sending Name SystemInfo~");
		return 0;
	}

	secondStruct.firstInt=0x000000ff;
	secondStruct.secondInt=0x00000089;
	secondStruct.thirdInt=0x0;
	secondStruct.fourthInt=0x061a0000;
	//send prepared data to ipc
	requestString=0;
	if(sendSocketData(secondStruct, requestString, sfd)==-1){
		printf("Something has wrong on Sending Name SystemInfo~");
		return 0;
	}

  	//free requestString
  	//free(requestString);

  //创建文件
 /* ffd=open(filename, O_RDWR|O_CREAT, 0666);

  float i=0;*/
  ////异常处理
  int i=100;
  //循环接收文件数据
  while(1){
	  r=recv(sfd, buf, 4, MSG_WAITALL);
	  printf("Received package: %08x\n", buf);
	/*  if(r==-1){
		  printf("Receive wrong~%d\n", i);
		  break;
	  }*/
	  i++;
	  //if(i==2)
		  break;
  }
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
  close(sfd);
  printf("Over~");

  return 0;
}
