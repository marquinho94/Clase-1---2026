#include <Arduino.h>
#define pulsador 34
#define led 18 // enciende con LOW
#define t_anti_rebote 250
#define t_parpadeo_ms 500

// estados globales

typedef enum
{
  lectura,
  espera,
  validacion
} antirebote;
antirebote antiRebotePulsador1 = lectura;

typedef enum
{
  apagado,
  encendido,
  parpadeo
} MAIN;
MAIN estadosLed1 = apagado;

void setup()
{
  // put your setup code here, to run once:
  // Configuración de puertos
  pinMode(pulsador, INPUT);
  pinMode(led, OUTPUT);

  Serial.begin(115200);
  Serial.println("¡Hola!");
}

void loop()
{
  delay(10); // por recomendación del simulador
  // put your main code here, to run repeatedly:

  // Serial.println("Introduzca la velocidad de conmutación");

  // Lectura de pulsador
  static u_long tiempoInicial = 0;
  static u_int32_t pulsaciones = 0;
  switch (antiRebotePulsador1)
  {
  case lectura:
    if (!digitalRead(pulsador))
    {
      antiRebotePulsador1 = espera;
      tiempoInicial = millis();
    }
    else
    {
      antiRebotePulsador1 = lectura; // solo a fines didácticos
    }
    break;
  case espera:
    if (millis() >= (tiempoInicial + t_anti_rebote))
    {
      antiRebotePulsador1 = validacion;
    }
    break;
  case validacion:
    if (!digitalRead(pulsador))
    {
      antiRebotePulsador1 = lectura;
      pulsaciones++;
      // debugging line
      Serial.print("se registran ");
      Serial.print(pulsaciones);
      Serial.println(" pulsaciones");
    }
    else
    { // cambio de estado sin sumar al contador-> considera que fue un ruido o rebote
      antiRebotePulsador1 = lectura;
    }
    break;

  default:
    /* digitalWrite(led,HIGH); */
    antiRebotePulsador1 = lectura;
    pulsaciones = 0;

    break;
  }
  // manejo de LED 1
  static u_int32_t *p_pulsaciones = &pulsaciones;
  switch (estadosLed1)
  {
  case apagado:
    digitalWrite(led, HIGH);
    if (*p_pulsaciones != 0)
    {
      *p_pulsaciones = 0;
      estadosLed1 = encendido;
    }

    break;
  case encendido:
    digitalWrite(led, LOW);
    if (*p_pulsaciones != 0)
    {
      *p_pulsaciones = 0;
      estadosLed1 = parpadeo;
    }

    break;

  case parpadeo:
    static u_int32_t tiempo = millis();

    if (millis() >= tiempo + t_parpadeo_ms)
    {
      digitalWrite(led, !digitalRead(led));
      tiempo = millis();
    }

    if (*p_pulsaciones != 0)
    {
      *p_pulsaciones = 0;
      estadosLed1 = apagado;
    }

    break;

  default:
    digitalWrite(led, HIGH);
    estadosLed1 = apagado;

    break;
  }
}
