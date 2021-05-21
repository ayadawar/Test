/*
	Program: Zigbee Bridge Application
	Purpose: Main Zigbee Bridge Application for Communicating between Bottomlayer(Z3GatewayHost) and Toplayer(Core).
	Language:  C
	Libries : cJson,mosquitto
	Author:	Sohail Shareef
	Date: 18th May 2021

How to compile:
	gcc -g main.c cJSON.c -o Zigbee_Bridge_App -lm -lmosquitto
How to Run:
	./Zigbee_Bridge_App
Note: Do not forget to copy cJSON.c and cJSON.h files to your project folder.
*/

// new linegghh

#include <stdio.h>
#include <stdlib.h>

#include <mosquitto.h>
#include <string.h>
#include "cJSON.h"

#define EUI64_STRING_LENGTH 19 // "0x" + 16 characters + NULL
#define GATEWAY_TOPIC_PREFIX_LENGTH 22 // 21 chars `gw/xxxxxxxxxxxxxxxx/` + NULL


#define ZCL_RESPONSE_TOPIC "zclresponse"
#define ZDO_RESPONSE_TOPIC "zdoresponse"
#define APS_RESPONSE_TOPIC "apsresponse"

/* cJSON Types: */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6

//global variables
static char gatewayTopicUriPrefix[GATEWAY_TOPIC_PREFIX_LENGTH]={0}; // /gw/xxxxxxxxxxxxxxxx/
static char gatewayEui64String[EUI64_STRING_LENGTH] = { 0 };// to store eui64
 char devicejoined_topic[50] = { 0 };
char * topic_devicejoined;
//char * topic_devicejoined				 = 	devicejoined_topic;
char * topic_deviceleft 				 = 	"gw/680AE2FFFE5CAC2E/deviceleft";
char * topic_devicestatechange	 = 	"gw/680AE2FFFE5CAC2E/devicestatechange";
char * topic_zclresponse 				 = 	"gw/680AE2FFFE5CAC2E/zclresponse";
char * topic_zdoresponse 				 = 	"gw/680AE2FFFE5CAC2E/zdoresponse";
char * topic_apsresponse 				 = 	"gw/680AE2FFFE5CAC2E/apsresponse";
char * topic_executed						 = 	"gw/680AE2FFFE5CAC2E/executed";
char * topic_devices 						 = 	"gw/680AE2FFFE5CAC2E/devices";
char * topic_relays							 = 	"gw/680AE2FFFE5CAC2E/relays";
char * topic_settings 					 = 	"gw/680AE2FFFE5CAC2E/settings";
char * topic_heartbeat					 = 	"gw/680AE2FFFE5CAC2E/heartbeat";
char * topic_otaevent 					 = 	"gw/680AE2FFFE5CAC2E/otaevent";

char * topic_gw_eui64							= "sample";

void payload_handler(const struct mosquitto_message *msg);
void zclresponse_handler(const struct mosquitto_message *msg);
void apsresponse_handler(const struct mosquitto_message *msg);
void devicejoined_handler(const struct mosquitto_message *msg);
void deviceleft_handler(const struct mosquitto_message *msg);
void gw_eui64_handler(const struct mosquitto_message *msg);

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
	//printf("ID: %d\n", * (int *) obj);
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "sample", 0);/*
	mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/devices", 			0);
	mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/relays", 			0);
	mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/settings", 		0);*/
	mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/devicejoined", 0);
	mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/deviceleft", 	0);
	/*mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/devicestatechange",0);
	mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/otaevent", 		0);
	mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/executed", 		0);*/
	//mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/zclresponse", 	0);
	//mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/zdoresponse", 	0);
	//mosquitto_subscribe(mosq, NULL, "gw/680AE2FFFE5CAC2E/apsresponse", 	0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {

	//printf("\n\nNew message with topic %s:\n%s\n", msg->topic, (char *) msg->payload);
	payload_handler(msg);
}

//payload handler

void payload_handler(const struct mosquitto_message *msg){


	if(strcmp(msg->topic , topic_gw_eui64)==0){
			printf("\nforwarding to gw_eui64_handler..\n");
			gw_eui64_handler(msg);
	}
	else if(strcmp(msg->topic , topic_zclresponse)==0){
			printf("\nforwarding to zclresponse_handler..\n\n");
			zclresponse_handler(msg);
	}
	else if(strcmp(msg->topic , topic_apsresponse)==0){
			printf("\nforwarding to apsresponse_handler..\n\n");
			apsresponse_handler(msg);
	}
	else if(strcmp(msg->topic , devicejoined_topic)==0){
			printf("\nforwarding to devicejoined_handler..\n\n");
			devicejoined_handler(msg);
	}
	else if(strcmp(msg->topic , topic_deviceleft)==0){
			printf("\nforwarding to deviceleft_handler..\n\n");
			deviceleft_handler(msg);
	}
	else{
		printf("no topic\n");
	}
}

void gw_eui64_handler(const struct mosquitto_message *msg){
char * topic_devicejoined;
	cJSON*root = cJSON_Parse(msg->payload);
	if(root){
					printf("gw_eui64_handler\n");
					//printf("eui64 = %s\n",cJSON_GetObjectItem(root,"eui64")->valuestring);

					strcpy(gatewayEui64String,cJSON_GetObjectItem(root,"eui64")->valuestring);
					strcat(gatewayTopicUriPrefix, "gw/");
					strcat(gatewayTopicUriPrefix, gatewayEui64String);
					strcat(gatewayTopicUriPrefix, "/");
					printf("HA Gateweay EUI64 = %s\n",gatewayEui64String);

					printf("Zigbee Gateway topic prefix = %s\n",gatewayTopicUriPrefix);

					strcat(devicejoined_topic,gatewayTopicUriPrefix);
printf("topic_devicejoined00000000000 = %s\n",devicejoined_topic);

					strcat(gatewayTopicUriPrefix,"devicejoined");
printf("Zigbee Gateway topic prefix222222222222222222 = %s\n",gatewayTopicUriPrefix);
					strcat(devicejoined_topic, "devicejoined");
					topic_devicejoined = devicejoined_topic;

					strcat(devicejoined_topic, "deviceleft");
					topic_devicejoined = devicejoined_topic;

					strcat(devicejoined_topic, "devicejoined");
					topic_devicejoined = devicejoined_topic;

					strcat(devicejoined_topic, "devicejoined");
					topic_devicejoined = devicejoined_topic;

				}
					topic_devicejoined = devicejoined_topic;
					printf("topic_devicejoined0 = %s\n",devicejoined_topic);
					printf("topic_devicejoined = %s\n",topic_devicejoined);

}

//individual handlers
void devicejoined_handler(const struct mosquitto_message *msg){

	cJSON*root = cJSON_Parse(msg->payload);
	if(root){
				printf("devicejoined_handler\n");
				//char*p = cJSON_Print(root);
				//printf("%s\n",p);

				printf("nodeId: %s\n",cJSON_GetObjectItem(root,"nodeId")->valuestring);
				printf("deviceState: %d\n",cJSON_GetObjectItem(root,"deviceState")->valueint);
				printf("deviceType:%s\n",cJSON_GetObjectItem(root,"deviceType")->valuestring);
				printf("timeSinceLastMessage: %d\n",cJSON_GetObjectItem(root,"timeSinceLastMessage")->valueint);

				if(cJSON_Object == cJSON_GetObjectItem(root,"deviceEndpoint")->type)//If it is an object
				{
						printf("It is a object!!\n");

						cJSON* root1 = cJSON_GetObjectItem(root,"deviceEndpoint");//Get object
						printf("eui64 = %s\n",cJSON_GetObjectItem(root1,"eui64")->valuestring);
						printf("endpoint = %d\n",cJSON_GetObjectItem(root1,"endpoint")->valueint);

						if(cJSON_Array == cJSON_GetObjectItem(root1,"clusterInfo")->type)//If it is an array
						{
								printf("It is a Array!!\n");
								cJSON* root2 = cJSON_GetObjectItem(root1,"clusterInfo");//Get the array
								cJSON* value = root2->child;//Get the value of the array
								int size = cJSON_GetArraySize(root2);//Get the size of the array
								printf("size = %d\n",size);
								int i = 0;
								for(i=0; i<size; i++)
									{
										//printf("table[%d]=%lf\n",i,value->valuedouble);
										cJSON* array = cJSON_GetArrayItem(root2, i);

										printf("ClusterID..: %s  | ",cJSON_GetObjectItem(array,"clusterId")->valuestring );
										printf("ClusterType..: %s\n\n",cJSON_GetObjectItem(array,"clusterType")->valuestring);
										value = value->next;//the next value of the array
									}
								//cJSON_Delete(root2);//Same as above
								value = NULL;
								root2 = NULL;
								}
								else
								printf("It is not a Array!!\n");

						//cJSON_Delete(root1);//There is no need to release memory here, root1 is a pointer to the memory requested by root
						root1 = NULL;

				}else
					printf("It is not a object!!\n");
			}
			cJSON_Delete(root);//Be sure to remember to release the memory! ! ! ! ! !
		root = NULL;
}

void deviceleft_handler(const struct mosquitto_message *msg){

	cJSON*root = cJSON_Parse(msg->payload);
	if(root){
				printf("deviceleft_handler\n");
				//char*p = cJSON_Print(root);
				//printf("%s\n",p);

				printf("eui64: %s\n",cJSON_GetObjectItem(root,"eui64")->valuestring);
				cJSON_Delete(root);//Be sure to remember to release the memory! ! ! ! ! !
				root = NULL;
		}
}
void zclresponse_handler(const struct mosquitto_message *msg){

	cJSON*root = cJSON_Parse(msg->payload);
	if(root){
				printf("zclresponse_handler\n");

				char*p                  = cJSON_Print(root);
				cJSON*mfgCode           = NULL;
				cJSON*commandId         = NULL;
				cJSON*commandData       = NULL;

				cJSON* clusterSpecific  = NULL;
				cJSON*attributeId       = NULL;
				cJSON*attributeBuffer   = NULL;
				cJSON*attributeDataType = NULL;

				//printf("%s\n",p);

				printf("clusterId: %s\n",cJSON_GetObjectItem(root,"clusterId")->valuestring);

				commandId = cJSON_GetObjectItem(root,"commandId");
				if(commandId != NULL){
					printf("commandId: %s\n",commandId->valuestring);
				}

				commandData = cJSON_GetObjectItem(root,"commandData");
				if(commandData!=NULL){
					printf("\n\ncommandData:%s\n\n",commandData->valuestring);
				}

				clusterSpecific = cJSON_GetObjectItem(root,"clusterSpecific");
				if(clusterSpecific !=NULL){
					printf("clusterSpecific: %d\n",cJSON_GetObjectItem(root,"clusterSpecific")->type);
				}

				mfgCode = cJSON_GetObjectItem(root,"mfgCode");
				if ( cJSON_GetObjectItem(root,"mfgCode") !=NULL){
					printf("mfgCode: %s\n",mfgCode->valuestring);
				}

				attributeId = cJSON_GetObjectItem(root,"attributeId");
				if(attributeId !=NULL){
					printf("attributeId: %s\n",attributeId->valuestring);
				}

				attributeBuffer = cJSON_GetObjectItem(root,"attributeBuffer");
				if(attributeId !=NULL){
					printf("attributeBuffer: %s\n",attributeBuffer->valuestring);
				}

				attributeDataType = cJSON_GetObjectItem(root,"attributeDataType");
				if(attributeDataType !=NULL){
						printf("attributeDataType: %s\n",attributeDataType->valuestring);
				}

				if(cJSON_Object == cJSON_GetObjectItem(root,"deviceEndpoint")->type)//If it is an object
				{
						printf("It is a nested object!!\n");

						cJSON* root1 = cJSON_GetObjectItem(root,"deviceEndpoint");//Get object
						printf("eui64 = %s\n",cJSON_GetObjectItem(root1,"eui64")->valuestring);
						printf("endpoint = %d\n",cJSON_GetObjectItem(root1,"endpoint")->valueint);

						//cJSON_Delete(root1);//There is no need to release memory here, root1 is a pointer to the memory requested by root
						root1 = NULL;
				}else
					printf("It is not a netsted object!!\n");
		cJSON_Delete(root);//Be sure to remember to release the memory! ! ! ! ! !
		root = NULL;
	}
}

void apsresponse_handler(const struct mosquitto_message *msg){

	cJSON*root     = cJSON_Parse(msg->payload);

	if(root){
				//char*p = cJSON_Print(root);
				//printf("%s\n",p);

				printf("statusType: %s\n",cJSON_GetObjectItem(root,"statusType")->valuestring);
				printf("eui64: %s\n",cJSON_GetObjectItem(root,"eui64")->valuestring);
				printf("status:%s\n",cJSON_GetObjectItem(root,"status")->valuestring);
				printf("clusterId: %s\n",cJSON_GetObjectItem(root,"clusterId")->valuestring);
				printf("commandId: %s\n",cJSON_GetObjectItem(root,"commandId")->valuestring);

	cJSON_Delete(root);//Be sure to remember to release the memory! ! ! ! ! !
	root         = NULL;
	}
}

int main() {

	printf("\t\t||||||||||||||||||||||||||||||||||||||||||\n");
	printf("\t\t||					||\n");
	printf("\t\t||	  Zigbee Bridge Application	||\n");
	printf("\t\t||		  Hogar			||\n");
	printf("\t\t||	       Version:0.1		||\n");
	printf("\t\t||					||\n");
	printf("\t\t||||||||||||||||||||||||||||||||||||||||||\n");

	int rc, id=12;

	mosquitto_lib_init();

	struct mosquitto *mosq;

	mosq = mosquitto_new("subscribe-test", true, &id);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);

	rc = mosquitto_connect(mosq, "localhost", 1883, 10);
	if(rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}

	mosquitto_loop_start(mosq);

	printf("\n\n\t   Application is Running\t\tPress Enter to quit...\n");

	getchar();
	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return 0;
}
