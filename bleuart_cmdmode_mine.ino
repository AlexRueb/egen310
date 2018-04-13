/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

/*=========================================================================
    APPLICATION SETTINGS

  FACTORYRESET_ENABLE     Perform a factory reset when running this sketch
 
                            Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                            running this at least once is a good idea.
 
                            When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0. If you are making changes to your
                            Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why. Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
   
                            Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Pin Configuration and Firmware Declarations

#define LED_PIN       13

const unsigned long
  BLINKTIME =         100;
  
unsigned long 
  t_blink =           0L;

int
  blinkState =        LOW;

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}



/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  pinMode(LED_PIN, OUTPUT);
  
  //while (!Serial);  // required for Flora & Micro
  //delay(500);

  //Serial.begin(115600);
  //Serial.println(F("Adafruit Bluefruit Command Mode Example"));
  //Serial.println(F("---------------------------------------"));

  /* Initialise the module */
  //Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin() )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  //Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    //Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  //Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  //Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  //Serial.println(F("Then Enter characters to send to Bluefruit"));
  //Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    //Serial.println(F("******************************"));
    //Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
    //Serial.println(F("******************************"));
  }
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  // Now Check for incoming characters from Bluefruit
  ble.println("AT+BLEUARTRX");
  ble.readline();
  ble.waitForOK();

  String BLEbuffer = ble.buffer;

 // if (BLEbuffer.length() && BLEbuffer.indexOf("OK") == -1) 
 //   Serial.print(F("[Recv] ")); Serial.println(BLEbuffer);

  if (BLEbuffer.indexOf("Status") >= 0) {
 //   Serial.println(F("Status Request Received"));
    ble.print("AT+BLEUARTTX=");
    if (t_blink) {
      ble.println("BLNK");
    }
    else {
      if (blinkState)
        ble.println("ON");
      else
        ble.println("OFF");
    }

    // check response stastus
    if (! ble.waitForOK() ) {
 //     Serial.println(F("Failed to get response"));
    }

    ble.println("AT+BLEUARTRX");
   
  }
  
  else if (BLEbuffer.indexOf("0") >= 0) {
    //Serial.print(BLEbuffer);
    blinkState = LOW;
    digitalWrite(LED_PIN, blinkState);
    analogWrite(13, 0);
    analogWrite(11, 0);
    digitalWrite(18, LOW);
    digitalWrite(19, LOW);
    digitalWrite(20, LOW);
    digitalWrite(21, LOW);
    t_blink = 0;
    ble.print("AT+BLEUARTTX=");
    ble.println("OFF");
    //Serial.println(F("OFF Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("1") >= 0) {
    //Serial.print(BLEbuffer);
    //if (!t_blink) t_blink = millis();
    analogWrite(13, 100);
    analogWrite(11, 100);
    digitalWrite(18, HIGH);
    digitalWrite(19, LOW);
    digitalWrite(20, LOW);
    digitalWrite(21, HIGH);
    ble.print("AT+BLEUARTTX=");
    ble.println("FORWARD");
    //Serial.println(F("BLINK Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("2") >= 0) {
    //Serial.print(BLEbuffer);
    blinkState = HIGH;
    digitalWrite(LED_PIN, blinkState);
    analogWrite(13, 100);
    analogWrite(11, 100);
    digitalWrite(18, LOW);
    digitalWrite(19, HIGH);
    digitalWrite(20, HIGH);
    digitalWrite(21, LOW);
    
    t_blink = 0;
    ble.print("AT+BLEUARTTX=");
    ble.println("BACK");
    //Serial.println(F("ON Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("3") >= 0) {
    //Serial.print(BLEbuffer);
    analogWrite(13, 100);
    analogWrite(11, 100);
    digitalWrite(18, HIGH);
    digitalWrite(19, LOW);
    digitalWrite(20, HIGH);
    digitalWrite(21, LOW);
    ble.print("AT+BLEUARTTX=");
    ble.println("LEFT");
    //Serial.println(F("BLINK Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("4") >= 0) {
    //if (!t_blink) t_blink = millis();
    //Serial.print(BLEbuffer);
    analogWrite(13, 100);
    analogWrite(11, 100);
    digitalWrite(18, LOW);
    digitalWrite(19, HIGH);
    digitalWrite(20, LOW);
    digitalWrite(21, HIGH);
    
    ble.print("AT+BLEUARTTX=");
    ble.println("RIGHT");
    //Serial.println(F("BLINK Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("5") >= 0) {
    //if (!t_blink) t_blink = millis();
    //Serial.print(BLEbuffer);
    analogWrite(13, 150);
    analogWrite(11, 100);
    digitalWrite(18, HIGH);
    digitalWrite(19, LOW);
    digitalWrite(20, LOW);
    digitalWrite(21, HIGH);
    
    ble.print("AT+BLEUARTTX=");
    ble.println("FL");
    //Serial.println(F("BLINK Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("6") >= 0) {
    //if (!t_blink) t_blink = millis();
    //Serial.print(BLEbuffer);
    analogWrite(13, 100);
    analogWrite(11, 150);
    digitalWrite(18, HIGH);
    digitalWrite(19, LOW);
    digitalWrite(20, LOW);
    digitalWrite(21, HIGH);
    
    ble.print("AT+BLEUARTTX=");
    ble.println("FR");
    //Serial.println(F("BLINK Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("7") >= 0) {
    //if (!t_blink) t_blink = millis();
   // Serial.print(BLEbuffer);
    analogWrite(13, 150);
    analogWrite(11, 100);
    digitalWrite(18, LOW);
    digitalWrite(19, HIGH);
    digitalWrite(20, HIGH);
    digitalWrite(21, LOW);
    
    ble.print("AT+BLEUARTTX=");
    ble.println("BL");
    //Serial.println(F("BLINK Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  else if (BLEbuffer.indexOf("8") >= 0) {
    //if (!t_blink) t_blink = millis();
    //Serial.print(BLEbuffer);
    analogWrite(13, 100);
    analogWrite(11, 150);
    digitalWrite(18, LOW);
    digitalWrite(19, HIGH);
    digitalWrite(20, HIGH);
    digitalWrite(21, LOW);
    
    ble.print("AT+BLEUARTTX=");
    ble.println("BR");
    //Serial.println(F("BLINK Request Received"));
    ble.println("AT+BLEUARTRX");
  }
  //else {
    //analogWrite(13, BLEbuffer);
    //analogWrite(11, BLEbuffer);
  //}
  BLEbuffer = "";
}

