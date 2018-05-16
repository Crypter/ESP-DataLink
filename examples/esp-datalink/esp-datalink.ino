#include <esp-datalink.h>

void sent(byte status){
  if (status) Serial.println("Packet dropped!"); //this could only happen if NOT in stubborn mode
}

void receive(byte *data, byte len){
  Serial.print("< ");
  Serial.print((char *)data);
}

void setup() {
  ESPDataLink.init();
  ESPDataLink.stubbornMode(true); //in stubborn mode the send function will block untill the message is certainly delivered, both devices must be in stubborn mode
  ESPDataLink.onReceive(receive);
  ESPDataLink.onSend(sent);
//  You need to pair both devices before use. Simply swap the commented on the second device...
//  ESPDataLink.pair("MAC:OF:ESP:DEVICE");
  ESPDataLink.pair("MAC:OF:OTHER:ESP:DEVICE");
  
  Serial.begin(9600);
  delay(50);
  Serial.println();
  Serial.print("Hi, I'm ");
  Serial.println(ESPDataLink.getMac());
}

void loop() {
  String data = Serial.readStringUntil('\n');
  if (data.length()){
    Serial.print("> ");
    Serial.print(data);
    data+="\n";
    ESPDataLink.send((uint8_t*)data.c_str(), data.length()+1);
    Serial.println(" (#)"); //once the function returns we can print 'seen'
  }
}

