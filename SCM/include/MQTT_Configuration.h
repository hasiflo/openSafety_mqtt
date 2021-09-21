/**
 * \file UDP_SCM/UDP_Configuration.h
 */


#include "UDP.h"

/** The port the system will receive data on */
#define k_MQTT_SERVICE_PORT       1883 /* UDP port */

/** The target network */
#define k_MQTT_BROKER_ADDRESS     "tcp://localhost:1883"//"tcp://5.189.178.9:1883"

#define k_MQTT_BASE_TOPIC		  "openSafety/scm"
#define k_MQTT_BASE_TOPIC_SW		  "openSafety/scmsw"
#define k_MQTT_SUBSCRIBE_TOPIC		  "openSafety/scm"


#define CLIENTID    "OpenSafetySCM"
#define QOS         0
#define TIMEOUT     10000L



