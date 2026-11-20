'use strict';
// Suscriptor: lee los mensajes que llegan al IoT Hub (endpoint compatible con Event Hubs).
// El paquete antiguo "azure-event-hubs" esta obsoleto; se usa @azure/event-hubs v5.

const { EventHubConsumerClient, latestEventPosition } = require('@azure/event-hubs');

// OJO: NO es la connection string del dispositivo.
// IoT Hub > Hub settings > Built-in endpoints > "Event Hub-compatible endpoint"
const connectionString = process.env.IOTHUB_EVENTHUB_CONNECTION_STRING;

if (!connectionString) {
  console.error('Falta IOTHUB_EVENTHUB_CONNECTION_STRING');
  process.exit(1);
}

const consumer = new EventHubConsumerClient('$Default', connectionString);

consumer.subscribe(
  {
    processEvents: async (events, context) => {
      for (const event of events) {
        console.log('Message received (particion ' + context.partitionId + '):');
        console.log(JSON.stringify(event.body));
        console.log('  dispositivo:', event.systemProperties['iothub-connection-device-id']);
        console.log('  propiedades:', JSON.stringify(event.properties));
        console.log('');
      }
    },
    processError: async (err) => console.error(err.message)
  },
  { startPosition: latestEventPosition }
);

console.log('Suscriptor escuchando el IoT Hub... (Ctrl+C para salir)');