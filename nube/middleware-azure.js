'use strict';
// Middleware: pide el dato al mote por CoAP (UDP, IPv6) y lo envia a
// Azure IoT Hub por MQTT (SDK azure-iot-device).
// Lee el sensor SIEMPRE; si Azure no esta disponible, muestra el mensaje
// que habria enviado y sigue funcionando.

const coap = require('coap');
const { Client, Message } = require('azure-iot-device');
const { Mqtt } = require('azure-iot-device-mqtt');

// IoT Hub > Devices > contiki-ng-01 > Primary connection string (opcional)
const connectionString = process.env.IOTHUB_DEVICE_CONNECTION_STRING;
const MOTE_IP = process.env.MOTE_IP || 'fd00::302:304:506:708';
const MOTE_PORT = parseInt(process.env.MOTE_PORT || '5683', 10);
const RESOURCE = '/sensors/temp';
const DEVICE_ID = 'contiki-ng-01';
const INTERVAL_MS = 3000;

const agent = new coap.Agent({ type: MOTE_IP.includes(':') ? 'udp6' : 'udp4' });
let client = null;
let azureReady = false;

function readAndSend() {
  const req = coap.request({
    hostname: MOTE_IP, port: MOTE_PORT, pathname: RESOURCE, method: 'GET', agent
  });

  req.on('response', async (res) => {
    try {
      const obj = JSON.parse(res.payload.toString());
      console.log('Dato del mote (CoAP ' + res.code + '):', obj);

      const data = JSON.stringify({
        deviceId: DEVICE_ID,
        temperature: obj.temp,
        humidity: obj.hum,
        timestamp: new Date().toISOString()
      });
      const message = new Message(data);
      message.properties.add('temperatureAlert', obj.temp > 30 ? 'true' : 'false');

      if (azureReady) {
        console.log('Enviando a Azure: ' + message.getData());
        await client.sendEvent(message);
        console.log('send status: MessageEnqueued');
      } else {
        console.log('Mensaje preparado (Azure no disponible): ' + message.getData() +
                    ' | temperatureAlert=' + (obj.temp > 30));
      }
    } catch (err) {
      console.error('Error:', err.message);
    }
  });

  req.on('error', (err) => console.error('Error CoAP:', err.message));
  req.end();
}

if (connectionString) {
  client = Client.fromConnectionString(connectionString, Mqtt);
  client.open()
    .then(() => { azureReady = true; console.log('Cliente conectado a Azure IoT Hub'); })
    .catch((err) => console.error('No se pudo conectar a Azure: ' + err.message));
} else {
  console.log('Sin IOTHUB_DEVICE_CONNECTION_STRING: se leera el sensor sin enviar a Azure');
}

console.log('Middleware CoAP->Azure iniciado. Mote: coap://[' + MOTE_IP + ']:' + MOTE_PORT + RESOURCE);
setInterval(readAndSend, INTERVAL_MS);