# Documentación de endpoints

## `GET /`

Muestra el formulario web para ingresar SSID y contraseña.

| Campo | Valor |
|---|---|
| Método | GET |
| URL | `/` |
| Headers | Ninguno |
| Query params | Ninguno |
| Payload | Ninguno |
| Respuesta exitosa | `200 text/html` |

---

## `POST /save`

Guarda las credenciales WiFi en memoria no volátil.

| Campo | Valor |
|---|---|
| Método | POST |
| URL | `/save` |
| Headers | `Content-Type: application/x-www-form-urlencoded` |
| Query params | Ninguno |
| Payload | `ssid`, `password` |
| Respuesta exitosa | `200 text/html` |
| Respuesta de error | `400 text/html` |

Ejemplo de payload:

```http
ssid=MiRedWiFi&password=MiPassword123
