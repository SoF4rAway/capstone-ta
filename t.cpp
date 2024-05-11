#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
// Insert your network credentials
#define WIFI_SSID "_"
#define WIFI_PASSWORD "jwtl1810"

// Insert Firebase project API Key
#define API_KEY "AIzaSyArADf57SHliMuSiU7mf1ILITMf2pyBa6k"
#define FIREBASE_PROJECT_ID "tugasakhir-c7b7b"
// Insert RTDB URLefine the RTDB URL */

#define USER_EMAIL "esp32@esp.com"
#define USER_PASSWORD "taini123"

//server mengambil data tanggal dan waktu sebenarnya
#define NTP_SERVER "time.nist.gov"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER);


#define INTERVAL 2000 // Delay between sensor readings in milliseconds
#define UPDATE_INTERVAL 60000 // Delay between data updates in milliseconds (10 seconds)
#define UPLOAD_INTERVAL 60000 // Delay between data uploads in milliseconds (1 minute)

#define pH_PIN A0 //inisialisasi pH
#define Offset 0.00
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40
int pHArray[ArrayLenth];
int pHArrayIndex=0;
static unsigned long samplingTime = 0;
static unsigned long printTime = 0;
float pHValue,voltage;

#define DO_PIN A1 //Inisialisasi DO
#define VREF 5000    // VREF (mv)
#define ADC_RES 1024 // ADC Resolution
#define TWO_POINT_CALIBRATION 0
#define READ_TEMP (25)
#define CAL1_V (1600)
#define CAL1_T (25)
#define CAL2_V (1300)
#define CAL2_T (15)
const uint16_t DO_Table[41] = {14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
                                11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
                                9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
                                7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long prevUpdateMillis = 0;
unsigned long prevUploadMillis = 0;
int count = 0;
bool signupOK = false;

String getTanggalWaktu() {
    timeClient.update();
    unsigned long rawTime = timeClient.getEpochTime();
    time_t t = rawTime + (7 * 3600);

    int jam = hour(t);
    String jamStr = jam < 10 ? "0" + String(jam) : String(jam);

    int menit = minute(t);
    String menitStr = menit < 10 ? "0" + String(menit) : String(menit);

    int detik = second(t);
    String detikStr = detik < 10 ? "0" + String(detik) : String(detik);

    int tgl = day(t);
    String tglStr = tgl < 10 ? "0" + String(tgl) : String(tgl);

    int bln = month(t);
    String blnStr = bln < 10 ? "0" + String(bln) : String(bln);

    int thn = year(t);
    String thnStr = String(thn);

    String tanggal = tglStr + "/" + blnStr + "/" + thnStr;
    String waktu = jamStr + ":" + menitStr + ":" + detikStr;

    return tanggal + " " + waktu;
}

void updateData(FirebaseData& fbdo, float pHValue, float DO)
{
     if (WiFi.status() == WL_CONNECTED && Firebase.ready())
    {
        String tanggalWaktu = getTanggalWaktu();
        FirebaseJson data;
        
        String documentPath = "karamba/"+ tanggalWaktu.substring(6, 10) + "-" + tanggalWaktu.substring(3, 5) + "-" + tanggalWaktu.substring(0, 2);
        String mask = "pH,DO";
        
        data.set("fields/Tanggal/stringValue", tanggalWaktu.substring(0, 10));
        data.set("fields/pH/doubleValue", pHValue);
        data.set("fields/DO/doubleValue", DO);

        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), data.raw(), mask.c_str()))
        {
             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        }
        else
        {
            Serial.println(fbdo.errorReason());
        }
    }
}

void uploadData(FirebaseData& fbdo, float pHValue, float DO)
{
    if (WiFi.status() == WL_CONNECTED && Firebase.ready())
    {
        String tanggalWaktu = getTanggalWaktu(); // Mendapatkan tanggal dan waktu sebenarnya
        
        FirebaseJson data;
       
        String documentPath = "history/" + tanggalWaktu.substring(6, 10) + "-" + tanggalWaktu.substring(3, 5) + "-" + tanggalWaktu.substring(0, 2) + "/" + tanggalWaktu.substring(11, 19);
      
        data.set("fields/Tanggal/stringValue", tanggalWaktu.substring(0, 10));
        data.set("fields/Jam/stringValue", tanggalWaktu.substring(11));
        data.set("fields/pH/doubleValue", pHValue);
        data.set("fields/DO/doubleValue", DO);
        
        if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), data.raw())) {
            Serial.print("ok\n%s\n\n");
            Serial.println(fbdo.payload());
        } else {
            Serial.println("Gagal mengirim data ke Firestore");
            Serial.println(fbdo.errorReason());
        }  
    }
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY; // Mengatur API key Firebase
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;

  /* Assign the callback function for the long running token generation task */
  fbdo.setResponseSize(4096);
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  timeClient.begin();
}

void loop(){
  unsigned long currentTime = millis();
  
  if(millis()-samplingTime > samplingInterval) {
    pHArray[pHArrayIndex++]=analogRead(pH_PIN);
    if(pHArrayIndex==ArrayLenth) pHArrayIndex=0;
    voltage = avergearray(pHArray, ArrayLenth)*5.0/1023;
    pHValue = 3.5*voltage+Offset;
    samplingTime=millis();

    Temperaturet = (uint8_t)READ_TEMP;
    ADC_Raw = analogRead(DO_PIN);
    ADC_Voltage = (uint32_t)VREF * ADC_Raw / ADC_RES;

    Serial.print("pH: ");
    Serial.println(pHValue, 2);

    Serial.print("Temperature: ");
    Serial.println(Temperaturet);

    DO = readDO(ADC_Voltage, Temperaturet);
    Serial.print("DO: ");
    Serial.println(DO);
  }

  if(millis() - printTime > printInterval) {
    Serial.print("Voltage: ");
    Serial.print(voltage, 2);
    Serial.print("    pH value: ");
    Serial.println(pHValue, 2);
    printTime=millis();
  }

// Pengiriman data ke monitoring
  if (currentTime - prevUpdateMillis >= UPDATE_INTERVAL) {
    updateData(fbdo, pHValue, DO);
    prevUpdateMillis = currentTime;
  }

  // Pengiriman data ke histori
  if (currentTime - prevUploadMillis >= UPLOAD_INTERVAL) {
    uploadData(fbdo, pHValue, DO);
    prevUploadMillis = currentTime;
  }
  
  delay (INTERVAL);
}

double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount=0;

  if(number<=0){
    Serial.println("Error number for the array to avraging!");
    return 0;
  }

  if(number<5){
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  } else {
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    } else {
      min=arr[1];max=arr[0];
    }

    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;
        min=arr[i];
      } else {
        if(arr[i]>max){
          amount+=max;
          max=arr[i];
        } else {
          amount+=arr[i];
        }
      }
    }
    avg = (double)amount/(number-2);
  }
  return avg;
}

int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c) {
#if TWO_POINT_CALIBRATION == 0
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}