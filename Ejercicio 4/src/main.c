/*
 * TAREA 5 - EJERCICIO 4: Integración UART + FreeRTOS + Queues

 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/uart.h"

// --- CONFIGURACIÓN ---
#define PIN_LED 2
#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024
#define TX_PIN 1
#define RX_PIN 3

// Tipos de Comandos (Mensajes que viajaran por la cola)
typedef enum {
    CMD_NINGUNO,
    CMD_ENCENDER_LED,
    CMD_APAGAR_LED
} comando_t;

// Variable global para la Cola (Queue Handle)
QueueHandle_t cola_comandos;

// Variable global de estado (para el reporte)
bool led_estado_actual = false;

// --- FUNCIONES AUXILIARES ---
void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// --- TAREA 1: Recepción e Interpretación UART ---
// Lee el teclado, interpreta el texto y envia el "ID" del comando a la cola.
void tarea_uart_rx(void *pvParameters) {
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    char buffer_cmd[50];
    int idx = 0;

    printf("\n--- SISTEMA INTEGRADO LISTO ---\n");
    printf("Escribe: 'led on' o 'led off'\n");

    while (1) {
        // Lee byte por byte
        int len = uart_read_bytes(UART_PORT, data, 1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            char c = (char)data[0];
            
            // Eco visual
            uart_write_bytes(UART_PORT, &c, 1);

            if (c == '\n' || c == '\r') {
                buffer_cmd[idx] = '\0'; // Cerrar string
                comando_t comando_enviar = CMD_NINGUNO;

                // Interpretar comando
                if (strcmp(buffer_cmd, "led on") == 0) {
                    comando_enviar = CMD_ENCENDER_LED;
                    printf("\n[UART] Comando detectado: ENCENDER -> Enviando a Cola...\n");
                } 
                else if (strcmp(buffer_cmd, "led off") == 0) {
                    comando_enviar = CMD_APAGAR_LED;
                    printf("\n[UART] Comando detectado: APAGAR -> Enviando a Cola...\n");
                }
                else {
                    printf("\n[UART] Comando desconocido.\n");
                }

                // Si hay comando valido, enviarlo a la cola
                if (comando_enviar != CMD_NINGUNO) {
                    // xQueueSend(Cola, Puntero al dato, Tiempo de espera)
                    xQueueSend(cola_comandos, &comando_enviar, portTICK_PERIOD_MS * 10);
                }
                
                idx = 0; // Reset buffer
                printf("Comando> ");
            } 
            else if (idx < 49) {
                buffer_cmd[idx++] = c;
            }
        }
    }
    free(data);
}

// --- TAREA 2: Ejecución (Consume de la Cola) ---
// Esta tarea está "dormida" hasta que llega algo a la cola.
void tarea_controlador(void *pvParameters) {
    gpio_reset_pin(PIN_LED);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    
    comando_t comando_recibido;

    while (1) {
        // xQueueReceive bloquea la tarea hasta que llegue un dato
        if (xQueueReceive(cola_comandos, &comando_recibido, portMAX_DELAY)) {
            
            printf("[CONTROL] Mensaje recibido de la cola. Ejecutando...\n");
            
            if (comando_recibido == CMD_ENCENDER_LED) {
                gpio_set_level(PIN_LED, 1);
                led_estado_actual = true;
            } 
            else if (comando_recibido == CMD_APAGAR_LED) {
                gpio_set_level(PIN_LED, 0);
                led_estado_actual = false;
            }
        }
    }
}

// --- TAREA 3: Reporte Periódico ---
void tarea_monitor(void *pvParameters) {
    while (1) {
        printf("[MONITOR] Estado Sistema: LED %s | Memoria Libre: %d bytes\n", 
               led_estado_actual ? "ON" : "OFF", 
               (int)esp_get_free_heap_size());
        
        vTaskDelay(3000 / portTICK_PERIOD_MS); // Reporte cada 3 seg
    }
}

void app_main(void) {
    init_uart();

    // 1. Crear la cola (Máximo 10 elementos de tipo comando_t)
    cola_comandos = xQueueCreate(10, sizeof(comando_t));

    if (cola_comandos == NULL) {
        printf("Error creando la cola.\n");
        return;
    }

    // 2. Crear las tareas
    // Tarea UART (Prioridad baja)
    xTaskCreate(tarea_uart_rx, "UART_Rx", 4096, NULL, 1, NULL);
    
    // Tarea Controlador (Prioridad Alta - Para reaccionar rápido)
    xTaskCreate(tarea_controlador, "Control_LED", 2048, NULL, 2, NULL);
    
    // Tarea Monitor (Prioridad Baja)
    xTaskCreate(tarea_monitor, "Monitor", 2048, NULL, 1, NULL);
}