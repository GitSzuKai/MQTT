#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
		
	pubmsg.payload = "Hello a";
	pubmsg.payloadlen = strlen("Hello a");
	pubmsg.qos = 1;
	pubmsg.retained = 0;


	/* Connection parameters */
	MQTTClient_create(&client, "tcp://localhost:1883", "Abb", MQTTCLIENT_PERSISTENCE_NONE, NULL);
	int rc;

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1; 

	/* Connect to MQTT Server */
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
		printf("Failed\n");
	} else{
		printf("Connected\n");
	}

	MQTTClient_publish(client,"/example",pubmsg.payloadlen,pubmsg.payload,pubmsg.qos,pubmsg.retained,NULL);

	printf("Message published\n");

	return 0; 
}