# Actividad 4 en Python (Grupo 3)

Sensor Contiki-NG -> border router (Cooja + tunslip6) -> middleware -> Azure IoT Hub -> suscriptor.
Basado en Kurniawan, A. (2018). Practical Contiki-NG. Apress, adaptado a Contiki-NG actual y a los SDK actuales de Azure.

## Carpetas
- `sensor/sensor-nube/`: mote que responde por HTTP un JSON con temperatura, humedad y CO2.
- `nube/python/middleware.py`: lee el sensor cada 5 s y manda cada lectura a Azure IoT Hub, con la propiedad `alerta_co2` (respuesta 4.a).
- `nube/python/suscriptor.py`: lee los mensajes desde el endpoint compatible con Event Hubs del IoT Hub (respuesta 4.c).

## Como correrlo
1. Copiar `sensor/sensor-nube` a `contiki-ng/examples/` y agregar `httpd-simple.c` y `httpd-simple.h` desde `examples/rpl-border-router/webserver/`.
2. En Cooja: mote 1 con `rpl-border-router/border-router.c` (Serial Socket SERVER en el puerto 60001) y mote 2 con `sensor-nube.c`.
3. `sudo tools/serial-io/tunslip6 -a 127.0.0.1 fd00::1/64` y probar con `curl -g "http://[fd00::202:2:2:2]/"`.
4. En `nube/python`: `pip install -r requirements.txt`, copiar `.env.example` a `.env` con las connection strings reales.
5. `python suscriptor.py` en una terminal y `python middleware.py` en otra.

El archivo `.env` no se sube al repositorio.
