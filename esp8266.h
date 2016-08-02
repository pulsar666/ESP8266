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
uint8_t TCPSend;
char str[12];

void wait_ESP(int);
uint32_t strlen(uint8_t *);
int is_subString(uint8_t *,uint8_t *);
char *num2str(int);
void str_cat(uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *);
void getIP(uint8_t *);
void Send_ESP(uint8_t *);
void AT_cmd(uint8_t *);
void create_TCPSocket(uint8_t *,uint8_t *);
void PIN_Init(void);
void Wifi_Status();
void Wifi_init(uint32_t);
void Wifi_connect(uint8_t *,uint8_t *);
void Wifi_disconnect(void);
void send_TCP(uint8_t *,int);
char MQTTpublish(char *topic,unsigned char *payload) ;
