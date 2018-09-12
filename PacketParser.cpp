#include "packet.h"

#include <Arduino.h>

void WHILL::PacketParser::setParent(WHILL* whill){
  this->whill = whill;
}

int WHILL::PacketParser::parsePacket(Packet* packet){
  if(!(packet->is_valid()))return -1;

  switch(packet->payload[0]){  // Read Command ID

      case 0x00:   // Data set 0
        parseDataset0(packet);
        break;

      case 0x01:   // Data set 1
        parseDataset1(packet);
        break;

      case 0x52:   // Response of power WHILL on.
        if(whill!=NULL){
          whill->fire_callback(CALLBACK_POWER_ON);
        }
        break;

      default:
        return -1;  // Unknown Command
  }

  return 0;

}


void WHILL::PacketParser::parseDataset0(Packet* packet){
  if(whill == NULL) return;
  whill->fire_callback(CALLBACK_DATA0);
}

void WHILL::PacketParser::parseDataset1(Packet* packet){
  whill->accelerometer.x = (signed short)((packet->payload[1] << 8)+(packet->payload[2]));
  whill->accelerometer.y = (signed short)((packet->payload[3] << 8)+(packet->payload[4]));
  whill->accelerometer.z = (signed short)((packet->payload[5] << 8)+(packet->payload[6]));

  whill->gyro.x = (signed short)((packet->payload[7] << 8)+(packet->payload[8]));
  whill->gyro.y = (signed short)((packet->payload[9] << 8)+(packet->payload[10]));
  whill->gyro.z = (signed short)((packet->payload[11] << 8)+(packet->payload[12]));

  whill->joy.y = (int)(signed char)packet->payload[13];
  whill->joy.x = (int)(signed char)packet->payload[14];

  whill->battery.level   = (unsigned char)packet->payload[15];
  whill->battery.current = (signed short)((packet->payload[16] << 8)+(packet->payload[17])) * 2; //Unit : 2mA
  
  whill->right_motor.angle = (float)((signed short)((packet->payload[18] << 8)+(packet->payload[19])) * 0.001 );
  whill->left_motor.angle  = (float)((signed short)((packet->payload[20] << 8)+(packet->payload[21])) * 0.001 );

  whill->right_motor.speed = (signed short)((packet->payload[22] << 8)+(packet->payload[23])) * 4;
  whill->left_motor.speed  = (signed short)((packet->payload[24] << 8)+(packet->payload[25])) * 4;

  whill->power                = packet->payload[26] == 0x00 ? false : true;
  whill->speed_mode_indicator = packet->payload[27];

  if(whill == NULL)return;
  whill->fire_callback(CALLBACK_DATA1);
}
