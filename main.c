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

int main(){
  //connect handler; vsfd is used to receive video data
  int sfd, ffd, vsfd;
  //r1 is used to receive video data
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
  	unsigned char buffSend[20];


  	char tempJson[] = "{ \"EncryptType\" : \"MD5\", \"LoginType\" : \"DVRIPWeb\", \"PassWord\" : \"6QNMIQGe\", \"UserName\" : \"admin\" }\n";
  	//json string length
  	testStruct.jsonSize=strlen(tempJson);
  	getSendDataInBinary(buffSend, testStruct);
  	//send head
  	r=send(sfd, &buffSend, 20, 0);
  	//send json parameters
  	r=send(sfd, &tempJson, testStruct.jsonSize, 0);

  //接收文件
  struct tcpRequire returnStruct;

  //unsigned char returnedBuff[20];
  r=recv(sfd, &returnStruct.firstInt ,4, MSG_WAITALL);
  r=recv(sfd, &returnStruct.secondInt ,4, MSG_WAITALL);
  r=recv(sfd, &returnStruct.thirdInt ,4, MSG_WAITALL);
  r=recv(sfd, &returnStruct.fourthInt ,4, MSG_WAITALL);
  r=recv(sfd, &returnStruct.jsonSize ,4, MSG_WAITALL);
  unsigned char buff[128];
  //printf("sizeof char=%08x;%08x;%08x;%08x;%08x;\n", returnStruct.firstInt,returnStruct.secondInt,returnStruct.thirdInt,returnStruct.fourthInt,returnStruct.jsonSize);

  r=recv(sfd, buff ,returnStruct.jsonSize, MSG_WAITALL);
  printf("firstInt is %08x;\n", returnStruct.firstInt);
  printf("secondInt is %08x;\n", returnStruct.secondInt);

  printf("size of returned data: %d\n",returnStruct.jsonSize);
  int countBuff;
  //for(countBuff=0; countBuff<128; countBuff++)
  printf("Received %d string is : %s;\n", countBuff, buff);


  //r=connect(sfd, (struct sockaddr*)&dr, sizeof(dr));

  //prepare struct data
	struct tcpRequire secondStruct;
	secondStruct.firstInt=0x000000ff;
	//r=send(sfd, &testStruct.firstInt, 4, 0);
	secondStruct.secondInt=0x00000040;
	//r=send(sfd, &testStruct.secondInt, 4, 0);
	secondStruct.thirdInt=0x0;
	//r=send(sfd, &testStruct.thirdInt, 4, 0);
	secondStruct.fourthInt=0x03fc0000;
	//r=send(sfd, &testStruct.fourthInt, 4, 0);

	// Get SessionID
	cJSON *json, *jsonSessionIDObj;
	json=cJSON_Parse(tempJson);
	jsonSessionIDObj = cJSON_GetObjectItem(json, "SessionID");
	char *sessionID = jsonSessionIDObj->valuestring;
	cJSON_Delete(jsonSessionIDObj);
	cJSON_Delete(json);

	free(tempJson);
  	*tempJson = "{ \"Name\" : \"SystemInfo\", \"SessionID\" : \"0x00000040\" }\n";

  	//json string length
  	secondStruct.jsonSize=strlen(tempJson);
  	getSendDataInBinary(buffSend, secondStruct);
  	//send head
  	r=send(sfd, &buffSend, 20, 0);
  	//send json parameters
  	r=send(sfd, &tempJson, testStruct.jsonSize, 0);


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
