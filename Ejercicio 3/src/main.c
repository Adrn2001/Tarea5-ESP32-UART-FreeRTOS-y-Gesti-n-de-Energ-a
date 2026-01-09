/*
 * TAREA 5 - EJERCICIO 3: Deep Sleep con Botón Físico
 * Hardware: ESP32 + Botón en GPIO 33 + LED integrado (GPIO 2)
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h" // Necesario para controlar pines en sueño
#include "driver/gpio.h"

#define PIN_BOTON_WAKEUP GPIO_NUM_33 
#define PIN_LED 2

void app_main(void) {
    // 1. Configurar LED
    gpio_reset_pin(PIN_LED);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);

    // 2. ¿Por qué nos despertamos?
    esp_sleep_wakeup_cause_t causa = esp_sleep_get_wakeup_cause();

    if (causa == ESP_SLEEP_WAKEUP_EXT0) {
        printf("\n!!! DESPIERTO POR EL BOTON !!!\n");
        
        // Señal visual de éxito: 3 parpadeos lentos
        for(int i=0; i<3; i++){
            gpio_set_level(PIN_LED, 1);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(PIN_LED, 0);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    } else {
        printf("\n=== ARRANQUE NORMAL (Power On / Reset) ===\n");
        
        // Señal de bienvenida: Parpadeo rápido
        for(int i=0; i<10; i++){
            gpio_set_level(PIN_LED, 1);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(PIN_LED, 0);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }

    // 3. Cuenta regresiva antes de dormir
    printf("El sistema se dormira en 5 segundos...\n");
    for(int i=5; i>0; i--) {
        printf("%d...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // 4. Preparar el despertar (WakeUp)
    printf("Configurando boton en GPIO 33...\n");

    // IMPORTANTE: Habilitar resistencia Pull-Down interna
    // Esto evita que el pin 'flote' y se despierte solo por ruido eléctrico.
    rtc_gpio_pulldown_en(PIN_BOTON_WAKEUP); 
    
    // Configurar EXT0 para despertar cuando el pin esté en ALTO (1 logic)
    // Cuando presiones el botón, le entrarán 3.3V y se activará.
    esp_sleep_enable_ext0_wakeup(PIN_BOTON_WAKEUP, 1); 

    printf("Zzz... Durmiendo. Presiona el boton para despertar.\n");
    
    // Apagar LED y entrar en sueño profundo
    gpio_set_level(PIN_LED, 0);
    esp_deep_sleep_start();
}