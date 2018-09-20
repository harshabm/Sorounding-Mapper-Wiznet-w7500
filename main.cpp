#include "mbed.h"
#include "WizFi310Interface.h"
#include "MQTTClient.h"
#include "MQTTmbed.h"
#include "MQTTSocket.h"
#include "hcsr04.h"
#include "Servo.h"


#define SECURE WizFi310::SEC_WPA2_MIXED
#define SSID "CDI"
#define PASS "Cdi*1717"
/*
    SET DHCP
*/
#define USE_DHCP    1
//--------- Have to modify the mac address-------------
unsigned char MAC_Addr[6] = {0x00,0x08,0xDC,0x12,0x34,0x56};
#if defined(TARGET_WIZwiki_W7500)
WizFi310Interface wizfi310(D1, D0, D7, D6, D8, NC, 115200);
Serial pc(USBTX, USBRX);
#endif
HCSR04 sensor(A1,A0); // Trigger , Echo
HCSR04 sensor1(A3,A2); // Trigger , Echo
HCSR04 sensor2(A5,A4); // Trigger , Echo
HCSR04 sensor3(D14,D15); // Trigger , Echo

int j=0; 
Servo myservo(D3);
DigitalIn button(D4);
int distance1[100];
int distance2[100];
int distance3[100];
int distance4[100];
int arrivedcount = 0;
void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n", message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", message.payloadlen, (char*)message.payload);
    ++arrivedcount;
}

class MQTTWIZ: public MQTTSocket
{
public:
    MQTTWIZ() {
        wait(1);
        this->createSocket();
    }
};

int main()
{
    pc.baud(9600);

    wait(1.0); // 1 sec  Admin
  //  printf("WizFi310  STATION. \r\n");
    wizfi310.init();
 //  printf("After Initialisation. \r\n");

  //  printf("After Set Address. \r\n");
    if ( wizfi310.connect(SECURE, SSID, PASS, WizFi310::WM_STATION))      return -1;
 //   printf("After Connect. \r\n");
   // printf("IP Address is %s\r\n", wizfi310.getIPAddress());

    MQTTWIZ ipstack = MQTTWIZ();
    MQTT::Client<MQTTWIZ, Countdown> client = MQTT::Client<MQTTWIZ, Countdown>(ipstack);

    char* hostname = "172.16.73.4";
    int port = 1883;

    int rc = ipstack.connect(hostname, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\n", rc);
 //   printf("rc from TCP connect is %d\n", rc);

    char MQTTClientID[30];

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    sprintf(MQTTClientID,"WIZwiki-W7500-client-%d",rand()%1000);
    data.clientID.cstring = MQTTClientID;
    data.username.cstring = "testuser";
    data.password.cstring = "testpassword";

    if ((rc = client.connect(data)) != 0)
     //   pc.printf("rc from MQTT connect is %d\n", rc);

 myservo.calibrate(0.0005, 170);
    
    
while (true) {       
    if(button==1){  
        char buf[100];
        MQTT::Message message;
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;     
    for(int i=0; i<100; i++) {
        myservo = i/100.0;
          distance1[i] = sensor.distance();
          distance2[i] = sensor1.distance();
          distance3[i] = sensor2.distance();
          distance4[i] = sensor3.distance();  
        // pc.printf("count  %d  \n",i);   
        wait(0.01); 
            }
    for(int i=100; i>0; i--) {
        myservo = i/100.0;
        wait(0.01);
            }
     j++;
     pc.printf("number of times mapped %d \n",j);
     sprintf(buf, "%ld", j);
     message.payload = (void*)buf;
     message.payloadlen = strlen(buf);
     rc = client.publish("Number of times mapped", message);
          
     for(int i=0;i<91;i++){
  
             sprintf(buf, "%ld", distance1[i]);
             pc.printf(" The distance is : %s\n",buf);
             message.payload = (void*)buf;
             message.payloadlen = strlen(buf);
             rc = client.publish("0 to 90 degree distance", message);
             pc.printf("0 to 90 degree distance: %s\r\n",message.payload);
       
             sprintf(buf, "%ld", distance2[i]);
   //        pc.printf(" The disstance is : %ld\n",distance1);
             pc.printf(" The distance is : %s\n",buf);
             message.payload = (void*)buf;
             message.payloadlen = strlen(buf);
             rc = client.publish("90 to 180 degree distance", message);
             pc.printf("90 to 180 degree distance : %s\r\n",message.payload);
             
               sprintf(buf, "%ld", distance3[i]);
             pc.printf(" The distance is : %s\n",buf);
             message.payload = (void*)buf;
             message.payloadlen = strlen(buf);
             rc = client.publish("180 to 270 degree distance", message);
             pc.printf("180 to 270 degree distance: %s\r\n",message.payload);
             
               sprintf(buf, "%ld", distance4[i]);
             pc.printf(" The distance is : %s\n",buf);
             message.payload = (void*)buf;
             message.payloadlen = strlen(buf);
             rc = client.publish("270 to 360 degree distance", message);
             pc.printf("270 to 360 degree distance: %s\r\n",message.payload);
             
             client.yield(1000);
             wait(0.05);
          }
    }              
   }
}