// File: KnxTelegram.cpp
// Author: Daniel Kleine-Albers (Since 2012)
// Modified: Thorsten Gehrig (Since 2014)
// Modified: Michael Werski (Since 2014)
// Modified: Katja Blankenheim (Since 2014)
// Modified: Mag Gyver (Since 2016)
// Modified: Rouven Raudzus (Since 2017)

// Last modified: 04.06.2024

#include "KnxTelegram.h"

KnxTelegram::KnxTelegram() {
  clear();
}

void KnxTelegram::clear() {
  for (unsigned i = 0; i < MAX_KNX_TELEGRAM_SIZE; i++) {
    buffer[i] = 0;
  }

  // Control Field, Normal Priority, No Repeat
  buffer[0] = 0b10111100;

  // Target Group Address, Routing Counter = 6, Length = 1 (= 2 Bytes)
  buffer[5] = 0b11100001;
}

byte KnxTelegram::getBufferByte(uint8_t index) {
  return buffer[index];
}

void KnxTelegram::setBufferByte(uint8_t index, byte content) {
  buffer[index] = content;
}

bool KnxTelegram::isRepeated() {
  // Parse Repeat Flag
  if (buffer[0] & 0b00100000) {
    return false;
  }
  else {
    return true;
  }
}

void KnxTelegram::setRepeated(bool repeat) {
  if (repeat) {
    buffer[0] = buffer[0] & 0b11011111;
  }
  else {
    buffer[0] = buffer[0] | 0b00100000;
  }
}

void KnxTelegram::setPriority(KnxPriorityType prio) {
  buffer[0] = buffer[0] & 0b11110011;
  buffer[0] = buffer[0] | (prio << 2);
}

KnxPriorityType KnxTelegram::getPriority() {
  // Priority
  return (KnxPriorityType) ((buffer[0] & 0b00001100) >> 2);
}

void KnxTelegram::setSourceAddress(uint8_t area, uint8_t line, uint8_t member) {
  buffer[1] = (area << 4) | line;	// Source Address
  buffer[2] = member; // Source Address
}

uint8_t KnxTelegram::getSourceArea() {
  return (buffer[1] >> 4);
}

uint8_t KnxTelegram::getSourceLine() {
  return (buffer[1] & 0b00001111);
}

uint8_t KnxTelegram::getSourceMember() {
  return buffer[2];
}

void KnxTelegram::setTargetGroupAddress(uint8_t main, uint8_t middle, uint8_t sub) {
  buffer[3] = (main << 3) | middle;
  buffer[4] = sub;
  buffer[5] = buffer[5] | 0b10000000;
}

void KnxTelegram::setTargetIndividualAddress(uint8_t area, uint8_t line, uint8_t member) {
  buffer[3] = (area << 4) | line;
  buffer[4] = member;
  buffer[5] = buffer[5] & 0b01111111;
}

bool KnxTelegram::isTargetGroup() {
  return buffer[5] & 0b10000000;
}

uint8_t KnxTelegram::getTargetMainGroup() {
  return ((buffer[3] & 0b11111000) >> 3);
}

uint8_t KnxTelegram::getTargetMiddleGroup() {
  return (buffer[3] & 0b00000111);
}

uint8_t KnxTelegram::getTargetSubGroup() {
  return buffer[4];
}

uint8_t KnxTelegram::getTargetArea() {
  return ((buffer[3] & 0b11110000) >> 4);
}

uint8_t KnxTelegram::getTargetLine() {
  return (buffer[3] & 0b00001111);
}

uint8_t KnxTelegram::getTargetMember() {
  return buffer[4];
}

void KnxTelegram::setRoutingCounter(uint8_t counter) {
  buffer[5] = buffer[5] & 0b10000000;
  buffer[5] = buffer[5] | (counter << 4);
}

uint8_t KnxTelegram::getRoutingCounter() {
  return ((buffer[5] & 0b01110000) >> 4);
}

void KnxTelegram::setPayloadLength(uint8_t length) {
  // Max length 16 bytes, 0-indexed
  length = (length - 1) & 0b00001111;
  buffer[5] = buffer[5] & 0b11110000;
  buffer[5] = buffer[5] | length;
}

uint8_t KnxTelegram::getPayloadLength() {
  // length is 0-indexed
  uint8_t length = (buffer[5] & 0b00001111) + 1;
  return length;
}

void KnxTelegram::setCommand(KnxCommandType command) {
  buffer[6] = buffer[6] & 0b11111100;
  buffer[7] = buffer[7] & 0b00111111;

  buffer[6] = buffer[6] | (command >> 2); // Command first two bits
  buffer[7] = buffer[7] | (command << 6); // Command last two bits
}

KnxCommandType KnxTelegram::getCommand() {
  return (KnxCommandType) (((buffer[6] & 0b00000011) << 2) | ((buffer[7] & 0b11000000) >> 6));
}

void KnxTelegram::setControlData(KnxControlDataType cd) {
  buffer[6] = buffer[6] & 0b11111100;
  buffer[6] = buffer[6] | cd;
}

KnxControlDataType KnxTelegram::getControlData() {
  return (KnxControlDataType) (buffer[6] & 0b00000011);
}

KnxCommunicationType KnxTelegram::getCommunicationType() {
  return (KnxCommunicationType) ((buffer[6] & 0b11000000) >> 6);
}

void KnxTelegram::setCommunicationType(KnxCommunicationType type) {
  buffer[6] = buffer[6] & 0b00111111;
  buffer[6] = buffer[6] | (type << 6);
}

uint8_t KnxTelegram::getSequenceNumber() {
  return (buffer[6] & 0b00111100) >> 2;
}

void KnxTelegram::setSequenceNumber(uint8_t number) {
  buffer[6] = buffer[6] & 0b11000011;
  buffer[6] = buffer[6] | (number << 2);
}

void KnxTelegram::createChecksum() {
  uint8_t checksumPos = getPayloadLength() + KNX_TELEGRAM_HEADER_SIZE;
  buffer[checksumPos] = calculateChecksum();
}

uint8_t KnxTelegram::getChecksum() {
  uint8_t checksumPos = getPayloadLength() + KNX_TELEGRAM_HEADER_SIZE;
  return buffer[checksumPos];
}

bool KnxTelegram::verifyChecksum() {
  uint8_t calculatedChecksum = calculateChecksum();
  return (getChecksum() == calculatedChecksum);
}

void KnxTelegram::print(TPUART_SERIAL_CLASS* serial) {
#if defined(TPUART_DEBUG)
  serial->print("Repeated: ");
  serial->println(isRepeated());

  serial->print("Priority: ");
  serial->println(getPriority());

  serial->print("Source: ");
  serial->print(getSourceArea());
  serial->print(".");
  serial->print(getSourceLine());
  serial->print(".");
  serial->println(getSourceMember());

  if (isTargetGroup()) {
    serial->print("Target Group: ");
    serial->print(getTargetMainGroup());
    serial->print("/");
    serial->print(getTargetMiddleGroup());
    serial->print("/");
    serial->println(getTargetSubGroup());
  }
  else {
    serial->print("Target Physical: ");
    serial->print(getTargetArea());
    serial->print(".");
    serial->print(getTargetLine());
    serial->print(".");
    serial->println(getTargetMember());
  }

  serial->print("Routing Counter: ");
  serial->println(getRoutingCounter());

  serial->print("Payload Length: ");
  serial->println(getPayloadLength());

  serial->print("Command: ");
  serial->println(getCommand());

  serial->print("First Data Byte: ");
  serial->println(getFirstDataByte());

  for (unsigned i = 2; i < getPayloadLength(); i++) {
    serial->print("Data Byte ");
    serial->print(i);
    serial->print(": ");
    serial->println(buffer[6 + i], BIN);
  }


  if (verifyChecksum()) {
    serial->println("Checksum matches");
  }
  else {
    serial->println("Checksum mismatch");
    serial->println(getChecksum(), BIN);
    serial->println(calculateChecksum(), BIN);
  }
#endif
}

uint8_t KnxTelegram::calculateChecksum() {
  uint8_t bcc = 0xFF;
  uint8_t size = getPayloadLength() + KNX_TELEGRAM_HEADER_SIZE;
  // @FIX Test unsigned vs int vs uint8_t
  for (unsigned i = 0; i < size; i++) {
    bcc ^= buffer[i];
  }

  return bcc;
}

uint8_t KnxTelegram::getTotalLength() {
  return KNX_TELEGRAM_HEADER_SIZE + getPayloadLength() + 1;
}

void KnxTelegram::setFirstDataByte(byte data) {
  buffer[7] = buffer[7] & 0b11000000;
  buffer[7] = buffer[7] | data;
}

byte KnxTelegram::getFirstDataByte() {
  return (buffer[7] & 0b00111111);
}

bool KnxTelegram::getBool() {
  if (getPayloadLength() != 2) {
    // Wrong payload length
    return 0;
  }

  return (getFirstDataByte() & 0b00000001);
}

uint8_t KnxTelegram::get4BitIntValue() {
  if (getPayloadLength() != 2) {
    // Wrong payload length
    return 0;
  }

  return (getFirstDataByte() & 0b00001111);
}

bool KnxTelegram::get4BitDirectionValue() {
  if (getPayloadLength() != 2) {
    // Wrong payload length
    return 0;
  }

  return ((getFirstDataByte() & 0b00001000)) >> 3;
}

uint8_t KnxTelegram::get4BitStepsValue() {
  if (getPayloadLength() != 2) {
    // Wrong payload length
    return 0;
  }

  return (getFirstDataByte() & 0b00000111);
}

void KnxTelegram::set1ByteIntValue(byte value) {
  setPayloadLength(3);
  buffer[8] = value;
}

byte KnxTelegram::get1ByteIntValue() {
  if (getPayloadLength() != 3) {
    // Wrong payload length
    return 0;
  }

  return (buffer[8]);
}

void KnxTelegram::set2ByteIntValue(uint16_t value) {
  setPayloadLength(4);

  buffer[8] = uint8_t(value >> 8);
  buffer[9] = uint8_t(value & 0x00FF);
}

uint16_t KnxTelegram::get2ByteIntValue() {
  if (getPayloadLength() != 4) {
    // Wrong payload length
    return 0;
  }
  uint16_t value = uint16_t(buffer[8] << 8) + uint8_t(buffer[9]);

  return (value);
}

void KnxTelegram::set2ByteFloatValue(float value) {
  setPayloadLength(4);

  // Scale the float by 100 to keep 2 digits precision
  int32_t scaled = round(value * 100.0f);

  // Determine exponent by right-shifting
  // 11 bits mantissa, 4 bits exponent
  uint8_t exponent = 0;
  while ((scaled > 2047 || scaled < -2048) && exponent < 15) {
    scaled >>= 1;
    exponent++;
  }

  // Mask in case number is negative for two's complement
  uint16_t mantissa = scaled & 0x07FF;

  bool sign = (scaled < 0);

  // S EEEE MMM
  buffer[8] = (sign << 7) | (exponent << 3) | (mantissa >> 8);
  // MMMM MMMM
  buffer[9] = mantissa & 0xFF;
}

float KnxTelegram::get2ByteFloatValue() {
  if (getPayloadLength() != 4) {
    // Wrong payload length
    return 0;
  }
  uint8_t exponent = (buffer[8] & 0b01111000) >> 3;
  int16_t mantissa = ((buffer[8] & 0b00000111) << 8) | (buffer[9]);

  // Check sign bit and convert to signed integer
  if (buffer[8] & 0b10000000) {
    mantissa |= 0xF800; // Sign-extend the 11-bit mantissa to a 16-bit signed integer
  }

  // Adjust mantissa by exponent using bit-shifting
  int32_t value = mantissa;
  value <<= exponent; // Multiply by 2^exponent using left shift

  // Scale down to restore the original float value
  return value * 0.01f;
}

void KnxTelegram::set3ByteTime(uint8_t weekday, uint8_t hour, uint8_t minute, uint8_t second) {
  setPayloadLength(5);

  // Move the weekday by 5 bits to the left
  weekday = weekday << 5;

  // Buffer [8] bit 5-7 for weekday [0-7] [0 = no day, 1 = monday ... 7 = sunday], bit 0-4 for hour [0-23]
  buffer[8] = (weekday & 0b11100000) + (hour & 0b00011111);

  // Buffer [9] bit 6-7 empty, bit 0-5 for minutes [0-59]
  buffer[9] =  minute & 0b00111111;

  // Buffer [10] bit 6-7 empty, bit 0-5 for seconds [0-59]
  buffer[10] = second & 0b00111111;
}

uint8_t KnxTelegram::get3ByteWeekdayValue() {
  if (getPayloadLength() != 5) {
    // Wrong payload length
    return 0;
  }
  return (buffer[8] & 0b11100000) >> 5;
}

uint8_t KnxTelegram::get3ByteHourValue() {
  if (getPayloadLength() != 5) {
    // Wrong payload length
    return 0;
  }
  return (buffer[8] & 0b00011111);
}

uint8_t KnxTelegram::get3ByteMinuteValue() {
  if (getPayloadLength() != 5) {
    // Wrong payload length
    return 0;
  }
  return (buffer[9] & 0b00111111);
}

uint8_t KnxTelegram::get3ByteSecondValue() {
  if (getPayloadLength() != 5) {
    // Wrong payload length
    return 0;
  }
  return (buffer[10] & 0b00111111);
}

void KnxTelegram::set3ByteDate(uint8_t day, uint8_t month, uint8_t year) {
  setPayloadLength(5);

  // Buffer [8] bit 5-7 empty, bit 0-4 for month days [1-31]
  buffer[8] = day & 0b00011111;

  // Buffer [9] bit 4-7 empty, bit 0-3 for months [1-12]
  buffer[9] =  month & 0b00001111;

  // Buffer [10] fill with year, bit 0-7 for year [0-99] [year ≥ 90 interpret as 20th century, year < 90 interpret as 21th century]
  buffer[10] = year & 0b01111111;
}

uint8_t KnxTelegram::get3ByteDayValue() {
  if (getPayloadLength() != 5) {
    // Wrong payload length
    return 0;
  }
  return (buffer[8] & 0b00011111);
}

uint8_t KnxTelegram::get3ByteMonthValue() {
  if (getPayloadLength() != 5) {
    // Wrong payload length
    return 0;
  }
  return (buffer[9] & 0b00001111);
}

uint8_t KnxTelegram::get3ByteYearValue() {
  if (getPayloadLength() != 5) {
    // Wrong payload length
    return 0;
  }
  return (buffer[10] & 0b01111111);
}

void KnxTelegram::set4ByteFloatValue(float value) {
  setPayloadLength(6);

  byte b[4];
  float *f = (float*)(void*) & (b[0]);
  *f = value;

  buffer[8 + 3] = b[0];
  buffer[8 + 2] = b[1];
  buffer[8 + 1] = b[2];
  buffer[8 + 0] = b[3];
}

float KnxTelegram::get4ByteFloatValue() {
  if (getPayloadLength() != 6) {
    // Wrong payload length
    return 0;
  }
  byte b[4];
  b[0] = buffer[8 + 3];
  b[1] = buffer[8 + 2];
  b[2] = buffer[8 + 1];
  b[3] = buffer[8 + 0];
  float *f = (float*)(void*) & (b[0]);
  float  r = *f;
  return r;
}

void KnxTelegram::set14ByteValue(String value) {
  // Define
  char _load[15];

  // Empty/Initialize with space
  for (unsigned i = 0; i < 14; ++i)
  {
    _load[i] = 0;
  }
  setPayloadLength(16);
  // Make out of value the Chararray
  value.toCharArray(_load, 15); // Must be 15 - because it completes with 0
  buffer[8 + 0] = _load [0];
  buffer[8 + 1] = _load [1];
  buffer[8 + 2] = _load [2];
  buffer[8 + 3] = _load [3];
  buffer[8 + 4] = _load [4];
  buffer[8 + 5] = _load [5];
  buffer[8 + 6] = _load [6];
  buffer[8 + 7] = _load [7];
  buffer[8 + 8] = _load [8];
  buffer[8 + 9] = _load [9];
  buffer[8 + 10] = _load [10];
  buffer[8 + 11] = _load [11];
  buffer[8 + 12] = _load [12];
  buffer[8 + 13] = _load [13];
}

String KnxTelegram::get14ByteValue() {
  if (getPayloadLength() != 16) {
    // Wrong payload length
    return "";
  }
  char _load[15];
  _load[0] = buffer[8 + 0];
  _load[1] = buffer[8 + 1];
  _load[2] = buffer[8 + 2];
  _load[3] = buffer[8 + 3];
  _load[4] = buffer[8 + 4];
  _load[5] = buffer[8 + 5];
  _load[6] = buffer[8 + 6];
  _load[7] = buffer[8 + 7];
  _load[8] = buffer[8 + 8];
  _load[9] = buffer[8 + 9];
  _load[10] = buffer[8 + 10];
  _load[11] = buffer[8 + 11];
  _load[12] = buffer[8 + 12];
  _load[13] = buffer[8 + 13];
  return (_load);
}
