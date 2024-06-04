#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "time.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Inserte las credenciales del wifi
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define API_KEY "YOUR_FIREDATABASE_API_KEY"

// Inserte el email y password correspondiente
#define USER_EMAIL "YOUR_EMAIL"
#define USER_PASSWORD "YOUR_PASSWORD"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"

#define SEISMIC_OBSERVATION_PERIOD 1000*30 //30sec
#define SAMPLING_SPEED 10 //10Hz
#define LAST_BUFFERSIZE 30 // find the acceleration in the last 3sec , find af that continues just 0.3sec ,
//                            assuming loopforSeismo() loops at 100msec interval
#include "bcj-simulation.h"
#define SIMULATION
#define BCJL1 //use BCJ-L1 wave , else BCJ-L2 is selected
#ifdef SIMULATION
#define BCJL1X_SIZE 6000
#define BCJL1Y_SIZE 6000
#define BCJL1Z_SIZE 6000
#include "bcj-simulation.h"

#endif



//#define VERBOSE

#define MAX1G 16384.0 // 1G in 2Gmax mode
#define GRAVITY 980.0 // 1G in cm/sec^2

#include "Wire.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
Adafruit_BMP3XX bmp;

//FreeRTOS
boolean publishGo = false;
QueueHandle_t xQueue;
TaskHandle_t Task1, Task2;
volatile SemaphoreHandle_t semaI2C;
#define  WAIT_FOR_SEMAPHORE_TIME  (100UL)

//MPU6050 paramter
const int MPU_addr = 0x68; // I2C address of the MPU-6050

#include <Filters.h> //https://github.com/JonHub/Filters 
FilterOnePole XFHigh(HIGHPASS, 0.2), YFHigh(HIGHPASS, 0.2), ZFHigh(HIGHPASS, 0.2);//0.2Hz high pass filter
//FilterOnePole XFHigh(HIGHPASS, 0.625), YFHigh(HIGHPASS, 0.625), ZFHigh(HIGHPASS, 0.625);//0.625Hz high pass filter

void codeForSeismo(void * parameter);


#define MPU6050_AUX_VDDIO          0x01   // R/W
#define MPU6050_SMPLRT_DIV         0x19   // R/W
#define MPU6050_CONFIG             0x1A   // R/W
#define MPU6050_GYRO_CONFIG        0x1B   // R/W
#define MPU6050_ACCEL_CONFIG       0x1C   // R/W
#define MPU6050_FF_THR             0x1D   // R/W
#define MPU6050_FF_DUR             0x1E   // R/W
#define MPU6050_MOT_THR            0x1F   // R/W
#define MPU6050_MOT_DUR            0x20   // R/W
#define MPU6050_ZRMOT_THR          0x21   // R/W
#define MPU6050_ZRMOT_DUR          0x22   // R/W
#define MPU6050_FIFO_EN            0x23   // R/W
#define MPU6050_I2C_MST_CTRL       0x24   // R/W
#define MPU6050_I2C_SLV0_ADDR      0x25   // R/W
#define MPU6050_I2C_SLV0_REG       0x26   // R/W
#define MPU6050_I2C_SLV0_CTRL      0x27   // R/W
#define MPU6050_I2C_SLV1_ADDR      0x28   // R/W
#define MPU6050_I2C_SLV1_REG       0x29   // R/W
#define MPU6050_I2C_SLV1_CTRL      0x2A   // R/W
#define MPU6050_I2C_SLV2_ADDR      0x2B   // R/W
#define MPU6050_I2C_SLV2_REG       0x2C   // R/W
#define MPU6050_I2C_SLV2_CTRL      0x2D   // R/W
#define MPU6050_I2C_SLV3_ADDR      0x2E   // R/W
#define MPU6050_I2C_SLV3_REG       0x2F   // R/W
#define MPU6050_I2C_SLV3_CTRL      0x30   // R/W
#define MPU6050_I2C_SLV4_ADDR      0x31   // R/W
#define MPU6050_I2C_SLV4_REG       0x32   // R/W
#define MPU6050_I2C_SLV4_DO        0x33   // R/W
#define MPU6050_I2C_SLV4_CTRL      0x34   // R/W
#define MPU6050_I2C_SLV4_DI        0x35   // R  
#define MPU6050_I2C_MST_STATUS     0x36   // R
#define MPU6050_INT_PIN_CFG        0x37   // R/W
#define MPU6050_INT_ENABLE         0x38   // R/W
#define MPU6050_INT_STATUS         0x3A   // R  
#define MPU6050_ACCEL_XOUT_H       0x3B   // R  
#define MPU6050_ACCEL_XOUT_L       0x3C   // R  
#define MPU6050_ACCEL_YOUT_H       0x3D   // R  
#define MPU6050_ACCEL_YOUT_L       0x3E   // R  
#define MPU6050_ACCEL_ZOUT_H       0x3F   // R  
#define MPU6050_ACCEL_ZOUT_L       0x40   // R  
#define MPU6050_TEMP_OUT_H         0x41   // R  
#define MPU6050_TEMP_OUT_L         0x42   // R  
#define MPU6050_GYRO_XOUT_H        0x43   // R  
#define MPU6050_GYRO_XOUT_L        0x44   // R  
#define MPU6050_GYRO_YOUT_H        0x45   // R  
#define MPU6050_GYRO_YOUT_L        0x46   // R  
#define MPU6050_GYRO_ZOUT_H        0x47   // R  
#define MPU6050_GYRO_ZOUT_L        0x48   // R  
#define MPU6050_EXT_SENS_DATA_00   0x49   // R  
#define MPU6050_EXT_SENS_DATA_01   0x4A   // R  
#define MPU6050_EXT_SENS_DATA_02   0x4B   // R  
#define MPU6050_EXT_SENS_DATA_03   0x4C   // R  
#define MPU6050_EXT_SENS_DATA_04   0x4D   // R  
#define MPU6050_EXT_SENS_DATA_05   0x4E   // R  
#define MPU6050_EXT_SENS_DATA_06   0x4F   // R  
#define MPU6050_EXT_SENS_DATA_07   0x50   // R  
#define MPU6050_EXT_SENS_DATA_08   0x51   // R  
#define MPU6050_EXT_SENS_DATA_09   0x52   // R  
#define MPU6050_EXT_SENS_DATA_10   0x53   // R  
#define MPU6050_EXT_SENS_DATA_11   0x54   // R  
#define MPU6050_EXT_SENS_DATA_12   0x55   // R  
#define MPU6050_EXT_SENS_DATA_13   0x56   // R  
#define MPU6050_EXT_SENS_DATA_14   0x57   // R  
#define MPU6050_EXT_SENS_DATA_15   0x58   // R  
#define MPU6050_EXT_SENS_DATA_16   0x59   // R  
#define MPU6050_EXT_SENS_DATA_17   0x5A   // R  
#define MPU6050_EXT_SENS_DATA_18   0x5B   // R  
#define MPU6050_EXT_SENS_DATA_19   0x5C   // R  
#define MPU6050_EXT_SENS_DATA_20   0x5D   // R  
#define MPU6050_EXT_SENS_DATA_21   0x5E   // R  
#define MPU6050_EXT_SENS_DATA_22   0x5F   // R  
#define MPU6050_EXT_SENS_DATA_23   0x60   // R  
#define MPU6050_MOT_DETECT_STATUS  0x61   // R  
#define MPU6050_I2C_SLV0_DO        0x63   // R/W
#define MPU6050_I2C_SLV1_DO        0x64   // R/W
#define MPU6050_I2C_SLV2_DO        0x65   // R/W
#define MPU6050_I2C_SLV3_DO        0x66   // R/W
#define MPU6050_I2C_MST_DELAY_CTRL 0x67   // R/W
#define MPU6050_SIGNAL_PATH_RESET  0x68   // R/W
#define MPU6050_MOT_DETECT_CTRL    0x69   // R/W
#define MPU6050_USER_CTRL          0x6A   // R/W
#define MPU6050_PWR_MGMT_1         0x6B   // R/W
#define MPU6050_PWR_MGMT_2         0x6C   // R/W
#define MPU6050_FIFO_COUNTH        0x72   // R/W
#define MPU6050_FIFO_COUNTL        0x73   // R/W
#define MPU6050_FIFO_R_W           0x74   // R/W
#define MPU6050_WHO_AM_I           0x75   // R

#define MPU6050_D0 0
#define MPU6050_D1 1
#define MPU6050_D2 2
#define MPU6050_D3 3
#define MPU6050_D4 4
#define MPU6050_D5 5
#define MPU6050_D6 6
#define MPU6050_D7 7

#define MPU6050_DLPF_CFG0     MPU6050_D0
#define MPU6050_DLPF_CFG1     MPU6050_D1
#define MPU6050_DLPF_CFG2     MPU6050_D2
#define MPU6050_EXT_SYNC_SET0 MPU6050_D3
#define MPU6050_EXT_SYNC_SET1 MPU6050_D4
#define MPU6050_EXT_SYNC_SET2 MPU6050_D5

#define MPU6050_EXT_SYNC_SET_0 (0)
#define MPU6050_EXT_SYNC_SET_1 (bit(MPU6050_EXT_SYNC_SET0))
#define MPU6050_EXT_SYNC_SET_2 (bit(MPU6050_EXT_SYNC_SET1))
#define MPU6050_EXT_SYNC_SET_3 (bit(MPU6050_EXT_SYNC_SET1)|bit(MPU6050_EXT_SYNC_SET0))
#define MPU6050_EXT_SYNC_SET_4 (bit(MPU6050_EXT_SYNC_SET2))
#define MPU6050_EXT_SYNC_SET_5 (bit(MPU6050_EXT_SYNC_SET2)|bit(MPU6050_EXT_SYNC_SET0))
#define MPU6050_EXT_SYNC_SET_6 (bit(MPU6050_EXT_SYNC_SET2)|bit(MPU6050_EXT_SYNC_SET1))
#define MPU6050_EXT_SYNC_SET_7 (bit(MPU6050_EXT_SYNC_SET2)|bit(MPU6050_EXT_SYNC_SET1)|bit(MPU6050_EXT_SYNC_SET0))

#define MPU6050_EXT_SYNC_DISABLED     MPU6050_EXT_SYNC_SET_0
#define MPU6050_EXT_SYNC_TEMP_OUT_L   MPU6050_EXT_SYNC_SET_1
#define MPU6050_EXT_SYNC_GYRO_XOUT_L  MPU6050_EXT_SYNC_SET_2
#define MPU6050_EXT_SYNC_GYRO_YOUT_L  MPU6050_EXT_SYNC_SET_3
#define MPU6050_EXT_SYNC_GYRO_ZOUT_L  MPU6050_EXT_SYNC_SET_4
#define MPU6050_EXT_SYNC_ACCEL_XOUT_L MPU6050_EXT_SYNC_SET_5
#define MPU6050_EXT_SYNC_ACCEL_YOUT_L MPU6050_EXT_SYNC_SET_6
#define MPU6050_EXT_SYNC_ACCEL_ZOUT_L MPU6050_EXT_SYNC_SET_7

#define MPU6050_DLPF_CFG_0 (0)
#define MPU6050_DLPF_CFG_1 (bit(MPU6050_DLPF_CFG0))
#define MPU6050_DLPF_CFG_2 (bit(MPU6050_DLPF_CFG1))
#define MPU6050_DLPF_CFG_3 (bit(MPU6050_DLPF_CFG1)|bit(MPU6050_DLPF_CFG0))
#define MPU6050_DLPF_CFG_4 (bit(MPU6050_DLPF_CFG2))
#define MPU6050_DLPF_CFG_5 (bit(MPU6050_DLPF_CFG2)|bit(MPU6050_DLPF_CFG0))
#define MPU6050_DLPF_CFG_6 (bit(MPU6050_DLPF_CFG2)|bit(MPU6050_DLPF_CFG1))
#define MPU6050_DLPF_CFG_7 (bit(MPU6050_DLPF_CFG2)|bit(MPU6050_DLPF_CFG1)|bit(MPU6050_DLPF_CFG0))

#define MPU6050_DLPF_260HZ    MPU6050_DLPF_CFG_0
#define MPU6050_DLPF_184HZ    MPU6050_DLPF_CFG_1
#define MPU6050_DLPF_94HZ     MPU6050_DLPF_CFG_2
#define MPU6050_DLPF_44HZ     MPU6050_DLPF_CFG_3
#define MPU6050_DLPF_21HZ     MPU6050_DLPF_CFG_4
#define MPU6050_DLPF_10HZ     MPU6050_DLPF_CFG_5
#define MPU6050_DLPF_5HZ      MPU6050_DLPF_CFG_6
#define MPU6050_DLPF_RESERVED MPU6050_DLPF_CFG_7

#define MPU6050_ACCEL_HPF0 MPU6050_D0
#define MPU6050_ACCEL_HPF1 MPU6050_D1
#define MPU6050_ACCEL_HPF2 MPU6050_D2
#define MPU6050_AFS_SEL0   MPU6050_D3
#define MPU6050_AFS_SEL1   MPU6050_D4
#define MPU6050_ZA_ST      MPU6050_D5
#define MPU6050_YA_ST      MPU6050_D6
#define MPU6050_XA_ST      MPU6050_D7

#define MPU6050_ACCEL_HPF_0 (0)
#define MPU6050_ACCEL_HPF_1 (bit(MPU6050_ACCEL_HPF0))
#define MPU6050_ACCEL_HPF_2 (bit(MPU6050_ACCEL_HPF1))
#define MPU6050_ACCEL_HPF_3 (bit(MPU6050_ACCEL_HPF1)|bit(MPU6050_ACCEL_HPF0))
#define MPU6050_ACCEL_HPF_4 (bit(MPU6050_ACCEL_HPF2))
#define MPU6050_ACCEL_HPF_7 (bit(MPU6050_ACCEL_HPF2)|bit(MPU6050_ACCEL_HPF1)|bit(MPU6050_ACCEL_HPF0))

#define MPU6050_ACCEL_HPF_RESET  MPU6050_ACCEL_HPF_0
#define MPU6050_ACCEL_HPF_5HZ    MPU6050_ACCEL_HPF_1
#define MPU6050_ACCEL_HPF_2_5HZ  MPU6050_ACCEL_HPF_2
#define MPU6050_ACCEL_HPF_1_25HZ MPU6050_ACCEL_HPF_3
#define MPU6050_ACCEL_HPF_0_63HZ MPU6050_ACCEL_HPF_4
#define MPU6050_ACCEL_HPF_HOLD   MPU6050_ACCEL_HPF_7

#define MPU6050_AFS_SEL_0 (0)
#define MPU6050_AFS_SEL_1 (bit(MPU6050_AFS_SEL0))
#define MPU6050_AFS_SEL_2 (bit(MPU6050_AFS_SEL1))
#define MPU6050_AFS_SEL_3 (bit(MPU6050_AFS_SEL1)|bit(MPU6050_AFS_SEL0))

#define MPU6050_AFS_SEL_2G  MPU6050_AFS_SEL_0
#define MPU6050_AFS_SEL_4G  MPU6050_AFS_SEL_1
#define MPU6050_AFS_SEL_8G  MPU6050_AFS_SEL_2
#define MPU6050_AFS_SEL_16G MPU6050_AFS_SEL_3


// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;

String aPath = "/Acceleration";
String IfFPath = "/IfF";
String IfFmaxPath = "/IfFmax";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;

FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

 

extern float  af2, IfF, IfFmax, If_h , IfF_hmax;

void codeForFirebase(void * parameter) {
  portTickType xLastWakeTime;
  BaseType_t result;
  boolean publishGo = false;
  while (1) {
    if (xQueue != 0) {
      result = xQueueReceive(xQueue, &publishGo, portMAX_DELAY);
      if (result != pdPASS) {
        Serial.printf("queue receive failed\r\n");
      } else {
        if (publishGo == true) {
          publishGo = false;
          if (Firebase.ready()) {
            timestamp = getTime();
            //Serial.printf("time %d",timestamp);
            parentPath= databasePath + "/" + String(timestamp);
            json.set(aPath.c_str(), String(af2));
            json.set(IfFPath.c_str(), String(IfF));
            json.set(IfFmaxPath.c_str(), String(IfFmax));
            json.set(timePath, String(timestamp));
            Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    

            if (Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json)) {
              Serial.println("Data sent to Firebase successfully");
            } else {
              Serial.println(fbdo.errorReason());
            }
          }
        }
      }
    }
  }
}

extern int initMPU6050();
extern void clearLastBuffer();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("ESP32-seismometer.ino");

  pinMode(GPIO_NUM_2, OUTPUT); //LED
  Wire.begin(GPIO_NUM_21, GPIO_NUM_22);
//  Wire.setClock(400000L);
  //http://www.azusa-st.com/kjm/FreeRtos/API/semaphores/vSemaphoreTake.html
  //I2C bus is shared by two devices , semaI2C takes exclusive I2C bus control
  semaI2C = xSemaphoreCreateMutex();
  xSemaphoreGive( semaI2C );  /* give one semaphore */

   delay(3000); //wait mechanical stability of MPU6050 after you turn on  ESP32 board
  if (initMPU6050()==-1) {
    Serial.printf("MPU6050/MPU9250 not detected\r\n");
    while(1);
  }
  

  initWiFi();
  configTime(0, 0, ntpServer);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

 
  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

  
  clearLastBuffer();//LastBuffer is used to find acceleration exceeds 0.3sec
  // publishGo is used to signal MQTT publish task to go
  xQueue = xQueueCreate(1, sizeof(boolean));
  if (xQueue == NULL)
  {
    Serial.printf("queue creation failed \r\n");
    while (1);
  } else {

    xTaskCreatePinnedToCore( codeForSeismo, "codeForSeismo", 4000, NULL, 1, &Task1, 1); //core 1
    delay(3000);//publish after sensor stabilization time
    xTaskCreatePinnedToCore( codeForFirebase, "codeForFireBase", 8000, NULL, 1, &Task2, 1); //core 1
  }

}



void writeMPU6050(byte reg, byte data) {
  Wire.beginTransmission(MPU_addr);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}
int initMPU6050() {
  unsigned char c;
  int error;
  Wire.beginTransmission(MPU_addr);
  Wire.write(MPU6050_WHO_AM_I);
  Wire.endTransmission();
  Wire.requestFrom(MPU_addr, 1);
  c = Wire.read();
  Serial.print("MPU9250 / MPU6050 WHO_AM_I ( 0x71/ 0x68): ");
  Serial.println(c, HEX);
  if (!(c==0x71 || c==0x68)) {
    return -1;
  }
  writeMPU6050(MPU6050_ACCEL_CONFIG, MPU6050_AFS_SEL_2G); // accel range: ±2g
  writeMPU6050(MPU6050_PWR_MGMT_1, 0x00);   // disable sleep mode
#ifdef SIMULATION
#else
  getOffsetMPU6050();
#endif
  return 0; //OK
}

void getAxAyAz( int16_t *AcX, int16_t *AcY , int16_t *AcZ) {
    int16_t  Tmp,GyX,GyY,GyZ;
    if (xSemaphoreTake( semaI2C, pdMS_TO_TICKS( WAIT_FOR_SEMAPHORE_TIME )  ) == pdTRUE ) {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14); // request a total of 14 registers
    *AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    *AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    *AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    Tmp=Wire.read()<<8|Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)  
    GyY=Wire.read()<<8|Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    xSemaphoreGive( semaI2C );  /* give one semaphore */
  } else {
    Serial.println("failed to get semaphore");
  }
}
int16_t offsetX, offsetY, offsetZ; //MPU9050 offset , placed horizontaly
void getOffsetMPU6050() {
  int i;
  int16_t AcX, AcY, AcZ;
  int32_t tempAcX = 0;
  int32_t tempAcY = 0;
  int32_t tempAcZ = 0;
#define CALIBCNT 1000
  for (i = 0; i < CALIBCNT; i++) {
    getAxAyAz( &AcX, &AcY , &AcZ);
   // Serial.printf("inner %d , %d , %d \r\n",AcX,AcY,AcZ-16384);
    tempAcX = tempAcX + AcX;
    tempAcY = tempAcY + AcY;
    tempAcZ = tempAcZ + AcZ;
    delay(10);
    if (i % 100 == 0) Serial.print(".");
  }
  offsetX = tempAcX / CALIBCNT;
  offsetY = tempAcY / CALIBCNT;
  offsetZ = tempAcZ / CALIBCNT;

  float afinG = sqrt((float)offsetX * (float)offsetX + (float)offsetY * (float)offsetY + (float)offsetZ * (float)offsetZ);

  Serial.printf("\r\nMPU6050 offset %d %d %d  total cm/sec^2 %f\r\n", offsetX, offsetY, offsetZ , afinG / MAX1G * GRAVITY);
  delay(3000);//wait for mechanical stabilization
}
//shared variables
float af2 = 0.0;
float IfF = 0.0;  //seismic index
float IfF_h = 0.0; //seismic index for horizontal acceleration
float IfFtemp = 0.0;
float IfFmax = 0.0; //max IfF during observation period
float IfF_htemp = 0.0;
float IfF_hmax = 0.0;//max IfF_h during observation period

void loop() {
  
}


void codeForSeismo( void * parameter)
{
  BaseType_t result;
  int i = 0;
  long ts;
  long ts_now;
  boolean publishGo;

  while (1) {
    if (i == 0)  {
      ts = millis();
    }
    loopforSeismo(); //update IfF IfF_h every 100msec
    //hold max IfF,IfF_h
    if ( IfF > IfFtemp )
    {
      IfFtemp = IfF;
    }
    if ( IfF_h > IfF_htemp ) {
      IfF_htemp = IfF_h;
    }
    i++;
    ts_now = millis();
    if ( ts_now - ts > SEISMIC_OBSERVATION_PERIOD ) { //get max IfF during observation period
      IfFmax = IfFtemp; IfFtemp = 0.0;
      IfF_hmax = IfF_htemp; IfF_htemp = 0.0;
      i = 0;

      publishGo = true;
      result = xQueueSend(xQueue, &publishGo, 10);

      if (result != pdPASS)
      {
        Serial.printf("xQueue failed\r\n");
      }
    }
  }

}
// https://github.com/emilv/ArduinoSort
#include <ArduinoSort.h>
long loopCnt = 0;
long bufferCnt = 0;

float IfF_h_last[LAST_BUFFERSIZE];
float IfF_last[LAST_BUFFERSIZE];
float temp_last[LAST_BUFFERSIZE];

void clearLastBuffer() {
  int i;
  for (i = 0; i < LAST_BUFFERSIZE; i++) {
    IfF_h_last[i] = 0.0;
    IfF_last[i] = 0.0;
  }
}
void loopforSeismo() {
  int16_t AcX = 0, AcY = 0, AcZ = 0;
  int32_t tempAcX = 0, tempAcY = 0, tempAcZ = 0;

  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();// Initialise the xLastWakeTime variable with the current time.
  const portTickType xFrequency = 100 ; //100msec loop
#define LOOPNUM 10

#ifdef SIMULATION
  extern float bcjL1_waveX[];
  extern float bcjL1_waveY[];
  extern float bcjL1_waveZ[];  
  extern float bcjL2_wave[];

  for (int i = 0; i < LOOPNUM; i++) {
#ifdef BCJL1
    AcX = (int16_t)(bcjL1_waveX[loopCnt % BCJL1X_SIZE] / GRAVITY * MAX1G);
    AcY = (int16_t)(bcjL1_waveY[loopCnt % BCJL1Y_SIZE] / GRAVITY * MAX1G);
    AcZ = (int16_t)(bcjL1_waveZ[loopCnt % BCJL1Z_SIZE] / GRAVITY * MAX1G);

#else
    AcX = (int16_t)(bcjL2_wave[loopCnt % BCJL2_SIZE] / GRAVITY * MAX1G);
#endif

    tempAcX = tempAcX + AcX;
    tempAcY = tempAcY + AcY;
    tempAcZ = tempAcZ + AcZ;
    if ( i < LOOPNUM - 1) { //skip delay at last loop
      delay(1000 / (SAMPLING_SPEED * LOOPNUM));
    }
    loopCnt++;
  }
  // get averaged Ac?
  AcX = tempAcX / LOOPNUM;
  AcY = tempAcY / LOOPNUM;
  AcZ = tempAcZ / LOOPNUM;
#else
  for (int i = 0; i < LOOPNUM; i++) {
    getAxAyAz( &AcX, &AcY , &AcZ);
    // Serial.printf("inner %d , %d , %d \r\n",AcX,AcY,AcZ-16384);
    tempAcX = tempAcX + AcX;
    tempAcY = tempAcY + AcY;
    tempAcZ = tempAcZ + AcZ;
    if ( i < (LOOPNUM - 1)) { //skip delay at last loop
      delay(1000 / (SAMPLING_SPEED * LOOPNUM));
    }
  }
  //get avearge
  AcX = tempAcX / LOOPNUM;
  AcY = tempAcY / LOOPNUM;
  AcZ = tempAcZ / LOOPNUM;

  AcX = AcX - offsetX;
  AcY = AcY - offsetY;
  AcZ = AcZ - offsetZ;
#endif

  // convert in G=980gal 1G=32768/2
  XFHigh.input(AcX / MAX1G);
  YFHigh.input(AcY / MAX1G);
  ZFHigh.input(AcZ / MAX1G);

#define REMOVE_GRAVITY
#ifdef REMOVE_GRAVITY
  //  Serial.printf("%f %f %f \r\n",XFHigh.output(),YFHigh.output(),ZFHigh.output());
  float af_h = GRAVITY * sqrt(XFHigh.output() * XFHigh.output() + YFHigh.output() * YFHigh.output() ); //in gal
  float af = GRAVITY * sqrt(XFHigh.output() * XFHigh.output() + YFHigh.output() * YFHigh.output() + ZFHigh.output() * ZFHigh.output() ); //in gal
#else
  // without soft high pass filter 1Hz
  float af_h = GRAVITY * sqrt( (float)AcX * (float)AcX / MAX1G / MAX1G + (float)AcY * (float)AcY / MAX1G / MAX1G  ); //in gal
  float af = GRAVITY * sqrt( (float)AcX * (float)AcX / MAX1G / MAX1G + (float)AcY * (float)AcY / MAX1G / MAX1G + (float)AcZ * (float)AcZ / MAX1G / MAX1G ); //in gal
  //  float af_h = GRAVITY * sqrt( (float)AcX*(float)AcX/MAX1G/MAX1G + (float)AcY*(float)AcY/MAX1G/MAX1G );//in gal
#endif


  float If = 2.0 * log10( af ) + 0.94; //https://www.jma.go.jp/jma/press/0903/30a/jma-shindo-kaisetsu-pub.pdf
  float If_horizontal = 2.0 * log10( af_h ) + 0.94;
  // af in cm/sec2
  // Serial.printf("%f ,%f\r\n",af,If);
  if (If < 0.0 ) If = 0.0;
  if (If_horizontal < 0.0 ) If_horizontal = 0.0;

  IfF_last[bufferCnt] = If;
  IfF_h_last[bufferCnt] = If_horizontal;

  //find minimum in the last 3 observation , correspondig to 0.3sec
  //  findMin( IfF_last , IfF_h_last, 3 , &IfF , &IfF_h);
  for (int j = 0; j < LAST_BUFFERSIZE; j++) {
    temp_last[j] = IfF_last[j];
  }
//#define DEBUG
#ifdef DEBUG
  Serial.print("If raw: ");
  for (int j = 0; j < LAST_BUFFERSIZE; j++) {
    Serial.print(temp_last[j]); Serial.print(",");
  }
  Serial.println();

#endif
  sortArrayReverse(temp_last, LAST_BUFFERSIZE);

#ifdef DEBUG
  Serial.print("sorted ");
  for (int j = 0; j < LAST_BUFFERSIZE; j++) {
    Serial.print(temp_last[j]); Serial.print(",");
  }
  Serial.println();
#endif

  IfF = temp_last[2]; // acceleration at least 0.3sec

  for (int j = 0; j < LAST_BUFFERSIZE; j++) {
    temp_last[j] = IfF_h_last[j];
  }
  sortArrayReverse(temp_last, LAST_BUFFERSIZE);
  IfF_h = temp_last[2];

  bufferCnt++;
  if (bufferCnt > (LAST_BUFFERSIZE - 1)) bufferCnt = 0;
  af2 = af;
  //Serial.printf("ts If IfF Iftemp Ifmax %d %f %f %f %f\r\n", millis(), If, IfF, IfFtemp, IfFmax);
  //Serial.printf("af_h If  IfF Iftemp Ifmax  %f %f %f %f %f\r\n", af_h , If, IfF, IfFtemp, IfFmax);
  Serial.println("If,  IfF, Iftemp, Ifmax");
  Serial.printf("%f,  %f , %f ,%f \r\n", If, IfF, IfFtemp, IfFmax);
    vTaskDelayUntil( &xLastWakeTime, xFrequency ); //wait until 100msec for this loop
}
