#include "WHILL.h"


WHILL::Packet::Packet(){
  protocol_sign = 0xAF;
  len = 1;
  build();
}

WHILL::Packet::Packet(unsigned char payload[],int size){
  protocol_sign = 0xAF;
  len = size + 1;
  memcpy(this->payload,payload,size);
  build();
}

bool WHILL::Packet::is_valid(){
  return getCalculatedCS() == cs;
}

int WHILL::Packet::rawLength(){
  return 2 + len;   // protocol_sign + len + (the length of payload and cs)
}

bool WHILL::Packet::setRaw(unsigned char* raw,int whole_length){

    protocol_sign = raw[0];
    len           = raw[1];

    int prefix = 2;
    int i = 0;
    for(i=0;i<len-1;i++){
      payload[i]= raw[prefix+i];
    }

    cs = raw[prefix+i];
    
    return is_valid();
}

unsigned char WHILL::Packet::getCalculatedCS(){
  unsigned char cs = 0x00;

  cs ^= protocol_sign;
  cs ^= len;

  for(int i=0;i<len-1;i++){
    cs ^= payload[i];
  }

  return cs;
}

void WHILL::Packet::build(){
  this->cs = getCalculatedCS();
}

int WHILL::Packet::getRaw(unsigned char* raw){

  int whole_length = 0;

  raw[0] = protocol_sign;
  raw[1] = len;

  int prefix = 2;
  int i = 0;
  for(i=0;i<len-1;i++){
    raw[prefix+i] = payload[i];
  }

  raw[prefix+i] = cs;
  
  return rawLength();
}
