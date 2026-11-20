import os
from dotenv import load_dotenv
from azure.eventhub import EventHubConsumerClient

load_dotenv()
cadena_eventhub = os.getenv("CADENA_EVENTHUB")


def al_recibir(contexto, evento):
    datos = evento.body_as_json()
    dispositivo = evento.system_properties.get(b"iothub-connection-device-id", b"?").decode()
    alerta = evento.properties.get(b"alerta_co2", b"?").decode()
    print("Mensaje recibido desde", dispositivo, "(particion", contexto.partition_id + ")")
    print("  datos:", datos)
    print("  alerta_co2:", alerta, "| llego a Azure:", evento.enqueued_time.strftime("%H:%M:%S"))


cliente = EventHubConsumerClient.from_connection_string(cadena_eventhub, consumer_group="$Default")
print("Escuchando mensajes nuevos del IoT Hub (Ctrl+C para salir)")
try:
    with cliente:
        cliente.receive(on_event=al_recibir, starting_position="@latest")
except KeyboardInterrupt:
    print("Suscriptor detenido")
