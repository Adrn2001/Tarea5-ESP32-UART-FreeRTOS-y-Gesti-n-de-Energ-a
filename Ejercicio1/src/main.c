// Tarea 5 Ejercicio1
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

// --- CONFIGURACIÓN ---

#define ES_SIMULACION 1

#if ES_SIMULACION
    #define TX_PIN (GPIO_NUM_1)
    #define RX_PIN (GPIO_NUM_3)
    #define UART_USADO UART_NUM_0
#else
    
    #define TX_PIN (GPIO_NUM_17)
    #define RX_PIN (GPIO_NUM_16)
    #define UART_USADO UART_NUM_2
#endif

#define LED_INTEGRADO 2
#define TAMANO_BUFFER 256

// Variables globales para el estado
int contador_cmds = 0;
bool estado_led = false;

// Configuración inicial del puerto
void configurar_uart() {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    // Instalamos el driver con un buffer de recepción 
    uart_driver_install(UART_USADO, TAMANO_BUFFER * 2, 0, 0, NULL, 0);
    uart_param_config(UART_USADO, &uart_config);
    uart_set_pin(UART_USADO, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// Funcion auxiliar para mandar texto rapido
void enviar_texto(const char* txt) {
    uart_write_bytes(UART_USADO, txt, strlen(txt));
}

// Logica de los comandos
void ejecutar_comando(char* cmd) {
    enviar_texto("\r\n"); 

    if (strcmp(cmd, "led on") == 0) {
        gpio_set_level(LED_INTEGRADO, 1);
        estado_led = true;
        enviar_texto("OK: Led encendido.\r\n");
        contador_cmds++;
    }
    else if (strcmp(cmd, "led off") == 0) {
        gpio_set_level(LED_INTEGRADO, 0);
        estado_led = false;
        enviar_texto("OK: Led apagado.\r\n");
        contador_cmds++;
    }
    else if (strcmp(cmd, "status") == 0) {
        char msg[50];
        sprintf(msg, "ESTADO: Led=%s | Comandos=%d\r\n", estado_led ? "ON" : "OFF", contador_cmds);
        enviar_texto(msg);
    }
    else if (strcmp(cmd, "info") == 0) {
        enviar_texto("SISTEMA: ESP32 UART2 - Baud: 115200\r\n");
    }
    else if (strcmp(cmd, "reset") == 0) {
        contador_cmds = 0;
        estado_led = false;
        gpio_set_level(LED_INTEGRADO, 0);
        enviar_texto("RESET: Valores reiniciados.\r\n");
    }
    else if (strlen(cmd) > 0) {
        enviar_texto("ERROR: Comando desconocido.\r\n");
    }
    
    // Prompt para escribir de nuevo
    enviar_texto("Comando> ");
}

void app_main() {
    configurar_uart();
    
    // Configurar el pin del LED
    gpio_reset_pin(LED_INTEGRADO);
    gpio_set_direction(LED_INTEGRADO, GPIO_MODE_OUTPUT);

    // Variables de lectura
    uint8_t caracter;
    char buffer_entrada[TAMANO_BUFFER];
    int indice = 0;

    enviar_texto("\r\n--- INICIANDO SISTEMA ---\r\n");
    enviar_texto("Comando> "); 

    while (true) {
        // Lee byte a byte (no bloqueante, espera max 20ms)
        int len = uart_read_bytes(UART_USADO, &caracter, 1, 20 / portTICK_PERIOD_MS);

        if (len > 0) {
            // Si es Enter (\r o \n) procesamos
            if (caracter == '\n' || caracter == '\r') {
                buffer_entrada[indice] = 0; // Cerramos el string
                ejecutar_comando(buffer_entrada);
                indice = 0; // Reset del buffer
            }
            // Si es Backspace (borrar)
            else if (caracter == '\b' || caracter == 127) {
                if (indice > 0) {
                    indice--;
                    uart_write_bytes(UART_USADO, "\b \b", 3); // Borrado visual
                }
            }
            // Caracter normal
            else {
                if (indice < TAMANO_BUFFER - 1) {
                    buffer_entrada[indice] = (char)caracter;
                    indice++;
                    uart_write_bytes(UART_USADO, (const char*)&caracter, 1); 
                }
            }
        }
    }
}