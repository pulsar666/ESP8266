#include "lpc_types.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_timer.h"


#define RXBUFF_LEN 100
#define TXBUFF_LEN 100

uint8_t rxbuf[RXBUFF_LEN];
uint8_t txbuf[TXBUFF_LEN];
uint8_t response[100];

uint8_t WIFIStat; /* on = 1, off = 0 */
uint8_t WIFIInit; /* WIFI not initialised = 0, WIFI initialised = 1*/
uint8_t TCPSocket;/* TCP SOCKET NOT CREATED= 0,TCP SOCKET CREATED = 1*/
uint8_t IP[16];
uint8_t TCPSend; /* 1 indicates data was successfully sent to ESP8266 */
char str[12]; /* For num2str output */


/* Wait for arg1 milliseconds */
void wait_ESP(int);

/* returns the length of string arg1 */
uint32_t strlen(uint8_t *);

/* returns the position of substring arg2 in superstring arg1 */
int is_subString(uint8_t *,uint8_t *);

/* Convert integer(arg1) and stores in str[] */
char *num2str(int);

/* Concatenates strings arg2,arg3,arg4,arg5 and puts it into arg1 */
void str_cat(uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *);

/* get the IP address of your ESP8266 in the current network(provided you have internet connection)
 * arg1 - address of string where IP is to be stored
 */
void getIP(uint8_t *);

/* Send data to ESP. arg1 - data */
void Send_ESP(uint8_t *);

/* Send AT commands to the ESP module
 * arg1 - "<command>\r\n" eg. - "AT\r\n"
 */
void AT_cmd(uint8_t *);

/* Create a TCP/IP socket
 * arg1- IP address of server
 * arg2 - Port no.
 * eg. - arg1 - "\"test.mosquitto.org\"" , arg2 - "1883"
 */
void create_TCPSocket(uint8_t *,uint8_t *);

/*
 * Initialise UART pins(can modify for your own pins/UART channel
 */
void PIN_Init(void);

/*
 * Get Wifi connectivity status in rxbuf
 */
void Wifi_Status();

/*
 * Initialise Wifi module
 * arg1 - baud rate
 */
void Wifi_init(uint32_t);

/*
 * Connect to a Wifi
 * arg1 - SSID e.g. "John's WIFI"
 * arg2 - password e.g "XYZ@123"
 */
void Wifi_connect(uint8_t *,uint8_t *);

/*
 * Disconnect from current WiFi
 */
void Wifi_disconnect(void);

/*
 * Send data through TCP socket (use createTCPSocket() before this)
 * arg1 - data
 * arg2 - length of data
 */
void send_TCP(uint8_t *,int);
