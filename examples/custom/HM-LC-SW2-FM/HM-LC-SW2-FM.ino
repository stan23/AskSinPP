//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2016-10-31 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2018-10-01 stan23 Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// ci-test=yes board=328p aes=no
//- -----------------------------------------------------------------------------------------------------------------------

// define this to read the device id, serial and device type from bootloader section
//#define USE_OTA_BOOTLOADER
//#define NDEBUG

// raise timer ticks to get more pin samples
#define TICKS_PER_SECOND 500UL
// we needs at least 10 LOW samples before we switch back to LOW
#define PINPOLL_COUNT_LOW 10

#include <AskSinPP.h>

#include <MultiChannelDevice.h>
#include <Switch.h>
#include <Remote.h>

// use MightyCore Standard pinout for ATmega32

#define RELAY1_PIN 0 // PB0
#define RELAY2_PIN 1 // PB1

#define BUTTON1_PIN 24 // PA0
#define BUTTON2_PIN 25 // PA1

// number of available peers per channel
// number of available peers per channel
#define PEERS_PER_SWCHANNEL  6
#define PEERS_PER_BTNCHANNEL 20

// all library classes are placed in the namespace 'as'
using namespace as;

// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
    {0x83,0x2A,0xE1},       // Device ID
    "HMSwC00001",           // Device Serial
    {0xf2,0x01},            // Device Model
    0x01,                   // Firmware Version
    as::DeviceType::Switch, // Device Type
    {0x01,0x00}             // Info Bytes
};

// Configure the used hardware
typedef AvrSPI<4,5,6,7> RadioSPI; // PB4-PB7
typedef StatusLed<12> LedType; // PD4
typedef AskSin<LedType,NoBattery,Radio<RadioSPI,11> > Hal; // PD3
Hal hal;

uint8_t SwitchPin (uint8_t number) {
  switch( number ) {
    case 4: return RELAY2_PIN;
  }
  return RELAY1_PIN;
}

typedef SwitchChannel<Hal,PEERS_PER_SWCHANNEL,List0>  SwChannel;
typedef RemoteChannel<Hal,PEERS_PER_BTNCHANNEL,List0> BtnChannel;

class MixDevice : public ChannelDevice<Hal,VirtBaseChannel<Hal,List0>,4> {
public:
  VirtChannel<Hal,BtnChannel,List0> c1,c2;
  VirtChannel<Hal,SwChannel,List0>  c3,c4;
public:
  typedef VirtBaseChannel<Hal,List0> ChannelType;
  typedef ChannelDevice<Hal,ChannelType,4> DeviceType;
  MixDevice (const DeviceInfo& info,uint16_t addr) : DeviceType(info,addr) {
    DeviceType::registerChannel(c1,1);
    DeviceType::registerChannel(c2,2);
    DeviceType::registerChannel(c3,3);
    DeviceType::registerChannel(c4,4);
  }
  virtual ~MixDevice () {}

  BtnChannel& btn1Channel () { return c1; }
  BtnChannel& btn2Channel () { return c2; }
  SwChannel&  sw1Channel  () { return c3; }
  SwChannel&  sw2Channel  () { return c4; }

  bool pollRadio () {
    bool worked = Device<Hal,List0>::pollRadio();
    for( uint8_t i=1; i<=this->channels(); ++i ) {
      ChannelType& ch = channel(i);
      if( ch.changed() == true ) {
        // we do not send status updates during a button is pressed
        if( btn1Channel().pressed()==false && btn2Channel().pressed()==false ) {
          this->sendInfoActuatorStatus(this->getMasterID(),this->nextcount(),ch);
          worked = true;
        }
      }
    }
    return worked;
  }

};
MixDevice sdev(devinfo,0x20);

void setup () {
  DINIT(19200,ASKSIN_PLUS_PLUS_IDENTIFIER);
  bool firstinit = sdev.init(hal);
  sdev.sw1Channel().init(RELAY1_PIN,false);
  sdev.sw2Channel().init(RELAY2_PIN,false);
  remoteChannelISR(sdev.btn1Channel(),BUTTON1_PIN);
  remoteChannelISR(sdev.btn2Channel(),BUTTON2_PIN);
  if( firstinit == true ) {
    // create internal peerings
    HMID devid;
    sdev.getDeviceID(devid);
    sdev.sw1Channel().peer(Peer(devid,1));
    sdev.sw2Channel().peer(Peer(devid,2));
    sdev.btn1Channel().peer(Peer(devid,3));
    sdev.btn2Channel().peer(Peer(devid,4));
    storage().store();
  }
  sdev.initDone();
}

void loop() {
//  bool pinchanged = sdev.btn1Channel().checkpin();
//  pinchanged |= sdev.btn2Channel().checkpin();
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
//  if( pinchanged == false && worked == false && poll == false ) {
//    hal.activity.savePower<Idle<> >(hal);
//  }
}
