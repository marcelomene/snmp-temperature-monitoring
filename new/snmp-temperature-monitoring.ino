#include <ESP8266WiFi.h> /* Wi-fi library */
#include <Agentuino.h> /* Modified Agentuino library */
#include <DHT.h>

/* DHT11 */
/* DHT 11 CONNECTION */
/* VCC -> VUSB */
/* GND -> GND (NEXT TO VUSB) */
/* DATA -> D3 (GPIO0) */

#define DHTPIN 0 // 
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

/* Wi-fi */
const char *ssid = "FADERGS";
const char *pass = "";

/* SNMP Agent stuff */
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;
char oid[SNMP_MAX_OID_LEN];
char *community = "public";
char *description = "SNMP Temperature Mon";
char *contact = "marcelo.jorda@hotmail.com";
char *location = "FADERGS";
char *systemName = "NodeMCU_ESP8266";
int temperature = 0;
int humidity = 0;

/* SNMP OIDs*/
const char *sysDescr       = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString) Done!
const char *sysUpTime      = "1.3.6.1.2.1.1.2.0";  // read-only  (TimeTicks)     Done!
const char *sysName        = "1.3.6.1.2.1.1.3.0";  // read-write (DisplayString) Done!
const char *sysLocation    = "1.3.6.1.2.1.1.4.0";  // read-write (DisplayString) Done!
const char *sysTemperature = "1.3.6.1.2.1.1.5.0";  // read-only  (DisplayString) Done!
const char *sysHumidity    = "1.3.6.1.2.1.1.6.0";  // read-only  (DisplayString) Done!

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
     Serial.println(oid);
     if ( strcmp( oid, sysUpTime ) == 0 )  /* Handles sysUpTime request */
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
    else if ( strcmp( oid, sysTemperature ) == 0 ) /* Handles sysTemperature request */
    {
       Serial.println("Received sysTemperature GET");
       if ( pdu.type == SNMP_PDU_SET ) 
       {
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = SNMP_ERR_READ_ONLY;//sysTemperature is read-only
       } 
       else 
       {
          status = pdu.VALUE.encode(SNMP_SYNTAX_INT, temperature);       
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = status;
       }
    }
    else if ( strcmp( oid, sysHumidity ) == 0 ) /* Handles sysHumidity request */
    {
       Serial.println("Received sysHumidity GET");
       if ( pdu.type == SNMP_PDU_SET ) 
       {
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = SNMP_ERR_READ_ONLY;//sysTemperature is read-only
       } 
       else 
       {
          status = pdu.VALUE.encode(SNMP_SYNTAX_INT, humidity);       
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = status;
       }
    }
    else if ( strcmp( oid, sysDescr ) == 0 ) /* Handles sysDescridity request */
    {
       Serial.println("Received sysDescr GET");
       if ( pdu.type == SNMP_PDU_SET ) 
       {
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = SNMP_ERR_READ_ONLY;//sysDescr is read-only
       } 
       else 
       {
          status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, description);       
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = status;
       }
    }
    else if ( strcmp( oid, sysName ) == 0 ) /* Handles sysName request */
    {
       Serial.println("Received sysName GET");
       if ( pdu.type == SNMP_PDU_SET ) 
       {
          // response packet from set-request - object is read/write
          status = pdu.VALUE.decode(systemName, strlen(systemName)); 
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = status;
       } 
       else 
       {
          status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, systemName);       
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = status;
       }
    }
    else if ( strcmp( oid, sysLocation ) == 0 ) /* Handles sysLocation request */
    {
       Serial.println("Received sysLocation GET");
       if ( pdu.type == SNMP_PDU_SET ) 
       {
          // response packet from set-request - object is read/write
          status = pdu.VALUE.decode(location, strlen(location)); 
          pdu.type = SNMP_PDU_RESPONSE;
          pdu.error = status;
       } 
       else 
       {
          status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, location);       
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

  Serial.println("Starting DHT11...");
  dht.begin();
  
  Serial.println("Starting SNMP agent...");
  api_status = Agentuino.begin();
  if ( api_status == SNMP_API_STAT_SUCCESS ) 
  {
    Agentuino.onPduReceive(pduReceivedCallback);
    Serial.println("SNMP Agent is runnning!");
    Serial.println("Listening for GET requests...");
    delay(10);
    return;
  }
}

void loop() 
{
  /* Led 'Alive' */
   digitalWrite(D1,HIGH); /* On */
   delay(100);

 /* Get temperature and humidity and sends the value as integers */
   temperature = (int)dht.readTemperature() * 10;
   humidity = (int)dht.readHumidity() * 10;
 
   if ( millis() - prevMillis > 1000 ) /* Updates sysUpTime */
   {
    prevMillis += 1000;
    locUpTime += 100;
   }
   
   digitalWrite(D1,LOW); /* Off */
   delay(100);

   Agentuino.listen();
}
