# Validación funcional del prototipo

## Objetivo

Validar que el sistema de aprovisionamiento WiFi cumple con los requisitos mínimos del reto: inicio en modo AP, configuración mediante portal web, almacenamiento persistente, reconexión automática y mecanismo de reset.

## Plan de pruebas

| ID | Prueba | Procedimiento | Resultado esperado | Estado |
|---|---|---|---|---|
| P01 | Inicio sin credenciales | Borrar memoria y reiniciar ESP32 | El ESP32 crea la red `ESP32-SETUP-XXXX` | Pendiente |
| P02 | Acceso al portal | Conectarse al AP y abrir `192.168.4.1` | Se muestra el formulario web | Pendiente |
| P03 | Guardar credenciales | Enviar SSID y contraseña válidos | El ESP32 guarda datos y reinicia | Pendiente |
| P04 | Conexión automática | Reiniciar después de guardar credenciales | El ESP32 se conecta a la red configurada | Pendiente |
| P05 | Consulta de estado | Acceder a `/api/status` | Se recibe JSON con modo, SSID, IP y memoria libre | Pendiente |
| P06 | Reset por botón | Mantener presionado GPIO27 durante 4 segundos | Se borran credenciales y vuelve al modo AP | Pendiente |
| P07 | Reset por endpoint | Enviar `POST /api/reset` | Se borran credenciales y reinicia en modo AP | Pendiente |
| P08 | Contraseña incorrecta | Guardar clave WiFi inválida | El ESP32 no conecta y vuelve a modo configuración | Pendiente |
| P09 | Pérdida de red | Apagar temporalmente el router | El ESP32 intenta reconectarse automáticamente | Pendiente |
