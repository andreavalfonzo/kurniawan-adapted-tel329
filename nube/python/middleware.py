import os
import json
import time
import urllib.request
from datetime import datetime
from dotenv import load_dotenv
from azure.iot.device import IoTHubDeviceClient, Message

load_dotenv()
cadena_dispositivo = os.getenv("CADENA_DISPOSITIVO")
url_sensor = os.getenv("URL_SENSOR", "http://[fd00::202:2:2:2]/")
intervalo = 5            # segundos entre lecturas
max_mensajes = 60        # para no gastar el cupo diario del F1
umbral_co2 = 1000        # ppm


def leer_sensor():
    with urllib.request.urlopen(url_sensor, timeout=5) as respuesta:
        return json.loads(respuesta.read().decode())


cliente = IoTHubDeviceClient.create_from_connection_string(cadena_dispositivo)
cliente.connect()
print("Conectado a Azure IoT Hub como sensor-g3")
print("Leyendo el sensor en", url_sensor, "cada", intervalo, "segundos")

enviados = 0
try:
    while enviados < max_mensajes:
        try:
            datos = leer_sensor()
        except Exception as error:
            print("No pude leer el sensor:", error)
            time.sleep(intervalo)
            continue

        datos["hora"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        print("Lectura del sensor:", datos)

        mensaje = Message(json.dumps(datos))
        mensaje.content_type = "application/json"
        mensaje.content_encoding = "utf-8"
        if datos["co2"] > umbral_co2:
            mensaje.custom_properties["alerta_co2"] = "si"
        else:
            mensaje.custom_properties["alerta_co2"] = "no"

        cliente.send_message(mensaje)
        enviados += 1
        print("Mensaje", enviados, "enviado y confirmado por Azure (alerta_co2 =",
              mensaje.custom_properties["alerta_co2"] + ")")
        time.sleep(intervalo)
except KeyboardInterrupt:
    print("Middleware detenido")
finally:
    cliente.shutdown()
    print("Total de mensajes enviados:", enviados)
