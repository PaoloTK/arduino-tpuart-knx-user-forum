// File: KnxTpUart.h
// Author: Daniel Kleine-Albers (Since 2012)
// Modified: Thorsten Gehrig (Since 2014)
// Modified: Michael Werski (Since 2014)
// Modified: Katja Blankenheim (Since 2014)
// Modified: Mag Gyver (Since 2016)

// Last modified: 06.06.2017

#ifndef KnxTpUart_h
#define KnxTpUart_h

#include "HardwareSerial.h"
#include "Arduino.h"

#include "KnxTelegram.h"

// Services from TPUART
#define TPUART_RESET_INDICATION_BYTE 0b11

// Services to TPUART
#define TPUART_DATA_START_CONTINUE 0b10000000
#define TPUART_DATA_END 0b01000000

// Uncomment the following line to enable debugging
//#define TPUART_DEBUG

#define TPUART_DEBUG_PORT Serial

#define TPUART_SERIAL_CLASS Stream

// Delay in ms between sending of packets to the bus
// Change only if you know what you're doing
#define SERIAL_WRITE_DELAY_MS 100

// Timeout for reading a byte from TPUART
// Change only if you know what you're doing
#define SERIAL_READ_TIMEOUT_MS 10

// Maximum number of group addresses that can be listened on
#define MAX_LISTEN_GROUP_ADDRESSES 24

enum KnxTpUartSerialEventType {
  TPUART_RESET_INDICATION,
  KNX_TELEGRAM,
  IRRELEVANT_KNX_TELEGRAM,
  TPUART_UNKNOWN_EVENT
};

class KnxTpUart {


  public:
    KnxTpUart(TPUART_SERIAL_CLASS* sport, String address);
    void uartReset();
    void uartStateRequest();
    KnxTpUartSerialEventType serialEvent();
    KnxTelegram* getReceivedTelegram();

    void setIndividualAddress(uint8_t area, uint8_t line, uint8_t member);

    void sendAck();
    void sendNotAddressed();

    bool groupWriteBool(String Address, bool value);
    bool groupWrite4BitInt(String Address, uint8_t value);
    bool groupWrite4BitDim(String Address, bool direction, byte steps);
    bool groupWrite1ByteInt(String Address, uint8_t value);
    bool groupWrite2ByteInt(String Address, uint16_t value);
    bool groupWrite2ByteFloat(String Address, float value);
    bool groupWrite3ByteTime(String Address, uint8_t weekday, uint8_t hour, uint8_t minute, uint8_t second);
    bool groupWrite3ByteDate(String Address, uint8_t day, uint8_t month, uint8_t year);
    bool groupWrite4ByteFloat(String Address, float value);
    bool groupWrite14ByteText(String Address, String value);

    bool groupAnswerBool(String, bool);
    /*
      bool groupAnswer4BitInt(String, int);
      bool groupAnswer4BitDim(String, bool, byte);
    */
    bool groupAnswer1ByteInt(String, int);
    bool groupAnswer2ByteInt(String, int);
    bool groupAnswer2ByteFloat(String, float);
    bool groupAnswer3ByteTime(String, int, int, int, int);
    bool groupAnswer3ByteDate(String, int, int, int);
    bool groupAnswer4ByteFloat(String, float);
    bool groupAnswer14ByteText(String, String);

    bool groupRead(String);

    void addListenGroupAddress(String);
    bool isListeningToGroupAddress(int, int, int);

    bool individualAnswerAddress();
    bool individualAnswerMaskVersion(int, int, int);
    bool individualAnswerAuth(int, int, int, int, int);

    void setListenToBroadcasts(bool);


  private:
    Stream* _serialport;
    KnxTelegram* _tg;       // for normal communication
    KnxTelegram* _tg_ptp;   // for PTP sequence confirmation
    int _source_area;
    int _source_line;
    int _source_member;
    int _listen_group_addresses[MAX_LISTEN_GROUP_ADDRESSES][3];
    int _listen_group_address_count;
    bool _listen_to_broadcasts;

    bool isKNXControlByte(int);
    void checkErrors();
    void printByte(int);
    bool readKNXTelegram();
    void createKNXMessageFrame(int, KnxCommandType, String, int);
    void createKNXMessageFrameIndividual(int, KnxCommandType, String, int);
    bool sendMessage();
    bool sendNCDPosConfirm(int, int, int, int);
    int serialRead();
};

#endif
