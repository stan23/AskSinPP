//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2018-08-03 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2021-01-07 stan23   Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

// define this to read the device id, serial and device type from bootloader section
// #define USE_OTA_BOOTLOADER

#define STORAGEDRIVER at24cX<0x50,128,32>
#define TICKS_PER_SECOND 500UL


// Derive ID and Serial from the device UUID
#define USE_HW_SERIAL

#include <SPI.h>    // when we include SPI.h - we can use LibSPI class
#include <Wire.h>
#include <EEPROM.h> // the EEPROM library contains Flash Access Methods
#include <OneWireSTM.h>
#include <AskSinPP.h>
#include <Register.h>
#include "analog.h"

// pins definitions match this HW:
// https://github.com/pa-pa/STM32Dimmer

// we use a STM32
// STM32 pin for the LED
#define LED_PIN LED_BUILTIN

// STM32 pin for the config button
#define CONFIG_BUTTON_PIN PB12

#define CC1101_CS         PA4
#define CC1101_GDO0       PB0

// to connect DS18B20 temp sensor
#define ONE_WIRE_PIN      PB5

#define DIMMER1_PIN       PB1
#define DIMMER2_PIN       PA3
#define DIMMER3_PIN       PA2
#define DIMMER4_PIN       PA9
#define DIMMER5_PIN       PA8

#define ENCODER1_SWITCH   PB15
#define ENCODER1_CLOCK    PB13
#define ENCODER1_DATA     PB14

#define ENCODER2_SWITCH   PB10
#define ENCODER2_CLOCK    PB9
#define ENCODER2_DATA     PB8

/**
 * This sketch supports either adressable LEDs (e.g. WS2812B or SK6812)
 * or dump PWM LEDs.
 * Adressable LEDs are selected by default.
 * PWM LEDs can be chosen by enabling PWM_ENABLED. Take care that WSNUM_LEDS must be 1 in that case.
 */

#define WSNUM_LEDS    1           // number of connected LEDs, 1 or more
#define WSLED_PIN     255         // GPIO pin for LED data
#define WSLED_TYPE    WS2812B     // LED type, see classes in FastLED.h
#define WSCOLOR_ORDER GRB         // order of colours

//#define ENABLE_RGBW             // for SK6812 LEDs

#define PWM_ENABLED               
#ifdef PWM_ENABLED
  #define PWM_RED_PIN     DIMMER1_PIN
  #define PWM_GREEN_PIN   DIMMER2_PIN
  #define PWM_BLUE_PIN    DIMMER3_PIN
  #define PWM_WHITE_PIN   DIMMER4_PIN   // PWM pin for white LEDs. comment if not used
  #define PWM_WHITE_ONLY  true          // in case PWM_WHITE_PIN is used: shall only that white LED be active for white colour?
#endif

#define SLOW_PROGRAM_TIMER     30       // transition waiting time in ms
#define NORMAL_PROGRAM_TIMER   15       // transition waiting time in ms
#define FAST_PROGRAM_TIMER     0        // transition waiting time in ms
#define FIRE_PROGRAM_COOLING   55
#define FIRE_PROGRAM_SPARKLING 120


#ifdef ENABLE_RGBW
#include "FastLED_RGBW.h"
#endif
#include "RGBCtrl.h"

// number of available peers per channel
#define PEERS_PER_CHANNEL 6

// all library classes are placed in the namespace 'as'
using namespace as;

// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
  // ID and last 6 digits of Serial are derived from STM32-UUID (see #define USE_HW_SERIAL)
  {0xF3, 0x41, 0x01},     // Device ID
  "JPRGB00001",           // Device Serial
  {0xF3, 0x41},           // Device Model
  0x25,                   // Firmware Version
  as::DeviceType::Dimmer, // Device Type
  {0x01, 0x00}            // Info Bytes
};

/**
 * Configure the used hardware
 */
typedef LibSPI<CC1101_CS> RadioSPI;
typedef AskSin<StatusLed<LED_BUILTIN>,NoBattery,Radio<RadioSPI,CC1101_GDO0> > HalType;



DEFREGISTER(Reg0, MASTERID_REGS, 0x20, 0x21)
class Ws28xxList0 : public RegList0<Reg0> {
  public:
    Ws28xxList0(uint16_t addr) : RegList0<Reg0>(addr) {}

    void defaults () {
      clear();
    }
};

typedef RGBLEDChannel<HalType, PEERS_PER_CHANNEL, Ws28xxList0> ChannelType;
typedef RGBLEDDevice<HalType, ChannelType, 3, Ws28xxList0> RGBLEDType;

HalType hal;
RGBLEDType sdev(devinfo, 0x20);
ConfigButton<RGBLEDType> cfgBtn(sdev);

void setup () {
  delay(5000);
  DINIT(57600, ASKSIN_PLUS_PLUS_IDENTIFIER);
  Wire.begin();
  sdev.init(hal);
  buttonISR(cfgBtn, CONFIG_BUTTON_PIN);
  sdev.initDone();
  DDEVINFO(sdev);
}

void loop () {
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
  if( worked == false && poll == false ) {
//    hal.activity.savePower<Idle<true> >(hal);
  }
  sdev.handleLED();
}
