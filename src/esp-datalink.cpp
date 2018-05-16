#include <ESP8266WiFi.h>
#include <espnow.h>
#include "esp-datalink.h"



	void (*ESPDataLinkClass::on_receive)(uint8_t *data, uint8_t len) = 0;
	void (*ESPDataLinkClass::on_send)(uint8_t status) = 0;
	
	uint8_t ESPDataLinkClass::paired[6]={};
  uint8_t ESPDataLinkClass::stubborn=0;
  uint8_t ESPDataLinkClass::ACK=0;
  uint8_t ESPDataLinkClass::SEQrx=1;
  uint8_t ESPDataLinkClass::SEQtx=0;
  uint8_t ESPDataLinkClass::channel=6;
	uint8_t ESPDataLinkClass::quality[100]={};
	uint8_t ESPDataLinkClass::quality_counter=0;
    
	ESPDataLinkClass::ESPDataLinkClass(){
	}

	String ESPDataLinkClass::macToString(const uint8_t* mac) {
		char buf[20];
		snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		return String(buf);
	}
	
	void ESPDataLinkClass::onReceive(void (*funk)(uint8_t *data, uint8_t len)){
		ESPDataLinkClass::on_receive = funk;
	}
	
	void ESPDataLinkClass::onSend(void (*funk)(uint8_t status)){
	  ESPDataLinkClass::on_send = funk;
	}
	
	int8_t ESPDataLinkClass::send(uint8_t* data, uint8_t len){
    if (stubborn) {
      uint8_t seqData[len+1];
      seqData[0] = SEQtx;
      SEQtx++;
      for (uint8_t i=0; i<len; i++) seqData[i+1] = data[i];
      
      uint8_t res=0;
      do {
        res = esp_now_send(ESPDataLinkClass::paired, seqData, len+1);
        delay(50);
//        Serial.println();
//        Serial.print("DEBUG | ");
//        Serial.printf("%d - %d", res, ESPDataLinkClass::ACK);
      } while( res || ESPDataLinkClass::ACK);
    }
		else return esp_now_send(ESPDataLinkClass::paired, data, len);
    return 0;
	}
	
	uint8_t ESPDataLinkClass::getQuality(){
		uint8_t result=0;
		for (uint8_t i=0; i<100; i++) result += quality[i];
		return result;
	}
	
	void ESPDataLinkClass::init(){
		WiFi.mode(WIFI_AP);
		WiFi.softAP(WiFi.softAPmacAddress().c_str(), 0, 6, 1);
		//WiFi.disconnect();
		esp_now_init();
		esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
		esp_now_register_send_cb(&ESPDataLinkClass::send_data);
		esp_now_register_recv_cb(&ESPDataLinkClass::receive_data);
		
	}
	
	String ESPDataLinkClass::getMac(){
		return WiFi.softAPmacAddress();
	}


  void ESPDataLinkClass::stubbornMode(bool enforce){
    ESPDataLinkClass::stubborn = enforce;
  }
	
	void ESPDataLinkClass::pair(String mac){
		mac.replace("-", "");
		mac.replace(":", "");
		mac.replace(" ", "");
		mac.toUpperCase();
		Serial.println(mac);
		paired[6];
		for (int i=0; i<6; i++) {
			paired[i] = ((mac[i*2] > '9') ? (mac[i*2]-'A'+0xA):(mac[i*2]-'0')) << 4 | ((mac[i*2+1] > '9') ? (mac[i*2+1]-'A'+0xA):(mac[i*2+1]-'0'));
		}
		esp_now_add_peer(paired, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
	}
	
	void ESPDataLinkClass::receive_data(uint8_t *mac, uint8_t *data, uint8_t len) {
  if (stubborn && ESPDataLinkClass::on_receive) {
    if (ESPDataLinkClass::SEQrx != data[0]) {
      ESPDataLinkClass::SEQrx = data[0];
      for (uint8_t i=0; i<len-1; i++) data[i] = data[i+1];
      ESPDataLinkClass::on_receive(data, len-1);
    }
  }
	else if (ESPDataLinkClass::on_receive) ESPDataLinkClass::on_receive(data, len);
	}
	
	void ESPDataLinkClass::send_data(uint8_t *mac, uint8_t status) {
    ESPDataLinkClass::ACK = status;
		status = !status; //ensure 0-1 range and invert logic
		quality[quality_counter]=status;
		quality_counter = (quality_counter+1)%100;
		if (ESPDataLinkClass::on_send && !ESPDataLinkClass::stubborn) ESPDataLinkClass::on_send(!status);
		
	}
