#include "mbed.h"
#include "trace.h"
#include "CellularContext.h"
#include "GEMALTO_CINTERION.h"
#include "GENERIC_AT3GPP.h"

#define MBED_CONF_GEMALTO_CINTERION_TX    PA_0
#define MBED_CONF_GEMALTO_CINTERION_RX    PI_9
#define MBED_CONF_GEMALTO_CINTERION_RTS   PI_10
//#define MBED_CONF_GEMALTO_CINTERION_CTS   PI_13
#define MBED_CONF_APP_SOCK_TYPE           1

mbed::DigitalOut on(PJ_7, 1);
mbed::DigitalOut rts(MBED_CONF_GEMALTO_CINTERION_RTS, 0);
REDIRECT_STDOUT_TO(Serial);

#include "drivers/BufferedSerial.h"
mbed::CellularDevice *mbed::CellularDevice::get_default_instance()
{
  Serial.println("default instance called");
  static BufferedSerial serial(MBED_CONF_GEMALTO_CINTERION_TX, MBED_CONF_GEMALTO_CINTERION_RX, 115200);
#if defined(MBED_CONF_GEMALTO_CINTERION_RTS) && defined(MBED_CONF_GEMALTO_CINTERION_CTS)
  serial.set_flow_control(mbed::SerialBase::RTSCTS, MBED_CONF_GEMALTO_CINTERION_RTS, MBED_CONF_GEMALTO_CINTERION_CTS);
#endif
  static GEMALTO_CINTERION device(&serial);
  //static GENERIC_AT3GPP device(&serial);
  return &device;
}

NetworkInterface *iface;

//REDIRECT_STDOUT_TO(Serial);
const char  host_name[] = "www.google.com";
const char ip_address[] = "104.18.28.45";
const char url_address[] = "http://www.arduino.cc:80/asciilogo.txt";
int port = 80;
nsapi_error_t test_send_recv()
{
  nsapi_size_or_error_t retcode;

  TCPSocket sock;


  Serial.println("Before open socket");

  retcode = sock.open(((mbed::CellularContext*)iface));

  if (retcode != NSAPI_ERROR_OK) {
    Serial.print("TCPSocket.open() fails, code: ");
    Serial.println(retcode);
    return -1;
  }

  int n = 1;
  char recv_buf[1400];

  sock.set_timeout(15000);

  SocketAddress sock_addr;
  sock_addr.set_ip_address(ip_address);
  sock_addr.set_url_address(url_address);
  sock_addr.set_port(port);


  retcode = sock.connect(sock_addr);
  if (retcode < 0) {
    Serial.print("TCPSocket.connect() fails, code: ");
    Serial.println(retcode);
    return -1;
  } else {
    Serial.print("TCP: connected with server ");
    Serial.println(ip_address);
  }
      /*
      const char *echo_string = "GET /asciilogo.txt HTTP/1.1";
      retcode = sock.send((void*) echo_string, strlen(echo_string));
      echo_string = "Host www.arduino.cc";
      retcode = sock.send((void*) echo_string, strlen(echo_string));
      echo_string = "Connection: close";
      retcode = sock.send((void*) echo_string, strlen(echo_string));
      echo_string = "\n";
      retcode = sock.send((void*) echo_string, strlen(echo_string));
      
      if (retcode < 0) {
        Serial.print("TCPSocket.send() fails, code: ");
        Serial.println(retcode);
        return -1;
      } else {
        Serial.print("TCP: Sent ");
        Serial.print(retcode);
        Serial.print(" bytes to ");
        Serial.println(host_name);
      }
      */
      
      
      const char *echo_string = "GET http://www.arduino.cc:80/asciilogo.txt HTTP/1.1";
      retcode = sock.send((void*) echo_string, strlen(echo_string));
      
  while (1) {
    n = sock.recv((void*) recv_buf, sizeof(recv_buf));
    Serial.println();
    Serial.println("-----------------------------------------------------");
    Serial.println(n);
    for (int i = 0; i < n; i++) {
      Serial.print(recv_buf[i]);
    }
    Serial.println("-----------------------------------------------------");
    if (n < 1) {
      break;
    }
  }
  sock.close();

  if (n > 0) {
    Serial.print(n);
    Serial.println(" bytes received from echo server");
    return 0;
  }
  return -1;
}

nsapi_error_t do_connect()
{
  nsapi_error_t retcode = NSAPI_ERROR_OK;
  uint8_t retry_counter = 0;
  ((mbed::CellularContext*)iface)->set_authentication_type((mbed::CellularContext::AuthenticationType)1);
  ((mbed::CellularContext*)iface)->set_credentials("lpwa.pelion", "streamip", "streamip");
  while (iface->get_connection_status() != NSAPI_STATUS_GLOBAL_UP) {
    Serial.println("Connecting...\n");
    retcode = ((mbed::CellularContext*)iface)->connect();
    Serial.println();
    if (retcode == NSAPI_ERROR_AUTH_FAILURE) {
      Serial.println("Authentication Failure. Exiting application.");
    } else if (retcode == NSAPI_ERROR_OK) {
      Serial.println("Connection Established.");
    } else if (retry_counter > 2) {
      Serial.print("Fatal connection failure: ");
      Serial.println(retcode);
    } else {
      Serial.println("Couldn't connect, will retry");
      retry_counter++;
      continue;
    }
    break;
  }
  return retcode;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  on = 0;
  delay(200);
  on = 1;

  trace_open();
  iface = mbed::CellularContext::get_default_instance();
  delay(1000);
  ((mbed::CellularDevice*)iface)->init();
  
  if (do_connect() == NSAPI_ERROR_OK) {
    test_send_recv();
  }
}

void loop() {

}
