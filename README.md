# Ecualizador En Ensamblador (x86-64)

Proyecto de pruebas para procesamiento de audio con NASM y validacion desde C usando `dr_wav`.

## Estructura del proyecto

```text
.
├── assets/
│   └── audio/
│       ├── entrada.wav
│       └── entrada_prueba.wav
├── build/
├── include/
│   ├── dr_mp3.h
│   └── dr_wav.h
├── src/
│   ├── asm/
│   │   ├── cambio_volumen.asm
│   │   └── procesar_ecualizador.asm
│   └── c/
│       ├── interfaz_ecualizador.c
│       └── interfaz_referencia.c
└── README.md
```

## Que hace cada modulo

- `procesar_volumen`: aplica una ganancia a muestras `float`.
- `procesar_ecualizador`: aplica ecualizacion de 3 bandas sobre audio estereo.

## Requisitos

- Linux x86-64
- `nasm`
- `gcc`

## Compilacion (tu flujo NASM + GCC)

Desde la raiz del proyecto, este es el flujo recomendado y compatible con tu forma de trabajo:

```bash
mkdir -p build
nasm -f elf64 src/asm/procesar_ecualizador.asm -o build/objeto.o
gcc src/c/interfaz_ecualizador.c build/objeto.o -Iinclude -lm -o build/ecualizador
```

Si antes usabas nombres antiguos, esta es la equivalencia:

- `procesarEcualizador.asm` -> `src/asm/procesar_ecualizador.asm`
- `Interfaz.c` -> `src/c/interfaz_ecualizador.c`

Compilacion con flags extra (opcional):

```bash
nasm -f elf64 -g -F dwarf src/asm/procesar_ecualizador.asm -o build/objeto.o
gcc -Wall -Wextra -std=c11 -Iinclude src/c/interfaz_ecualizador.c build/objeto.o -lm -o build/ecualizador
```

## Ejecucion

```bash
./build/ecualizador
```

Entrada esperada:

- `assets/audio/entrada_prueba.wav`

Salida generada:

- `assets/audio/salida_ecualizador.wav`

## Variante de referencia

Si deseas compilar la interfaz de referencia:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/c/interfaz_referencia.c -o build/interfaz_referencia
```

## Notas

- Los binarios y objetos deben quedar en `build/`.
- Si cambias la firma de una rutina ASM, actualiza el `extern` correspondiente en C.