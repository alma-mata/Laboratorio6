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
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/****************************************/
// Prototipos de función
void setup(void);
void UART_init(void);
void ADC_init(void);
void send_CHAR(unsigned char caracter);
void send_STRING(char* texto);
void show_MENU(void);
void send_ASCII(uint8_t dato);
uint16_t read_ADC();

volatile uint8_t received_RX = 0;
volatile uint8_t dato_ENVIADO = 0;
volatile uint16_t valor_ADC = 0;
volatile uint8_t modo_ASCII = 0;
/****************************************/
// Función principal
int main(void) {
	
	setup(); // Configuraciones iniciales
	show_MENU();
	
	while (1) {
		if (dato_ENVIADO)
		{
			if (!modo_ASCII)
			{
				if (received_RX == '1')
				{
					valor_ADC = read_ADC();
					char BUFF[20];
					sprintf(BUFF, "\n Valor ADC: %u\n", valor_ADC);
					send_STRING(BUFF);
					show_MENU();
				}
				else if (received_RX == '2')
				{
					modo_ASCII = 1;
					send_STRING("\n Introduzca un caracter: ");
				}
				else{
					send_STRING("\n Debe seleccionar ´1´ o ´2´ \n");
					show_MENU();
				}
			}
			else {
				send_CHAR(received_RX);
				send_CHAR('\n');
				send_ASCII(received_RX);
				modo_ASCII = 0;
				show_MENU();
			}
			dato_ENVIADO = 0;
		}
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
	ADC_init();
	
	sei();
}

void UART_init(void){
	UBRR0 = 103;
	
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); //Rx int. | Rxen | Txen
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Async. | No parity | 1 stop | 8 data
}

void ADC_init(void){
	ADMUX = (1 << REFS0);  // VCC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // PRESCALER 128
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

void show_MENU(void){
	send_STRING("\nElija una opcion:\n");
	send_STRING("1. Leer potenciometro\n");
	send_STRING("2. Enviar ASCII\n");
}

void send_ASCII(uint8_t dato){
	PORTB = ((dato & 0xFC) >> 2);
	PORTD = (PORTD & 0x3F) | ((dato & 0x03) << 6);
}

uint16_t read_ADC() {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}
/****************************************/
// Subrutinas de Interrupcion
ISR(USART_RX_vect){
	received_RX = UDR0;
	dato_ENVIADO = 1;
}