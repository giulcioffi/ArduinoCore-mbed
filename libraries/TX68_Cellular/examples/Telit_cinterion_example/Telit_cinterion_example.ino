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

#define print_function printf
//REDIRECT_STDOUT_TO(Serial);
const char  host_name[] = "www.google.com";
//port_t port = 80;
nsapi_error_t test_send_recv()
{
  nsapi_size_or_error_t retcode;

  TCPSocket sock;


  Serial.println("Before open socket");

  retcode = sock.open(((mbed::CellularContext*)iface));

  if (retcode != NSAPI_ERROR_OK) {
    print_function("TCPSocket.open() fails, code: %d\n", retcode);
    return -1;
  }

  int n = 1;
  const char *echo_string = "TEST";
  char recv_buf[5];

  sock.set_timeout(15000);

  SocketAddress sock_addr;
  //    retcode = iface->gethostbyname(host_name, &sock_addr);
  //    if (retcode != NSAPI_ERROR_OK) {
  //      print_function("Couldn't resolve remote host: %s, code: %d\n", host_name, retcode);
  //      return -1;
  //    }
  sock_addr.set_ip_address("104.18.28.45");
  //sock_addr.set_url_address("http://www.arduino.cc:80/asciilogo.txt");
  sock_addr.set_port(80);


  retcode = sock.connect(sock_addr);
  if (retcode < 0) {
    print_function("TCPSocket.connect() fails, code: %d\n", retcode);
    return -1;
  } else {
    print_function("TCP: connected with %s server\n", "104.18.28.45");
  }
  //    retcode = sock.send((void*) echo_string, strlen(echo_string));
  //    if (retcode < 0) {
  //      print_function("TCPSocket.send() fails, code: %d\n", retcode);
  //      return -1;
  //    } else {
  //      print_function("TCP: Sent %d Bytes to %s\n", retcode, host_name);
  //    }
  //delay(10000);
  while (retcode == 0) {
    n = sock.recv((void*) recv_buf, sizeof(recv_buf));
    Serial.println();
    Serial.println("-----------------------------------------------------");
    Serial.println(n);
    Serial.println(recv_buf);
    Serial.println("-----------------------------------------------------");
    if (n > 0) {
      Serial.write(recv_buf, n);
    } else {
      break;
    }
  }
  sock.close();

  if (n > 0) {
    print_function("Received from echo server %d Bytes\n", n);
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
    printf("during connect\n");
    retcode = ((mbed::CellularContext*)iface)->connect();
    printf("after connect\n");
    if (retcode == NSAPI_ERROR_AUTH_FAILURE) {
      print_function("\n\nAuthentication Failure. Exiting application\n");
    } else if (retcode == NSAPI_ERROR_OK) {
      print_function("\n\nConnection Established.\n");
    } else if (retry_counter > 2) {
      print_function("\n\nFatal connection failure: %d\n", retcode);
    } else {
      print_function("\n\nCouldn't connect: %d, will retry\n", retcode);
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
  //iface->set_default_parameters();

  Serial.println("here");
  //iface->set_sim_pin("0000");
  //
  if (do_connect() == NSAPI_ERROR_OK) {
    test_send_recv();
  }

  Serial.println("loop");
}

void loop() {
  // put your main code here, to run repeatedly:

}
