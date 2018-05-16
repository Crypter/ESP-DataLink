#ifndef ESPDatalink_h
#define ESPDatalink_h

typedef struct ESPDataLinkPacket {
  uint8_t data[250];
  uint8_t length;
} ESPDataLinkPacket;


  class ESPDataLinkClass {
	public:
	
	ESPDataLinkClass();
	
	static void init();
	
	static String macToString(const uint8_t* mac);
	
	static void onReceive(void (*funk)(uint8_t *data, uint8_t len));
	
	static void onSend(void (*funk)(uint8_t status));
	
	static int8_t send(uint8_t* data, uint8_t len);
	
	static uint8_t getQuality();
	
	static String getMac();
	
  static void pair(String mac);
  
  static void stubbornMode(bool enforce);

		private:

	static void receive_data(uint8_t *mac, uint8_t *data, uint8_t len);
	static void send_data(uint8_t *mac, uint8_t status);
	
	static void (*on_receive)(uint8_t *data, uint8_t len);
	static void (*on_send)(uint8_t status);
	
	static uint8_t paired[6];
  static uint8_t stubborn;
  static uint8_t ACK;
  static uint8_t SEQtx, SEQrx;
  
  static uint8_t channel;
	static uint8_t quality[100];
	static uint8_t quality_counter;
  };
  
  extern ESPDataLinkClass ESPDataLink;
  
#endif
