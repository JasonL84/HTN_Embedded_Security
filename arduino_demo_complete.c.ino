#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Ed25519.h>

byte mac_addr[] = { 0x98, 0x4f, 0xEE, 0x01, 0xca, 0xea };

IPAddress server(192, 168, 1, 29);

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 177);
IPAddress dns(192, 168, 1, 122);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
byte challenge_buffer[4096];
byte response_buffer[64];
byte priv_key_buffer[32];
byte pub_key_buffer[32];


// HELPER FUNCTIONS
void init_ethernet()
{
  Serial.println("test");
  system("ifconfig eth0 up");
  system("ifup eth0");
  Ethernet.begin(mac_addr, ip);
}

void read_from_EEPROM(int addr, int len, byte* buf)
{
  for(int i = 0; i < len; i++)
  {
    buf[i] = EEPROM.read(addr+i);
  }
}

int load_keys()
{
  /*
   * Loads the private key from EEPROM memory to a buffer.
   * param key: The address to a 32 byte size buffer.
   * return: 0 on success, 1 on failure
   */
  read_from_EEPROM(0, 32, pub_key_buffer);
  read_from_EEPROM(32, 32, priv_key_buffer);
  return 0;
}

// CRYPTO FUNCTION
int generate_signature(byte* challenge, int challenge_len)
{
  /*
   * Generate a signature from the given challenge.
   * return: Size of generated signature in bytes, -1 on failure
   */
  Ed25519::sign(response_buffer, priv_key_buffer, pub_key_buffer, challenge, challenge_len);

  if(!Ed25519::verify(response_buffer, pub_key_buffer, challenge, challenge_len))
  {
    return -1;
  }
  return 64;
}

// ARDUINO SPECIFIC FUNCTIONS
void setup() {
  Serial.begin(9600);

  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  init_ethernet();
  load_keys();
  Serial.println("connecting...");

}

void loop() {
  if (client.connect(server, 19997)) {
    int len = -1;
    int amount_read = 0;
    while(len < 0)
    {
      //spin lock for until there is data to read
      len = client.available();
    }

    len = client.read();
    for(int i = 0; i < len; i++)
    {
      challenge_buffer[i] = client.read();
      if(challenge_buffer[i] == 0xFF){
        i--;
      }
    }
    Serial.println();

    
    int sig_len = generate_signature(challenge_buffer, len);

    if(sig_len > 0)
    {
      client.write(response_buffer, sig_len);
    }
    else
    {
      Serial.println("Something went wrong here");
      client.stop();
      Serial.write(challenge_buffer, len);
      Serial.print("Length of challenge recieved ");
      Serial.print(len, DEC);
      //Something went wrong, should probably start panicking
    }

    if(client.connected())
    {
      byte resp[8];
      int len = -1;
  
      while(len < 0)
      {
        //spin lock for until there is data to read
        len = client.available();
      }

      len = client.read();
      if(len > 8) len = 8;

      for(int i = 0; i < len; i++)
      {
        resp[i] = client.read();
        if(resp[i] == 0xFF){
          i--;
          continue;
        }
      }
      Serial.println();
      Serial.write(resp, len);
      client.write("PONG");
    }

    client.stop();
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    system("ifconfig -a > /dev/ttyGS0");
    // system("ping 192.168.1.29 > /dev/ttyGS0");
    client.stop();
  }
  sleep(5);
}
