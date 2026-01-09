# Tarea 5: Sistemas Embebidos (ESP32 + FreeRTOS)

Este repositorio contiene el desarrollo de la **Tarea 5**, enfocada en la programación avanzada de microcontroladores ESP32 utilizando el sistema operativo de tiempo real **FreeRTOS** y el framework **ESP-IDF** (PlatformIO).

## 📂 Organización del Repositorio

El proyecto está dividido en 4 ejercicios independientes:

* **📂 /Ejercicio1 (UART):**
    * Sistema de intérprete de comandos por puerto serial.
    * Control de LED mediante comandos de texto ("on", "off", ) sin bloquear el procesador.

* **📂 /Ejercicio2 (Multitarea FreeRTOS):**
    * Implementación de 3 tareas concurrentes (Sensor, Actuador, Monitor).
    * Demostración de uso de **Dual Core**: Tareas asignadas específicamente al *Core 0* (PRO_CPU) y *Core 1* (APP_CPU).

* **📂 /Ejercicio 3 (Ahorro de Energía):**
    * Implementación del modo **Deep Sleep** (Sueño Profundo).
    * Configuración de despertar mediante interrupción externa (Botón en GPIO 33).
    * *Nota: Este ejercicio requiere hardware real para verificar el consumo.*

* **📂 /Ejercicio 4 (Sistema Integrado):**
    * Integración final usando **Colas (Queues)** de FreeRTOS.
    * Arquitectura "Productor-Consumidor" para desacoplar la recepción UART del control de hardware.

## 🛠️ Requisitos de Software y Hardware

* **Placa:** ESP32 DOIT DEVKIT V1
* **IDE:** Visual Studio Code
* **Extensión:** PlatformIO IDE
* **Framework:** Espressif IoT Development Framework (ESP-IDF)

## 🚀 Instrucciones de Compilación y Ejecución

Para probar cualquiera de los ejercicios, siga estos pasos:

1.  **Clonar el repositorio:**
    ```bash
    git clone [https://github.com/Adrn2001/Tarea5-ESP32-UART-FreeRTOS-y-Gesti-n-de-Energ-a.git](https://github.com/Adrn2001/Tarea5-ESP32-UART-FreeRTOS-y-Gesti-n-de-Energ-a.git)
    ```

2.  **Abrir en PlatformIO:**
    * Abra VS Code.
    * Vaya a la extensión de PlatformIO (ícono de la hormiga).
    * Seleccione **"Open Project"** y navegue a la carpeta del ejercicio que desea probar (ej: `Ejercicio2`).

3.  **Compilar y Subir:**
    * Conecte el ESP32 por USB.
    * Haga clic en el botón **Build** (✓) para compilar.
    * Haga clic en el botón **Upload** (→) para subir el código a la placa.

4.  **Monitoreo:**
    * Abra el Monitor Serial (ícono de enchufe) para ver la salida de datos.
    * *Configuración:* Baud Rate 115200.

---
**Autores:** [Alexis Rodríguez,Leonardo Rosero]
**Materia:** Sistemas Embebidos
