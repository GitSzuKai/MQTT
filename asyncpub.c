#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
volatile MQTTClient_deliveryToken deliveredtoken;


int flag = 1; 
void connlost(void *context, char *cause){
	printf("Connection Lost\n");
	printf("Reason: %s\n",cause);
}

/* Note this function will only be called when QOS is 1 */
void delivered(void *context, MQTTClient_deliveryToken dt){
	printf("Message Successfully Delivered\n");
	printf("The token is %d\n", dt);
	flag = 0; 
	deliveredtoken = dt; 
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");

    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
  }

int main(int argc, char const *argv[])
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	pubmsg.payload = "Hello async";
	pubmsg.payloadlen = (int)strlen("Hello async");
	pubmsg.qos = 1;
	pubmsg.retained = 0;
	//deliveredtoken = 0 ; 

	/* Connection parameters */
	MQTTClient_create(&client, "tcp://localhost:1883", "Async", MQTTCLIENT_PERSISTENCE_NONE, NULL);
	
	int rc;
    
    //設置觸發function 要在connect前設置
    /*
    *  這裡有個bug 這個function 要求msgarrvd 一定要存在否則return error 
	*  msgarrvd 提供后delivered 才能使用
    */
	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	
	/* Connect to MQTT Server */
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
		printf("Failed\n");
	} else{
		printf("Connected\n");
	}

	MQTTClient_publishMessage(client,"/example", &pubmsg, &token);
	//int second_publish = MQTTClient_publish(client,"/example",pubmsg.payloadlen,pubmsg.payload,pubmsg.qos,pubmsg.retained,&token);
	printf("Message published\n");

	while(deliveredtoken != token)
	//while(flag != 0)	
		;

	//結束連線
   	MQTTClient_disconnect(client, 10000);
   	//清理client object
    MQTTClient_destroy(&client);
	
	return 0;
}