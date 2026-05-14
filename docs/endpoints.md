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

### Ejemplo de solicitud

```http
GET / HTTP/1.1
Host: 192.168.4.1
```

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

### Ejemplo de payload

```http
ssid=MiRedWiFi&password=MiPassword123
```

### Respuesta exitosa

```html
<h2>Credenciales guardadas</h2>
<p>El ESP32 se reiniciará e intentará conectarse a la red configurada.</p>
```

### Posibles errores

| Código | Causa |
|---:|---|
| 400 | No se envió SSID |
| 400 | El SSID está vacío |

---

## `GET /api/status`

Consulta el estado actual del dispositivo.

| Campo | Valor |
|---|---|
| Método | GET |
| URL | `/api/status` |
| Headers | Ninguno |
| Query params | Ninguno |
| Payload | Ninguno |
| Respuesta exitosa | `200 application/json` |

### Ejemplo de respuesta en modo Station

```json
{
  "mode": "STA",
  "connected": true,
  "ssid": "MiRedWiFi",
  "ip": "192.168.1.34",
  "freeHeap": 213456
}
```

### Ejemplo de respuesta en modo configuración

```json
{
  "mode": "AP_CONFIG",
  "connected": false,
  "ssid": "ESP32-SETUP-ABCD",
  "ip": "192.168.4.1",
  "freeHeap": 211320
}
```

---

## `POST /api/reset`

Borra las credenciales guardadas y reinicia el ESP32 en modo configuración.

| Campo | Valor |
|---|---|
| Método | POST |
| URL | `/api/reset` |
| Headers | Ninguno |
| Query params | Ninguno |
| Payload | Ninguno |
| Respuesta exitosa | `200 text/html` |

### Ejemplo de respuesta

```html
<h2>Configuración borrada</h2>
<p>El ESP32 se reiniciará en modo configuración.</p>
```

---

## Endpoints auxiliares de portal cautivo

| Endpoint | Método | Función |
|---|---|---|
| `/generate_204` | GET | Redirección para Android |
| `/hotspot-detect.html` | GET | Redirección para iOS/macOS |
| `/fwlink` | GET | Redirección para Windows |
