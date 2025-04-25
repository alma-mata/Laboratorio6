/*
 * Universidad del Valle de Guatemala
 * IE2023: Programacion de Microcontroladores
 * Laboratorio6.c
 * Autor: Alma Lisbeth Mata Ixcayau
 * Proyecto: Laboratorio 6
 * Descripcion: Control de servomotores y led con PWM
 * Creado: 24/04/2025
*/
/****************************************/
// Encabezado
#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/****************************************/
// Prototipos de función
void setup(void);
void UART_init(void);
void send_CHAR(unsigned char caracter);
void send_STRING(char* texto);

volatile uint8_t received_RX = 0;
/****************************************/
// Función principal
int main(void) {
	
	setup(); // Configuraciones iniciales
	//send_STRING("Hola\r\n");
	while (1) {
		send_CHAR('H');
		_delay_ms(1000);
	}
}

/****************************************/
// Subrutinas sin Interrupcion
void setup(void){
	cli();
	
	// Entradas y Salidas
	DDRD |= (1 << DDD7) | (1 << DDD6); 
	DDRB = 0xFF; // Todo PORTB como salida
	
	UART_init(); // Configuracion del UART
	
	sei();
}

void UART_init(void){
	UBRR0 = 103;
	
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); //Rx int. | Rxen | Txen
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Async. | No parity | 1 stop | 8 data
}

void send_CHAR(unsigned char caracter){
	while (!(UCSR0A & (1 << UDRE0))); //Revisar si UDR está vacío
	UDR0 = caracter;
}

void send_STRING(char* texto){
	uint8_t i = 0;
	while (texto[i] != '\0'){
		send_CHAR(texto[i]);
		i++;
	}
}
/****************************************/
// Subrutinas de Interrupcion
ISR(USART_RX_vect){
	received_RX = UDR0;
	PORTB = ((received_RX & 0xFC) >> 2);
	PORTD = (PORTD & 0x3F) | ((received_RX & 0x03) << 6);
}