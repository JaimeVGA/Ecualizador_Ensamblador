# Ecualizador de Audio en Ensamblador x86_64

Proyecto de procesamiento de audio hecho con:
- Ensamblador x86_64 con NASM
- C como capa de control y procesamiento de archivo WAV
- Python con CustomTkinter para la interfaz gráfica

La aplicación carga un archivo WAV, aplica el ecualizador de 3 bandas implementado en ensamblador y guarda el resultado como un nuevo archivo de salida.

## Archivos principales en la raíz

- `app.py`: interfaz gráfica en Python
- `dr_wav.h`: biblioteca de lectura/escritura WAV
- `Ecualizador.asm`: rutina principal en ensamblador
- `Interfaz.c`: puente en C entre Python y ensamblador
- `libecualizador.so`: biblioteca compartida que usa la interfaz Python

## Requisitos

- Linux x86_64
- `nasm`
- `gcc`
- `python3`
- `customtkinter`

Instalación de dependencia Python:

```bash
pip install customtkinter
```

## Compilación

Desde la raíz del proyecto:

```bash
nasm -f elf64 -g -F dwarf Ecualizador.asm -o Ecualizador.o
gcc -fPIC -c Interfaz.c -o Interfaz.o
gcc -shared -o libecualizador.so Ecualizador.o Interfaz.o -lm
```

Si quieres probar la ejecución de la interfaz gráfica directamente:

```bash
python3 app.py
```

## Cómo funciona

1. `app.py` abre una ventana con controles para cargar un archivo WAV.
2. La interfaz llama a `libecualizador.so` con `ctypes`.
3. La librería en C carga el audio con `dr_wav.h`.
4. El archivo se procesa en ensamblador mediante `Ecualizador.asm`.
5. El resultado se guarda como `salida_eq.wav` en la carpeta del ejecutable o del script.

## Estructura del proyecto

```text
.
├── app.py
├── dr_wav.h
├── Ecualizador.asm
├── Interfaz.c
├── libecualizador.so
└── Pruebas/
    └── ... archivos originales de trabajo
```

## Notas

- La carpeta `Pruebas/` se mantiene intacta como área de desarrollo.
- Los archivos de la raíz son copias listas para ejecución y empaquetado.
- Si cambias el ensamblador o el archivo C, recompila `libecualizador.so`.
