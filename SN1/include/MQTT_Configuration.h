/**
 * \file UDP_SCM/UDP_Configuration.h
 */


#include "UDP.h"

/** The port the system will receive data on */
#define k_MQTT_SERVICE_PORT       1883 /* UDP port */

/** The target network */
#define k_MQTT_BROKER_ADDRESS     "tcp://192.168.0.21:1883"

#define k_MQTT_BASE_TOPIC		  "openSafety/scm"
#define k_MQTT_BASE_TOPIC_SW		  "openSafety/scmsw"
#define k_MQTT_SUBSCRIBE_TOPIC		  "openSafety/scm"


#define CLIENTID    "OpenSafetySN"
#define QOS         0
#define TIMEOUT     10000L



