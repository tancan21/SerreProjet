#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 

 #include "WiFi.h"
#include <time.h>

#define uS_TO_mS_FACTOR 1000ULL
#define TIME_TO_SLEEP 300000

  #include <Arduino.h>
 #include <WiFi.h>
 #include <HTTPClient.h>
 #include <ArduinoJson.h>

#include <ESP_Mail_Client.h>

#include <WiFiMulti.h>
#include <WiFiUdp.h>

 const char* password = "hamdihamdi";
 const char* ssid = "Android";
 
//////////////*********** EMAIL**********////////////

WiFiMulti WiFiMulti;

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 587

/* The sign in credentials */
#define AUTHOR_EMAIL "testse20202021@gmail.com"
#define AUTHOR_PASSWORD "eywvbvqvmkozbeno"

/* Recipient's email*/
#define RECIPIENT_EMAIL "tetu@et.esiea.fr"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);
///////////*********END EMAIL **********////////////////////

 
//----- Adressage matériel ----- 
// En cas de non fonctionnement, mettez la ligne 8 en  
// commentaire et retirez le commentaire à la ligne 9. 
 
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);
const int buttonPin = 34;
const int ledPin = 4;
////////////////////*******TIME************/////////////////

const int decalage = 2;  // la valeur dépend de votre fuseau horaire. Essayez 2 pour la France. 
const int delaiDemande = 5 * 60; // nombre de secondes entre deux demandes consécutives au serveur NTP
int compteur = 0 ;
unsigned long derniereDemande = millis(); // moment de la plus récente demande au serveur NTP
unsigned long derniereMaJ = millis(); // moment de la plus récente mise à jour de l'affichage de l'heure
time_t maintenant;
struct tm * timeinfo;

void Serre_Ouvre() {
 int compteur = 0 ; 
      if ( compteur <5) {
      digitalWrite(ledPin, HIGH);
      delay(500) ; 
      digitalWrite(ledPin, LOW);
      delay(500) ; 
      compteur++ ; 
      }
     
    
    
}

void Serre_ferme() {
   if ( compteur <5) {
      digitalWrite(ledPin, HIGH);
      delay(200) ; 
      digitalWrite(ledPin, LOW);
      delay(200) ; 
      compteur++ ; 
      }
      compteur = 0 ; 
    }
    
void afficheHeureDate() {

  timeinfo = localtime(&maintenant);

  Serial.print("Heure:   ");
  if ((timeinfo->tm_hour ) < 10) {
    Serial.print("0");
  }
  Serial.print(timeinfo->tm_hour );  // heure entre 0 et 23
  Serial.print(":");
  if ( timeinfo->tm_hour > 21 and timeinfo->tm_hour < 8  ) {
   
    Serial.println("c'est la nuit , on ferme la serre " ) ; 
   // Serre_ferme() ; 
    
  
  }
    if ( timeinfo->tm_hour > 8 and timeinfo->tm_hour < 21    ) {
    
    Serial.println("c'est le jour  , on ouvre la serre " ) ; 
   // Serre_Ouvre() ; 
    }
  
  
   

  if (timeinfo->tm_min < 10) {
    Serial.print("0");
  }
  Serial.print(timeinfo->tm_min);   // timeinfo->tm_min: minutes (0 - 59)
  Serial.print(":");

  if (timeinfo->tm_sec < 10) {
    Serial.print("0");
  }
  Serial.println(timeinfo->tm_sec);   // timeinfo->tm_sec: secondes (0 - 60)
}

////////////////************END TIME*****************///////////




 

int val;

bool sens = true  ; 


void setup() {
Serial.begin(115200);
  //attachInterrupt(digitalPinToInterrupt(buttonPin), LED_fct, CHANGE);
//////////////***************** EMAIL**********////////////
 //Initialisation du Wifi
  setup_wifi();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Raph", RECIPIENT_EMAIL);

  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1> La serre se ferme !</h1><p>- Sent from ESP board</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;


  /*
  //Send raw text message
  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;*/

  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
    ////////////************** END EMAIL ************////////////////


    
  pinMode(ledPin, OUTPUT);  // declare LED as output
  pinMode(buttonPin, INPUT);    // declare pushbutton as input
/////////////////***************API WEATHER SETUP **********//////////
 // We start by connecting to a WiFi network
 /*
 Serial.println();
 Serial.println();
 Serial.print("Connecting to ");
Serial.println(ssid);

 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
*/
  ////////////////******* END API WEATHER SETUP*********////////////

  while (!Serial);
  Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
   lcd.init(); // initialisation de l'afficheur 

/////////// TIME//////////////////////////////////////
  WiFi.mode(WIFI_STA);

  Serial.println();

  WiFi.begin(ssid, password);
  Serial.print("Connexion au reseau WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
 
  }

  Serial.println();

  
  configTime(decalage * 3600, 0, "fr.pool.ntp.org");  //serveurs canadiens
       // en Europe, essayez europe.pool.ntp.org ou fr.pool.ntp.org
  
  Serial.print("Attente date et heure");

  while (time(nullptr) <= 100000) {
    Serial.print(".");
    delay(1000);
  }

  time(&maintenant);

  Serial.println("");
  /////////////////////////////////////////////////////////
  
///////////////*****deep Sleep Timer *****////////////////////////

// ESP32 wakes after TIME_TO_SLEEP milliseconds  ( 5 minutes = 300000 millisecondes ) 
esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_mS_FACTOR);

 //Reveille le processeur en appuyant sur un bouton connecte au GPIO34
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_34,1);
}




void loop() {
  //digitalWrite(ledPin, HIGH);
  val = digitalRead ( buttonPin );
if ( val== HIGH ){
  
  digitalWrite(ledPin, LOW);
  
}


  
  ///////////////////**************WEATHER API***************/////////////////
// wait for WiFi connection
 if((WiFi.status() == WL_CONNECTED)) {
 HTTPClient http;

// Serial.print("[HTTP] begin...\n");

 http.begin("http://api.weatherstack.com/current?access_key=2ef6d4b9578aaceabb335935d5ea8ba1&query=PARIS");

 //Serial.print("[HTTP] GET...\n");
 // start connection and send HTTP header
 int httpCode = http.GET();

 // httpCode will be negative on error
 if(httpCode == HTTP_CODE_OK) {
 String payload = http.getString();
 //Serial.println(payload);

 // Convert to JSON
 DynamicJsonDocument doc(1024);
 deserializeJson(doc, payload);

 // Read and display values
 String temp = doc["current"]["temperature"];
 String desc = doc["current"]["weather_descriptions"][0];
 //Serial.println("Temperature: "+temp+"*C, description: "+desc);
 Serial.println("description: "+desc);
 if ( desc == "Rainy" ) {
  Serial.println( "fermer  la serre " ) ; 
  Serre_ferme() ; 
 }
 
 } else {
 Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
 }
 http.end();
 }
 delay(1000);
 /////////////////////***********END WEATHER API *************////////////
  
///////////////////////TIME//////////////////////
  // est-ce le moment de demander l'heure NTP?
  if ((millis() - derniereDemande) >=  delaiDemande * 1000 ) {
    time(&maintenant);
    derniereDemande = millis();

    Serial.println("Interrogation du serveur NTP");
  }

  // est-ce que millis() a débordé?
  if (millis() < derniereDemande ) {
    time(&maintenant);
    derniereDemande = millis();
  }

  // est-ce le moment de raffraichir la date indiquée?
  if ((millis() - derniereMaJ) >=   1000 ) {
    maintenant = maintenant + 1;
    afficheHeureDate();
    derniereMaJ = millis();
  }
  
////////////////////////////////////END TIME ////////////////////////

  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");
if ( bme.temperature < 10 ) {
  Serial.println( "fermer la serre" ) ; 
  //Serre_ferme() ; 
}

  lcd.backlight(); 
  // Envoi du message 
  lcd.setCursor(0, 0); 
  lcd.print("Temperature = "); 
    lcd.setCursor(0,1); 
  lcd.print(bme.temperature); 
  delay(1000);
  
    lcd.setCursor(0, 0); 
  lcd.print("Humidity = "); 
    lcd.setCursor(0,1); 
  lcd.print(bme.humidity); 
  delay(1000);

//////////////////////////////////////////////
esp_deep_sleep_start();


}



/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}

void setup_wifi(){
 //connexion au wifi
  WiFiMulti.addAP(ssid, password);//WiFi.begin(ssid, password);
  while ( WiFiMulti.run() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("MAC : ");
  Serial.println(WiFi.macAddress());
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}
