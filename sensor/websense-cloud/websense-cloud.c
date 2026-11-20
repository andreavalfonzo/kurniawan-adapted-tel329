/*
 * websense-cloud.c - Sensor Contiki-NG (Actividad 4, TEL329)
 * Servidor HTTP minimo que responde {"temp":X,"hum":Y} a cada GET.
 * Basado en Kurniawan (2018) y en examples/rpl-border-router/webserver.
 */
#include "contiki.h"
#include "net/ipv6/uip.h"
#include "httpd-simple.h"
#include "lib/random.h"
#include <stdio.h>
#include <string.h>

#define SEND_STRING(s, str) PSOCK_SEND(s, (uint8_t *)(str), strlen(str))

/* static: las variables locales no sobreviven dentro de un protothread */
static char buff[48];
static int temperature;
static int humidity;

PROCESS(websense_process, "Websense Cloud");
AUTOSTART_PROCESSES(&websense_process);

/*---------------------------------------------------------------------------*/
static
PT_THREAD(generate_routes(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  temperature = 15 + random_rand() % 25;   /* 15..39 C */
  humidity    = 80 + random_rand() % 10;   /* 80..89 % */

  snprintf(buff, sizeof(buff), "{\"temp\":%d,\"hum\":%d}", temperature, humidity);
  printf("send json to requester: %s\n", buff);
  SEND_STRING(&s->sout, buff);

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
/* httpd-simple.c llama a esta funcion para saber que responder */
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{
  return generate_routes;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(websense_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Websense Cloud: servidor HTTP en puerto 80\n");
  httpd_init();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
  }

  PROCESS_END();
}