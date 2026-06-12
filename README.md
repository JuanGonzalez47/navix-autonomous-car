# 🚗 Navix — Vehículo de Navegación Autónoma Multicore

Navix es una plataforma robótica móvil autónoma impulsada por el microcontrolador **Raspberry Pi Pico (RP2040)**. Diseñado con una filosofía de hardware y software modular, Navix resuelve problemas de robótica embebida mediante el uso intensivo de procesamiento concurrente (Dual-Core).

---

## 📖 a. Descripción del Sistema Final y sus Aplicaciones
El sistema final es un vehículo de exploración terrestre capaz de operar en dos modos independientes:
1. **Evasión Activa de Obstáculos:** Navegación en entornos cerrados utilizando un radar ultrasónico dinámico montado sobre un servomotor, escaneando el entorno en 180° para la toma de decisiones en tiempo real.
2. **Navegación Global (Go-To-Goal):** Desplazamiento hacia coordenadas satelitales remotas ingresadas vía Bluetooth, calculando la distancia (Haversine) y corrigiendo el rumbo dinámicamente mediante una brújula magnética.

**Aplicaciones:** Bases para robótica de exploración espacial (rovers), automatización logística en almacenes y sistemas de micro-navegación agrícola.

---

## 🛠 b. Arquitectura de Hardware (Diagrama de Bloques)

El sistema centraliza el procesamiento en la RP2040 operando a 3.3V, integrando múltiples buses de comunicación en paralelo:

| Componente | Función en el Sistema | Protocolo / Pin |
| :--- | :--- | :--- |
| **Raspberry Pi Pico** | Cerebro Central (RP2040) Dual-Core | N/A |
| **TB6612FNG** | Driver de Potencia de Motores | PWM / GPIO |
| **HC-SR04 + SG90** | Radar Ultrasónico Activo | GPIO (Trig/Echo) + PWM |
| **GY-511** | Brújula Magnética (Corrección de Rumbo) | I2C |
| **NEO-6M** | Módulo GPS (Posicionamiento) | UART |
| **HC-05** | Módulo Bluetooth (Telemetría/Comandos) | UART |

**Explicación HW:** El diseño aísla la lógica (Power Bank 5V regulado a 3.3V) de la etapa de potencia de los motores (Li-ion 7.4V). Se implementaron divisores de tensión para proteger los pines de la Pico frente a las señales de retorno de 5V del HC-SR04 y del Bluetooth.

---

## 💻 c. Arquitectura de Firmware (Diagrama de Flujo Multicore)

Para cumplir con la exigencia de tiempo real sin utilizar un RTOS, el firmware se dividió utilizando la librería `pico/multicore.h`.

* **CORE 1 (Transductor y Matemática):** * Dedicado a la sensórica lenta e ininterrumpida.
  * *Flujo:* Escucha comandos Bluetooth (UART) $\rightarrow$ Parsea tramas NMEA del GPS $\rightarrow$ Lee orientación I2C $\rightarrow$ Ejecuta barridos de barrera ultrasónica $\rightarrow$ Calcula vectores de error matemáticos.
* **CORE 0 (Lazo de Control y Motores):** * Dedicado a la respuesta física en tiempo real.
  * *Flujo:* Ejecuta una máquina de estados reactiva $\rightarrow$ Toma decisiones de evasión o corrección de rumbo $\rightarrow$ Genera señales PWM hacia el driver TB6612FNG.
* **Sincronización de Datos (Spinlocks):** Para evitar condiciones de carrera (*Race Conditions*) al compartir la memoria SRAM, se descartó el uso de memorias FIFO. En su lugar, el sistema utiliza **Spinlocks de Hardware** del bloque SIO de la RP2040, garantizando operaciones atómicas de bloqueo al escribir/leer variables globales compartidas.

---

## 🎯 d. Análisis de Requisitos Funcionales

1. **Requisito: Navegación evitando obstáculos.**
   * *Estado:* **CUMPLIDO.** La máquina de estados detecta objetos a menos de la distancia umbral, detiene el chasis, escanea el sector izquierdo y derecho, y decide la ruta despejada de forma autónoma.
2. **Requisito: Enrutamiento mediante coordenadas GPS.**
   * *Estado:* **CUMPLIDO (Validación HIL).** Se detectó que el módulo GPS comercial presenta un error circular (CEP) superior a 25 metros en interiores por efecto *multipath*. Para aislar esta variable, se implementó una simulación *Hardware-in-the-Loop (HIL)* congelando el origen satelital por software. Esto permitió demostrar físicamente el éxito de la algoritmia de la brújula, la telemetría Bluetooth y los cálculos esféricos en tiempo real al rotar el chasis.

---

## ⚙️ e. Análisis de Requisitos No Funcionales

* **Operación en Tiempo Real:** Logrado mediante la división de carga en dos núcleos. El control de motores nunca experimenta latencia por culpa de las esperas de comunicación UART/I2C.
* **Modularidad y Mantenibilidad:** El código C está estrictamente modularizado (`radar.c`, `gps.c`, `motores.c`, etc.) y documentado.
* **Tolerancia al Ruido (Área de Mejora):** Se identificó que el ruido electromagnético (EMI) inducido por los motores interfiere ocasionalmente con las tramas del bus I2C (GY-511). Esto justifica la necesidad futura de aislar las pistas en un diseño de placa de circuito impreso (PCB) dedicada.

---

## 💰 f. Análisis de Costo (Prototipo vs. Producción)

| Etapa | Descripción | Costo Estimado (COP) |
| :--- | :--- | :--- |
| **Prototipo (Actual)** | Uso de módulos de desarrollo individuales (RPi Pico, placas breakout para sensores y drivers), chasis acrílico, baterías sueltas y protoboards/jumpers. | ~$180,000 |
| **Producción en Masa** | Fabricación de una PCB multicapa FR-4 integrando el microcontrolador RP2040 directamente (IC), puente H SMD, y chips sensores a bordo. Moldes de inyección plástica para el chasis. Compras de volumen (>1000 unidades). | ~$60,000 / unidad |

*Conclusión Económica:* Pasar del prototipo a la producción masiva reduce los costos en un ~66% por unidad, además de resolver los problemas de inductancia parásita y ruido EMI causados por los cables DuPont.

---

## 👥 g. Organización del Equipo y Flujo de Trabajo

El desarrollo se gestionó aplicando metodologías ágiles y control de versiones distribuido.

* **Estrategia de Código:** Se utilizó un repositorio en GitHub con protección de la rama `main`.
* **Distribución de Roles:**
  * *Hardware y Potencia:* Ensamblaje del chasis, cálculos de divisores de tensión y pruebas de torque/corriente.
  * *Firmware Core 0:* Programación de la máquina de estados, control del driver PWM e interrupciones de tiempo.
  * *Firmware Core 1 y Sincronización:* Desarrollo de la matemática de navegación, parseo de protocolos y configuración de *Spinlocks* de hardware.

---
*Este reporte representa el estado final del proyecto entregado para la asignatura de Electrónica Digital III.*
