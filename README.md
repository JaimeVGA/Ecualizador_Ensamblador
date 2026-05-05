# Ecualizador De Audio ASM x86_64

Proyecto de ecualización de audio WAV desarrollado con tres capas:

1. Ensamblador x86_64 (NASM) para el procesamiento de muestras.
2. C como puente para cargar y guardar audio usando dr_wav.
3. Python (CustomTkinter) para la interfaz gráfica.

La aplicación permite seleccionar un archivo WAV, ajustar ganancias de bajos, medios y agudos, y generar un archivo de salida procesado.

## Objetivo Del Proyecto

- Implementar un ecualizador de 3 bandas en bajo nivel.
- Integrar ASM + C + Python en una sola aplicación usable.
- Mantener una carpeta de trabajo independiente llamada Pruebas.

## Estructura Actual

```text
.
├── app.py
├── Ecualizador.asm
├── Interfaz.c
├── dr_wav.h
├── libecualizador.so
├── README.md
└── Pruebas/
    └── archivos de desarrollo y pruebas locales
```

## Requisitos

- Linux x86_64
- Python 3
- NASM
- GCC

Dependencia Python de interfaz:

```bash
pip install customtkinter
```

## Compilación De La Librería

Ejecutar desde la raíz del proyecto:

```bash
nasm -f elf64 -g -F dwarf Ecualizador.asm -o Ecualizador.o
gcc -fPIC -c Interfaz.c -o Interfaz.o
gcc -shared -o libecualizador.so Ecualizador.o Interfaz.o -lm
```

Resultado esperado:

- Se genera o actualiza libecualizador.so en la raíz.

## Ejecución De La Interfaz

```bash
python3 app.py
```

Flujo de uso:

1. Cargar archivo WAV de entrada.
2. Ajustar sliders de LOW, MID y HIGH.
3. Procesar audio.
4. Revisar salida_eq.wav generado por la aplicación.

## Cómo Funciona Internamente

1. app.py carga la biblioteca compartida con ctypes.
2. Interfaz.c abre el WAV, prepara parámetros y llama a la rutina ASM.
3. Ecualizador.asm aplica el procesamiento por bandas.
4. Interfaz.c escribe el audio procesado en un nuevo archivo WAV.

## Desarrollo En Carpeta Pruebas

La carpeta Pruebas se utiliza como entorno de trabajo y experimentación.

- No es necesario modificarla para ejecutar desde la raíz.
- Puedes sincronizar manualmente archivos entre raíz y Pruebas cuando lo necesites.

## Solución De Problemas Rápida

Si app.py no encuentra la librería:

1. Verifica que exista libecualizador.so en la raíz.
2. Recompila con los comandos de la sección Compilación.
3. Ejecuta nuevamente python3 app.py.
