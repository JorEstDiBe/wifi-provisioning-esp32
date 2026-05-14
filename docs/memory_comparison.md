# Comparación de memoria Flash

## Objetivo

Comparar la memoria Flash usada por la implementación propia frente al ejemplo `Basic` de la librería WiFiManager.

## Condiciones de compilación

| Parámetro | Valor |
|---|---|
| IDE | Arduino IDE ____ |
| Core Arduino-ESP32 | ____ |
| Placa | ____ |
| Partition Scheme | ____ |
| CPU Frequency | ____ |
| Upload Speed | ____ |

## Procedimiento

1. Compilar la implementación propia.
2. Registrar la línea de uso de memoria mostrada por Arduino IDE.
3. Abrir el ejemplo `Basic` de WiFiManager.
4. Compilarlo con la misma placa y configuración.
5. Registrar los valores de Flash y RAM.
6. Comparar los resultados.

## Resultados

| Implementación | Flash usada | Porcentaje de Flash | RAM dinámica usada | Diferencia |
|---|---:|---:|---:|---:|
| Implementación propia | ____ bytes | ____ % | ____ bytes | Referencia |
| WiFiManager Basic | ____ bytes | ____ % | ____ bytes | ____ bytes |

## Análisis

La implementación propia utiliza directamente las librerías `WiFi.h`, `WebServer.h`, `DNSServer.h` y `Preferences.h`. Esto permite mayor control sobre el flujo de aprovisionamiento, los endpoints y la lógica de reset.

WiFiManager ofrece una solución más completa y general, incluyendo portal cautivo y funcionalidades adicionales. Sin embargo, puede ocupar una cantidad diferente de memoria Flash debido a que integra más características.
