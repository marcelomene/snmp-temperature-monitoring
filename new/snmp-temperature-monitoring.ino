#include <ESP8266WiFi.h> /* Wi-fi library */
#include <Agentuino.h>

/* Wi-fi */
const char *ssid = "CYBERNET MARCELO";
const char *pass = "90054730";

/* SNMP Agent */
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;
char oid[SNMP_MAX_OID_LEN];
char *community = "public";
char *description = "SNMP Temperature Mon";
char *contact = "marcelo.jorda@hotmail.com";
char *location = "FADERGS";
char *systemName = "NodeMCU_ESP8266";

/* SNMP OIDs*/
const char *sysDescr    = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString)
const char *sysUpTime   = "1.3.6.1.2.1.1.3.0";  // read-only  (TimeTicks)
const char *sysContact  = "1.3.6.1.2.1.1.4.0";  // read-write (DisplayString)
const char *sysName     = "1.3.6.1.2.1.1.5.0";  // read-write (DisplayString)
const char *sysLocation = "1.3.6.1.2.1.1.6.0";  // read-write (DisplayString)
const char *sysServices = "1.3.6.1.2.1.1.7.0";  // read-only  (Integer)

/* Utils */
uint32_t locUpTime = 0;
uint32_t prevMillis = millis();

void pduReceivedCallback()
{
  SNMP_PDU pdu;
  api_status = Agentuino.requestPdu(&pdu);

  if(pdu.type == SNMP_PDU_GET && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS)
  {
     pdu.OID.toString(oid);
     if ( strcmp( oid, sysUpTime ) == 0 ) 
     {
       Serial.println("Received sysUpTime GET");
       if ( pdu.type == SNMP_PDU_SET ) 
       {
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = SNMP_ERR_READ_ONLY;//sysUpTime is read-only
       } 
       else 
       {
          status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);       
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = status;
       }
    }
    else
    {
      // oid does not exist
      // response packet - object not found
      pdu.type = SNMP_PDU_RESPONSE;
      pdu.error = SNMP_ERR_NO_SUCH_NAME;
    }
     if(Agentuino.responsePdu(&pdu) == SNMP_API_STAT_SUCCESS)
        Serial.println("Response sent!");
     //Agentuino.freePdu(&pdu); 
  }
}

void setup() 
{
  Serial.begin(9600);
  
  pinMode(D1,OUTPUT);
  
  Serial.println("Starting Wi-Fi connection...");
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!"); 
  Serial.println("Current IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting SNMP agent...");
  api_status = Agentuino.begin();
  if ( api_status == SNMP_API_STAT_SUCCESS ) 
  {
    Agentuino.onPduReceive(pduReceivedCallback);
    Serial.println("SNMP Agent is runnning.");
    Serial.println("PDU received callback configured.");
    Serial.println("Listening for GET requests...");
    delay(10);
    return;
  }
}

void loop() 
{
  // listen/handle for incoming SNMP requests
   digitalWrite(D1,HIGH);
   delay(100);
   Agentuino.listen();
  
   if ( millis() - prevMillis > 1000 ) 
   {
    prevMillis += 1000;
    locUpTime += 100;
   }
   digitalWrite(D1,LOW);
   delay(100);
}
