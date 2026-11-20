#include "contiki.h"
#include "contiki-net.h"
#include "httpd-simple.h"
#include "lib/random.h"
#include "sys/node-id.h"
#include <stdio.h>
#include <string.h>

PROCESS(proceso_sensor, "Sensor nube grupo 3");
AUTOSTART_PROCESSES(&proceso_sensor);

/* static porque PSOCK_SEND cede el control y las locales se pierden */
static char respuesta[100];
static int temperatura;
static int humedad;
static int co2;
static unsigned int num_lectura = 0;

static PT_THREAD(enviar_lectura(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  temperatura = 15 + random_rand() % 21;   /* 15 a 35 C */
  humedad = 30 + random_rand() % 61;       /* 30 a 90 % */
  co2 = 400 + random_rand() % 1101;        /* 400 a 1500 ppm */
  num_lectura++;

  snprintf(respuesta, sizeof(respuesta),
           "{\"nodo\":%u,\"lectura\":%u,\"temperatura\":%d,\"humedad\":%d,\"co2\":%d}",
           node_id, num_lectura, temperatura, humedad, co2);
  printf("Consulta recibida, respondo: %s\n", respuesta);

  PSOCK_SEND_STR(&s->sout, respuesta);

  PSOCK_END(&s->sout);
}

httpd_simple_script_t httpd_simple_get_script(const char *nombre)
{
  return enviar_lectura;
}

PROCESS_THREAD(proceso_sensor, ev, data)
{
  PROCESS_BEGIN();

  random_init(node_id);
  httpd_init();
  printf("Sensor %u listo, servidor HTTP en el puerto 80\n", node_id);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
  }

  PROCESS_END();
}
