# Ecualizador Ensamblador

Proyecto de prueba para procesar audio en ensamblador x86-64 y validarlo desde C.

## Estructura

- `Modulos Ensamblador/`: código NASM del procesamiento de audio.
- `Librerias/Interfaces/`: interfaz C de referencia con lectura y escritura de WAV.
- `Pruebas/`: archivos de prueba, binarios generados y la interfaz que se está usando para validar el módulo.
- `Audios Prueba/`: audios de entrada y salida para las pruebas.

## Módulos disponibles

- `procesar_volumen`: aplica una ganancia a una secuencia de muestras `float`.
- `procesar_ecualizador`: procesa audio estéreo con una configuración de bandas tipo biquad.

## Compilación

Para ensamblar y enlazar el módulo de prueba:

```bash
nasm -f elf64 -g -F dwarf Pruebas/CambioVolumen.asm -o Pruebas/CambioVolumen.o
gcc -Wall -Wextra -std=c11 Pruebas/Interfaz.c Pruebas/CambioVolumen.o -o Pruebas/a.out
```

Si quieres ejecutar la interfaz de prueba por teclado, compila el archivo correspondiente en `Pruebas/` y enlázalo con el objeto ASM.

## Ejecución

La interfaz de referencia carga un WAV de entrada, llama al módulo en ensamblador y guarda un WAV de salida.

Ejemplo de uso:

```bash
cd Pruebas
./a.out
```

## Notas

- Los archivos generados, como `.o` y binarios, no deberían versionarse.
- Si cambias la firma de una rutina ASM, actualiza también la declaración `extern` en la interfaz C.