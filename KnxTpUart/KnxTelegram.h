// File: KnxTelegram.h
// Author: Daniel Kleine-Albers (Since 2012)
// Modified: Thorsten Gehrig (Since 2014)
// Modified: Michael Werski (Since 2014)
// Modified: Katja Blankenheim (Since 2014)
// Modified: Mag Gyver (Since 2016)

// Last modified: 06.06.2017

#ifndef KnxTelegram_h
#define KnxTelegram_h

#include "Arduino.h"

#define MAX_KNX_TELEGRAM_SIZE 23
#define KNX_TELEGRAM_HEADER_SIZE 6

#define TPUART_SERIAL_CLASS Stream

// KNX priorities
enum KnxPriorityType {
  KNX_PRIORITY_SYSTEM = 0b00,
  KNX_PRIORITY_ALARM = 0b10,
  KNX_PRIORITY_HIGH = 0b01,
  KNX_PRIORITY_NORMAL = 0b11
};

// KNX commands / APCI Coding
enum KnxCommandType {
  KNX_COMMAND_READ = 0b0000,
  KNX_COMMAND_WRITE = 0b0010,
  KNX_COMMAND_ANSWER = 0b0001,
  KNX_COMMAND_INDIVIDUAL_ADDR_WRITE = 0b0011,
  KNX_COMMAND_INDIVIDUAL_ADDR_REQUEST = 0b0100,
  KNX_COMMAND_INDIVIDUAL_ADDR_RESPONSE = 0b0101,
  KNX_COMMAND_MASK_VERSION_READ = 0b1100,
  KNX_COMMAND_MASK_VERSION_RESPONSE = 0b1101,
  KNX_COMMAND_RESTART = 0b1110,
  KNX_COMMAND_ESCAPE = 0b1111
};

// Extended (escaped) KNX commands
enum KnxExtendedCommandType {
  KNX_EXT_COMMAND_AUTH_REQUEST = 0b010001,
  KNX_EXT_COMMAND_AUTH_RESPONSE = 0b010010
};

// KNX Transport Layer Communication Type
enum KnxCommunicationType {
  KNX_COMM_UDP = 0b00, // Unnumbered Data Packet
  KNX_COMM_NDP = 0b01, // Numbered Data Packet
  KNX_COMM_UCD = 0b10, // Unnumbered Control Data
  KNX_COMM_NCD = 0b11  // Numbered Control Data
};

// KNX Control Data (for UCD / NCD packets)
enum KnxControlDataType {
  KNX_CONTROLDATA_CONNECT = 0b00,      // UCD
  KNX_CONTROLDATA_DISCONNECT = 0b01,   // UCD
  KNX_CONTROLDATA_POS_CONFIRM = 0b10,  // NCD
  KNX_CONTROLDATA_NEG_CONFIRM = 0b11   // NCD
};

class KnxTelegram {
  public:
    KnxTelegram();

    void clear();
    void setBufferByte(uint8_t index, byte content);
    byte getBufferByte(uint8_t index);
    void setPayloadLength(uint8_t length);
    uint8_t getPayloadLength();
    void setRepeated(bool repeat);
    bool isRepeated();
    void setPriority(KnxPriorityType prio);
    KnxPriorityType getPriority();
    void setSourceAddress(uint8_t area, uint8_t line, uint8_t member);
    uint8_t getSourceArea();
    uint8_t getSourceLine();
    uint8_t getSourceMember();
    void setTargetGroupAddress(uint8_t main, uint8_t middle, uint8_t sub);
    void setTargetIndividualAddress(uint8_t area, uint8_t line, uint8_t member);
    bool isTargetGroup();
    uint8_t getTargetMainGroup();
    uint8_t getTargetMiddleGroup();
    uint8_t getTargetSubGroup();
    uint8_t getTargetArea();
    uint8_t getTargetLine();
    uint8_t getTargetMember();
    void setRoutingCounter(uint8_t counter);
    uint8_t getRoutingCounter();
    void setCommand(KnxCommandType command);
    KnxCommandType getCommand();

    void setFirstDataByte(byte data);
    byte getFirstDataByte();
    bool getBool();

    uint8_t get4BitIntValue();
    bool get4BitDirectionValue();
    uint8_t get4BitStepsValue();

    void set1ByteIntValue(byte value);
    byte get1ByteIntValue();

    void set2ByteIntValue(uint16_t value);
    uint16_t get2ByteIntValue();
    void set2ByteFloatValue(float value);
    float get2ByteFloatValue();

    void set3ByteTime(uint8_t weekday, uint8_t hour, uint8_t minute, uint8_t second);
    uint8_t get3ByteWeekdayValue();
    uint8_t get3ByteHourValue();
    uint8_t get3ByteMinuteValue();
    uint8_t get3ByteSecondValue();
    void set3ByteDate(uint8_t day, uint8_t month, uint8_t year);
    uint8_t get3ByteDayValue();
    uint8_t get3ByteMonthValue();
    uint8_t get3ByteYearValue();

    void set4ByteFloatValue(float value);
    float get4ByteFloatValue();

    void set14ByteValue(String value);
    String get14ByteValue();

    void createChecksum();
    bool verifyChecksum();
    uint8_t getChecksum();
    void print(TPUART_SERIAL_CLASS* serial);
    uint8_t getTotalLength();
    KnxCommunicationType getCommunicationType();
    void setCommunicationType(KnxCommunicationType type);
    uint8_t getSequenceNumber();
    void setSequenceNumber(uint8_t number);
    KnxControlDataType getControlData();
    void setControlData(KnxControlDataType cd);
  private:
    uint8_t buffer[MAX_KNX_TELEGRAM_SIZE];
    uint8_t calculateChecksum();

};

#endif