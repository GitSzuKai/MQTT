#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>


int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message){
 	
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

    //需要把memory free 掉。Note 兩個free function 都需使用
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}


int main(){

	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	
	/* Connection parameters */
	MQTTClient_create(&client, "tcp://localhost:1883", "A", MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1; 

	//設置觸發function 要在connect前設置
	MQTTClient_setCallbacks(client, NULL, NULL, msgarrvd, NULL);

	int rc;
	int ch;

	/* Connect to MQTT Server */
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
		printf("Failed\n");
	} else{
		printf("Connected\n");
	}

	MQTTClient_subscribe(client, "/example", 1);
	printf("Successfully Subscribed to the topic\n");

	 do 
    {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');

    //結束連線
    MQTTClient_disconnect(client, 10000);
    //清理client object
    MQTTClient_destroy(&client);
	return rc;
}