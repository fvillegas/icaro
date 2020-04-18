#include "MadgwickAHRS.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

long timer = 0;
MPU6050 accelgyro;
Madgwick filter;
HMC5883L mag;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

void setup() {

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(38400);

  // initialize device
  Serial.println("I Initializing I2C devices...");
  accelgyro.initialize();
  accelgyro.setI2CBypassEnabled(true);
  mag.initialize();

  // verify connection
  Serial.println("I Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "I MPU6050 connection successful" : "E MPU6050 connection failed");
  Serial.println(mag.testConnection() ? "E HMC5883L connection successful" : "E HMC5883L connection failed");

}

void loop() {
  if (millis() - timer > 1000) {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // display tab-separated accel/gyro x/y/z values
    Serial.print("D a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);

    // display tab-separated gyro x/y/z values
    Serial.print("D mag:\t");
    Serial.print(mx); Serial.print("\t");
    Serial.print(my); Serial.print("\t");
    Serial.println(mz);

    timer = millis();
  }
}
