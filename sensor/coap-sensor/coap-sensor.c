/*
 * coap-sensor.c - Sensor Contiki-NG (Actividad 4, TEL329)
 * Expone el recurso CoAP "sensors/temp" que responde {"temp":X,"hum":Y}.
 * Reemplaza al servidor HTTP websense (mismo JSON, pero sobre CoAP/UDP).
 */
#include "contiki.h"
#include "coap-engine.h"
#include "lib/random.h"
#include <stdio.h>
#include <string.h>

static void res_get_handler(coap_message_t *request, coap_message_t *response,
                            uint8_t *buffer, uint16_t preferred_size,
                            int32_t *offset);

RESOURCE(res_temp,
         "title=\"Temperatura y humedad\";rt=\"application/json\"",
         res_get_handler, NULL, NULL, NULL);

static void
res_get_handler(coap_message_t *request, coap_message_t *response,
                uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int temperature = 15 + random_rand() % 25;  /* 15..39 C */
  int humidity    = 80 + random_rand() % 10;  /* 80..89 % */
  int len = snprintf((char *)buffer, preferred_size,
                     "{\"temp\":%d,\"hum\":%d}", temperature, humidity);

  printf("send json to requester: %s\n", (char *)buffer);
  coap_set_header_content_format(response, APPLICATION_JSON);
  coap_set_payload(response, buffer, len);
}
/*---------------------------------------------------------------------------*/
PROCESS(coap_sensor_process, "CoAP sensor");
AUTOSTART_PROCESSES(&coap_sensor_process);

PROCESS_THREAD(coap_sensor_process, ev, data)
{
  PROCESS_BEGIN();

  coap_activate_resource(&res_temp, "sensors/temp");
  printf("Sensor CoAP listo: recurso /sensors/temp en puerto 5683\n");

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}