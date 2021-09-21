/*
 * \file SHNFnetwork.h
 * Network specific functionality
 * \addtogroup Demo
 * \addtogroup UDPSHNF
 * \{
 * \brief SHNF - Safety "Firmware close to the hardware"
 *
 * SHNF, unit to simulate the interface to firmware driver.
 * For test purposes this unit provides all necessary
 * interfaces between EPLsafety Stack and firmware driver
 * ("Firmware close to the hardware").
 *
 ******************************************************************************
 * Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 *                      IXXAT Automation GmbH
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 *******************************************************************************
 *
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \author M.Molnar, IXXAT Automation GmbH (original Demo application)
 *
 */

#ifndef linux
#pragma warning (disable : 4115) /* This pragma is used to avoid a compiler
                                    warning (warning C4115: _RPC_ASYNC_STATE
                                    named type definition in parentheses) in
                                    rpcasync.h windows header */
#endif

#include "MQTT.h"

#include "MQTTAsync.h"
#include "base64.h"

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLStarget.h"

#include <SHNF.h>
#include "MQTT.h"
#include "UDP.h"
#include "SHNF_Definitions.h"
#include "MQTT_Configuration.h"
#include "datalogger.h"

#include "Time.h"
#include "MessageHandler.h"
#include "MQTTBufferHandling.h"

#define k_MAX_DATA_LEN 254

/*MQTT Stuff*/
MQTTAsync client;
volatile MQTTAsync_token deliveredtoken;
int disc_finished = 0;
int subscribed = 0;
int finished = 0;


UINT8 base64enBuff[k_MAX_DATA_LEN* 2]; //size is big enough. just to be sure. Could use b64e_size function
UINT8 base64deBuff[k_MAX_DATA_LEN]; //Base64 Decoding buffer

void mqttSend(const UINT8 * c_sendBuffer, UINT32 i_sendBufferLength){

	//encode message as base64
	if (i_sendBufferLength > k_MAX_DATA_LEN){
		DATA_LOGGER("FATAL ERROR MQTT DATA TOO LONG");
		return;
	}

	//send switched Frame
	UINT8 * pb_sendMemBlock = switchFrames( i_sendBufferLength, c_sendBuffer ); //switch frames
	//UINT32 base64enSize2 = b64_encode(pb_sendMemBlock, i_sendBufferLength, base64enBuff);
	//mqttSendMessageSw(client, base64enBuff, base64enSize2);
	//mqttSendMessageSw(client, c_sendBuffer, i_sendBufferLength);

	//send normal Frame
	//UINT32 base64enSize = b64_encode(c_sendBuffer, i_sendBufferLength, base64enBuff);
	//mqttSendMessage(client, base64enBuff, base64enSize);
	mqttSendMessage(client, c_sendBuffer, i_sendBufferLength); //without base64
}



//-------------MQTT-Functions------------------------------------
int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    int i;
    char* payloadptr;

//    DATA_LOGGER("Message arrived\n");
//    DATA_LOGGER1("     topic: %s\n", topicName);
//    DATA_LOGGER("   message: ");
//
//    payloadptr = message->payload;
//    //payloadlen = message->payloadlen;
//    for(i=0; i<message->payloadlen; i++)
//    {
//        putchar(*payloadptr++);
//    }
//    putchar('\n');


    /* The received data is written into the buffer */
   if ( message->payloadlen > 0 )
   {
	   //base64decode

	   //UINT16 base64deSize = b64_decode(message->payload, message->payloadlen, base64deBuff);
	   //UINT8 * pb_ret = (UINT8 *) switchFrames(base64deBuff, base64deSize);
	   /** Insert packages into queue */
	   //InsertIntoRXBuffer ( base64deBuff, (UINT16) base64deSize);
	   //InsertIntoRXBuffer ( pb_ret, (UINT16) base64deSize); //switch frames back
	   InsertIntoRXBuffer ( message->payload, (UINT16) message->payloadlen); //without base64
   }
   MQTTAsync_freeMessage(&message);
   MQTTAsync_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;

	DATA_LOGGER("\nConnection lost\n");
	if (cause)
		DATA_LOGGER1("     cause: %s\n", cause);

	DATA_LOGGER("Reconnecting\n");
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		DATA_LOGGER1("Failed to start connect, return code %d\n", rc);
		finished = 1;
	}
}

void onDisconnect(void* context, MQTTAsync_successData* response)
{
	DATA_LOGGER("Successful disconnection\n");
	disc_finished = 1;
}


void onSubscribe(void* context, MQTTAsync_successData* response)
{
	DATA_LOGGER("Subscribe succeeded\n");
	subscribed = 1;
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	DATA_LOGGER1("Subscribe failed, rc %d\n", response ? response->code : 0);
	finished = 1;
}


void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	DATA_LOGGER1("Connect failed, rc %d\n", response ? response->code : 0);
	finished = 1;
}


void onConnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc;

	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	pubmsg.payload = "SCM is online";
	pubmsg.payloadlen = (int)strlen("SN is online");
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;

	DATA_LOGGER("Successful connection\n");

	DATA_LOGGER3("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", k_MQTT_SUBSCRIBE_TOPIC, CLIENTID, QOS);
	opts.onSuccess = onSubscribe;
	opts.onFailure = onSubscribeFailure;
	opts.context = client;

	deliveredtoken = 0;

	if ((rc = MQTTAsync_subscribe(client, k_MQTT_SUBSCRIBE_TOPIC, QOS, &opts)) != MQTTASYNC_SUCCESS)
	{
		DATA_LOGGER1("Failed to start subscribe, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	if ((rc = MQTTAsync_sendMessage(client, k_MQTT_BASE_TOPIC, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
		{
			printf("Failed to start sendMessage, return code %d\n", rc);
			exit(EXIT_FAILURE);
		}
}

void mqttSendMessage(void* context, const UINT8 * c_sendBuffer, UINT32 i_sendBufferLength){
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	pubmsg.payload = c_sendBuffer;
	pubmsg.payloadlen = i_sendBufferLength;
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;
	int rc;

	//opts.onSuccess = onSend;
	opts.context = client;

	if ((rc = MQTTAsync_sendMessage(client, k_MQTT_BASE_TOPIC, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start sendMessage, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}

void mqttSendMessageSw(void* context, const UINT8 * c_sendBuffer, UINT32 i_sendBufferLength){
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	pubmsg.payload = c_sendBuffer;
	pubmsg.payloadlen = i_sendBufferLength;
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;
	int rc;

	//opts.onSuccess = onSend;
	opts.context = client;

	if ((rc = MQTTAsync_sendMessage(client, k_MQTT_BASE_TOPIC_SW, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start sendMessage, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}

/**
 * This function starts the thread, which will receive data from the network.
 * This function relies heavily on cross-compilation macros. The real code is in
 * \file CrossCompile_Linux.h or \file CrossCompile_Win32.h , so go look there for
 * - \see CREATE_THREAD
 * - \see THREAD_FAILURE
 * - \see THREAD_TYPE
 *
 * \return - TRUE on success
 *         - FALSE on failure
 */
BOOLEAN StartMqttClient()
{

	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
	int rc;
	int ch;



	MQTTAsync_create(&client, k_MQTT_BROKER_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	MQTTAsync_setCallbacks(client, client, connlost, msgarrvd, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.automaticReconnect = 1; //turn on automatic reconnection
	conn_opts.minRetryInterval = 1; //try to reconnect after 1s
	conn_opts.maxRetryInterval =60; //doubles recconnect-intervall at each fail till max RetryInterval
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		DATA_LOGGER1("Failed to start connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
	while(!subscribed){
		usleep(10000L);
	}
	DATA_LOGGER("Subscribing done\n");



    return finished;
}
