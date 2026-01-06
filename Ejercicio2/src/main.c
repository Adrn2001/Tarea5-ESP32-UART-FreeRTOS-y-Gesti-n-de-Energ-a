/*
 * TAREA 5 - EJERCICIO 2: Sistema Multitarea con FreeRTOS
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// --- DEFINICIONES ---
#define PIN_LED 2
int val_sensor = 0; // Recurso global compartido

// --- TAREA 1: Adquisición de Datos (Sensor Virtual) ---
// Prioridad: 1 (Baja) | Core: Automático
void tarea_sensor(void *pvParameters) {
    while (1) {
        // Generación de valor aleatorio entre 20 y 40
        val_sensor = 20 + (rand() % 21);
        
        // Muestreo cada 2 segundos (No bloqueante)
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

// --- TAREA 2: Control de Actuador (LED) ---
// Prioridad: 1 (Baja) | Core: 0 (PRO_CPU)
void tarea_led(void *pvParameters) {
    gpio_reset_pin(PIN_LED);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);

    while (1) {
        // Secuencia de parpadeo (500ms periodo total)
        gpio_set_level(PIN_LED, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS); 
        
        gpio_set_level(PIN_LED, 0);
        vTaskDelay(250 / portTICK_PERIOD_MS);

        // Debug para verificar ejecución en Core 0
        printf(" -> Actividad LED en Core: %d\n", xPortGetCoreID());
    }
}

// --- TAREA 3: Monitor de Sistema ---
// Prioridad: 2 (Media) | Core: 1 (APP_CPU)
void tarea_monitor(void *pvParameters) {
    while (1) {
        printf("--- ESTADO DEL SISTEMA ---\n");
        printf("Temp. Sensor: %d C\n", val_sensor);
        printf("Estado LED: Activo\n");
        printf("Monitor corriendo en Core: %d\n", xPortGetCoreID());
        printf("--------------------------\n");
        
        // Reporte cada 1 segundo
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// --- PROGRAMA PRINCIPAL ---
void app_main(void) {
    printf("Iniciando Scheduler FreeRTOS...\n");

    // 1. Tarea Sensor: Asignación automática de núcleo
    xTaskCreate(tarea_sensor, "Task_Sensor", 2048, NULL, 1, NULL);
    
    // 2. Tarea LED: Forzada al Núcleo 0 (PRO_CPU)
    xTaskCreatePinnedToCore(tarea_led, "Task_LED", 2048, NULL, 1, NULL, 0);

    // 3. Tarea Monitor: Forzada al Núcleo 1 (APP_CPU) y mayor prioridad
    xTaskCreatePinnedToCore(tarea_monitor, "Task_Monitor", 2048, NULL, 2, NULL, 1);

    printf("Tareas inicializadas.\n");
}