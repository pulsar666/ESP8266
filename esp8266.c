#include "colors.h"
#include "lpc_types.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "SSD1289.h"
#include "LCD_delay.h"
#include "lpc17xx_timer.h"

#include "MQTTConnect.h"
#include "MQTTPacket.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"
#include "StackTrace.h"

#define RXBUFF_LEN 200
#define TXBUFF_LEN 200

uint8_t rxbuf[RXBUFF_LEN];
uint8_t txbuf[TXBUFF_LEN];
uint8_t response[100];
uint8_t IP[16];
uint8_t WIFIStat; /* on = 1, off = 0 */
uint8_t TCPSocket;
uint8_t TCPSend;
uint8_t WIFIInit;
uint8_t DataRcv;
char str[12];
int rcvlen;


void wait_ESP(int delay) {
	int i = 10000, j = delay;
	while (i > 0) {
		while (j > 0)
			j--;
		j = delay;
		i--;
	}
}
uint32_t strlen(uint8_t *str) {
	uint32_t len = 0;
	while (str[len] != '\0')
		len++;
	return len;
}
int is_subString(uint8_t *str, uint8_t *sub) {
	int i = 0, j = 0, flag = 0;
	while (str[i] != '\0') {
		if (sub[0] == str[i]) {
			j = 0;
			flag = 0;
			while (sub[j] != '\0' && str[i + j] != '\0') {
				if (sub[j] != str[i + j]) {
					flag = 1;
					break;
				}
				j++;
			}
			if (flag == 0)
				if (strlen(sub) == j)
					return i;
		}
		i++;
	}
	return -1;
}

char *num2str(int num) {
	int i = 0, p = 0;
	char temp[11];
	for (i = 0; i < 10; i++) {
		temp[i] = ' ';
		str[i] = ' ';
	}
	temp[10] = '\0';
	if (num == 0) {
		str[0] = '0';
		str[1] = '\0';
		return str;
	}
	if (num < 0) {
		temp[0] = '-';
		num *= -1;
	} else
		temp[0] = ' ';
	i = 0;
	while (num > 0) {
		temp[9 - i] = (char) ((num % 10) + 48);
		num /= 10;
		i++;
	}
	for (i = 0; temp[i] != '\0'; i++) {
		if (temp[i] == ' ')
			continue;
		else {
			str[p] = temp[i];
			p++;
		}
	}
	str[p] = '\0';
	return str;
}

void str_cat(uint8_t *str, uint8_t *str1, uint8_t *str2, uint8_t *str3,
		uint8_t *str4, uint8_t *str5) {
	int i = 0, j = 0;
	while (str1[i] != '\0') {
		str[i] = str1[i];
		i++;
	}
	while (str2[j] != '\0') {
		str[i] = str2[j];
		i++;
		j++;
	}
	j = 0;
	while (str3[j] != '\0') {
		str[i] = str3[j];
		i++;
		j++;
	}
	j = 0;
	while (str4[j] != '\0') {
		str[i] = str4[j];
		i++;
		j++;
	}
	j = 0;
	while (str5[j] != '\0') {
		str[i] = str5[j];
		i++;
		j++;
	}
	str[i] = '\0';
}
void getIP(uint8_t *str) {
	int pos = is_subString(str, (uint8_t *) "STAIP"), i = 0;

	while (str[pos + 7] != '\"') {
		IP[i] = str[pos + 7];
		i++;
		pos++;
	}
	IP[i] = '\0';
}
void UART0_IRQHandler() {
	rcvlen = UART_Receive(LPC_UART0, rxbuf, sizeof(rxbuf), BLOCKING);
	//LCD_PutString((char *)rxbuf,12,RED);
	if (is_subString(rxbuf, (uint8_t *) "WIFI GOT IP") >= 0)
		WIFIStat = 1;
	if (is_subString(rxbuf, (uint8_t *) "WIFI DISCONNECT") >= 0)
		WIFIStat = 0;
	if (is_subString(rxbuf, (uint8_t *) "SEND OK") >= 0)
		TCPSend = 1;
	if (is_subString(rxbuf, (uint8_t *) "+IPD,") >= 0)
		DataRcv = 1;
	/*if(is_subString(rxbuf,(uint8_t *)"WIFI DISCONNECT") >= 0)
	 LCD_PutString("\nWiFi DISCONNECTED",12,RED);
	 if(is_subString(rxbuf,(uint8_t *)"WIFI CONNECTED") >= 0)
	 LCD_PutString("\nWiFi CONNECTED",12,BLUE);*/
}

void Send_ESP(uint8_t* cmd, int size) {
	while (UART_CheckBusy(LPC_UART0) == SET)
		;
	UART_Send(LPC_UART0, cmd, size, BLOCKING);
}

void AT_cmd(uint8_t *cmd) {
	//uint8_t *res;
	//int i=0;
	//LCD_PutString("\nSending ",12,WHITE);
	//LCD_PutString((char *)cmd,12,RED);
	while (UART_CheckBusy(LPC_UART0) == SET)
		;
	/*while(i<sizeof(rxbuf)) {
	 rxbuf[i] = '\0';
	 i++;
	 }*/
	Send_ESP(cmd, strlen(cmd));
	wait_ESP(200);
}

void create_TCPSocket(uint8_t *IP, uint8_t *port) {
	uint8_t cmd[50];
	int timeout=20;
	int i;

	while (i < sizeof(cmd)) {
		cmd[i] = '\0';
		i++;
	}
	str_cat(cmd, (uint8_t *) "AT+CIPSTART=", (uint8_t *) "\"TCP\",", IP,
								port, (uint8_t *) "\r\n");
	AT_cmd((uint8_t *) "AT+CIPSTATUS?\r\n");
		//LCD_PutString((char *)rxbuf,12,RED);

	if (is_subString(rxbuf, (uint8_t *)"ERROR") >= 0) {
			while(TCPSocket!=1&&timeout!=0) {
				AT_cmd((uint8_t *) "AT+CIPMUX=0\r\n");
				AT_cmd(cmd);
				AT_cmd((uint8_t *)"AT+CIPSTATUS?\r\n");
				//LCD_PutString((char *)rxbuf,12,RED);
				if (is_subString(rxbuf, (uint8_t *) "CONNECT") >= 0)
					TCPSocket = 1;
				else {
					TCPSocket = 0;
					timeout--;
				}
			}
	}
}

void close_TCPSocket() {
	AT_cmd((uint8_t *) "AT+CIPCLOSE\r\n");
	TCPSocket = 0;
}

void PIN_Init(void) {
	/* Pin configuration for UART0 */
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize UART0 pin connect
	 */
	//LCD_PutString("\nInitialising UART PINS",12,WHITE);
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum = PINSEL_PORT_0;
	PinCfg.Pinnum = PINSEL_PIN_2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = PINSEL_PIN_3;
	PINSEL_ConfigPin(&PinCfg);
}

void Wifi_Status() {
	AT_cmd((uint8_t *) "AT+CIFSR\r\n");
}

void Wifi_init(uint32_t baud) {

	/* UART Configuration structure variable */
	UART_CFG_Type UARTConfigStruct;
	/* UART FIFO configuration Struct variable */
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	/* Set Baudrate to 115200 */
	(&UARTConfigStruct)->Baud_rate = baud;
	(&UARTConfigStruct)->Databits = UART_DATABIT_8;
	(&UARTConfigStruct)->Parity = UART_PARITY_NONE;
	(&UARTConfigStruct)->Stopbits = UART_STOPBIT_1;
	PIN_Init();
	/* Initialize UART0 peripheral with given to corresponding parameter */
	UART_Init(LPC_UART0, &UARTConfigStruct);
	SystemCoreClockUpdate();
	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	(&UARTFIFOConfigStruct)->FIFO_Level = UART_FCR_TRG_LEV0; /* 8 Bytes */
	//(&UARTFIFOConfigStruct)->FIFO_ResetRxBuf = DISABLE;
	/* Initialize FIFO for UART0 peripheral */
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);

	/** config and enable interrupter */
	UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);

	/** NVIC enable interrupter , it's request锛�*/
	NVIC_EnableIRQ(UART0_IRQn);

	/*  Enable UART Transmit */
	UART_TxCmd(LPC_UART0, ENABLE);

	while(is_subString(rxbuf, (uint8_t *) "OK") < 0) {
		AT_cmd((uint8_t *) "AT\r\n");
	}
	WIFIInit = 1;
	//Wifi_Status();
}
void Wifi_connect(uint8_t *usr, uint8_t *pass) {
	uint8_t cmd[100];
	str_cat(cmd, (uint8_t *) "AT+CWJAP_DEF=\"", usr, (uint8_t *) "\",\"", pass,
			(uint8_t *) "\"\r\n");
	AT_cmd(cmd);
	Wifi_Status();
}
void Wifi_disconnect(void) {
	AT_cmd((uint8_t *) "AT+CWQAP\r\n");
	//AT_cmd((uint8_t *)"AT+RST");
	WIFIStat = 0;
}

void send_TCP(uint8_t *data, int len) {
	uint8_t *buf_len = (uint8_t *) num2str(len);
	uint8_t cmd[50];
	str_cat(cmd, (uint8_t *) "AT+CIPSEND=", buf_len, (uint8_t *) "\r\n",
			(uint8_t *) "\0", (uint8_t *) "\0");

	create_TCPSocket((uint8_t *) "\"test.mosquitto.org\",", (uint8_t *) "1883");
	//LCD_PutString(rxbuf,12,RED);
		/*if (TCPSocket == 1)
			LCD_PutString("TCP Socket Created\n", 12, BLUE);
		else
			LCD_PutString("TCP Socket Failed\n", 12, RED);*/
	AT_cmd(cmd);
	/* while (ESPRecReady != 1);
		ESPRecReady = 0;
	 */
	UART_Send(LPC_UART0, data, len, BLOCKING);
	TCPSocket =0;
	//LCD_PutString((char *)rxbuf,12,RED);
}

int getData(unsigned char *buf, int len) {
	int rlen=0;
	while(DataRcv!=1);
	while(rxbuf[i]!=',') {
		i++;
	}
	i++;
	while(rxbuf[i]!=':') {
		rlen = rlen*10+(rxbuf[i]-48);
		i++;
	}
	i++;
	len = MIN(len,rlen);
	for(j=0;j<len;j++) {
		buf[j]= rxbuf[i+j];
	}
	return len;
}


