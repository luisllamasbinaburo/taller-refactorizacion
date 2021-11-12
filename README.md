# Ficheros de taller refactorización

Ficheros para el taller de limpieza de código de Arduino de la comunidad Maker en Discord https://www.luisllamas.es/Discord


## ¿Cómo participar?
En el fichero "original.ino" tenéis el código que debéis limpiar y reordenar , manteniendo la funcionalidad.
No hace falta tener el hardware para probarlo, es un ejercicio de limpieza de código. Con que os compile, asumimos que estará bien.

Para añadir vuestra solución:
1. Hacer un Fork a este repositorio (icono arriba a la derecha)
2. Añadir los ficheros en vuestro fork
3. Poner un comentario o un enlace en el canal "talleres" de la comunidad Discord para que todos sepamos que está aquí

## Cómo compilar el programa
El proyecto original es para el ESP8266 pero, debido a cambios en las librerías e incompatibilidades **nosotros vamos a compilar para el ESP32**
He adaptado el código para que compile en esta plataforma

Además, necesitaréis estas librerías:
- LiquidCrystal_I2C https://github.com/johnrickman/LiquidCrystal_I2C
- DHTEsp https://github.com/beegee-tokyo/DHTesp

## Crédito al autor
El código del que partimos en el taller está sacado de este proyecto Open Source
https://www.hackster.io/cesar-j-mcf/alexa-controlled-thermostat-with-nodemcu-v3-and-raspberry-pi-501d56

¡Aprovechad para darle las gracias su trabajo y compartirlo con la comunidad!
