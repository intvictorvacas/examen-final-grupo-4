#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"

//https://github.com/miguelbalboa/rfid
#if defined(ESP8266)
#define SS_PIN D8
#define RST_PIN D0
#endif

// database and Wifi connection
#define FIREBASE_HOST "base-de-datos-bd932-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "YDBihh8jX0YaK8mYgpzaEQq0spKRB9KJ6L8y2Qok"
#define WIFI_SSID "ONE FRANK HURTADO"
#define WIFI_PASSWORD "frank969583"
FirebaseData firebaseData;
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;


// Init array that will store new NUID
byte nuidPICC[4];

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
String DatoHex;
const String UserReg_1 = "33DEF203";
const String UserReg_2 = "4A6561BF";
const String UserReg_3 = "";
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void setup()
{
  Serial.begin(115200);
  // connect to wifi and firebase.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.enableClassicRequest(firebaseData, true);

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  Serial.println();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println();
  Serial.println();
  Serial.println("Iniciando el Programa");
}

void loop()
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println("Su Tarjeta no es del tipo MIFARE Classic.");
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] )
  {
    Serial.println("Se ha detectado una nueva tarjeta.");

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    DatoHex = printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.print("Codigo Tarjeta: "); Serial.println(DatoHex);

    if (UserReg_1 == DatoHex)
    {
      Serial.println("USUARIO 1 - PUEDE INGRESAR");
    }
    else if (UserReg_2 == DatoHex)
    {
      Serial.println("USUARIO 2 - PUEDE INGRESAR");
    }
    else if (UserReg_3 == DatoHex)
    {
      Serial.println("USUARIO 3 - PUEDE INGRESAR");
    }
    else
    {
      Serial.println("NO ESTA REGISTRADO - PROHIBIDO EL INGRESO");
    }
    Serial.println();
  }
  else
  {
    Serial.println("Tarjeta leida previamente");
  }
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


String printHex(byte *buffer, byte bufferSize)
{
  String DatoHexAux = "";
  for (byte i = 0; i < bufferSize; i++)
  {
    if (buffer[i] < 0x10)
    {
      DatoHexAux = DatoHexAux + "0";
      DatoHexAux = DatoHexAux + String(buffer[i], HEX);
    }
    else {
      DatoHexAux = DatoHexAux + String(buffer[i], HEX);
    }
  }

  for (int i = 0; i < DatoHexAux.length(); i++) {
    DatoHexAux[i] = toupper(DatoHexAux[i]);
  }
  return DatoHexAux;
}

//Firebase.getInt(firebaseData, path + "/key" + DatoHex);
// if (firebaseData.intData() != data[DatoHex]) return false;
 
