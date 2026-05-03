# Ecualizador De Audio En Ensamblador (x86-64)

Proyecto profesional de procesamiento de audio en tiempo real usando **Ensamblador x86-64 (NASM) + C + Python (CustomTkinter)** con validación mediante `dr_wav`.

## Estructura del proyecto

```
.
├── app.py                       # Interfaz gráfica (Python + CustomTkinter)
├── README.md
├── assets/
│   └── audio/                   # Audios de entrada para pruebas
│       ├── entrada.wav
│       └── entrada_prueba.wav
├── build/                       # Artefactos compilados (generado)
├── include/                     # Cabeceras comunes
│   ├── dr_mp3.h
│   └── dr_wav.h
├── src/
│   ├── asm/                     # Código en ensamblador
│   │   ├── cambio_volumen.asm
│   │   ├── Ecualizador.asm      # Ecualizador 3-bandas (copia de trabajo)
│   │   └── procesar_ecualizador.asm
│   └── c/                       # Código en C
│       ├── Interfaz.c           # Interfaz C (copia de trabajo)
│       ├── ecualizador.c        # (copia de trabajo)
│       ├── interfaz_ecualizador.c
│       └── interfaz_referencia.c
├── Pruebas/                     # Área de desarrollo y experimentación
│   ├── app.py
│   ├── Ecualizador.asm
│   ├── Interfaz.c
│   ├── ecualizador.c
│   └── ... (otros artefactos de prueba)
└── .gitignore
```

## Descripción de módulos

| Módulo | Ubicación | Descripción |
|--------|-----------|-------------|
| **Ecualizador 3-bandas** | `src/asm/Ecualizador.asm` | Procesamiento de audio estéreo en ASM x86-64 |
| **Interfaz C** | `src/c/Interfaz.c` | Wrapper C que llama a rutinas ASM y maneja audio |
| **Interfaz GUI** | `app.py` | Aplicación CustomTkinter con preview en tiempo real |
| **Pruebas** | `Pruebas/` | Área de desarrollo con versiones de trabajo y artefactos compilados |

## Requisitos

- Linux x86-64
- `nasm`
- `gcc`

## Compilación

### Compilación desde raíz (estructura profesional)

```bash
mkdir -p build
nasm -f elf64 src/asm/Ecualizador.asm -o build/ecualizador_asm.o
gcc -Wall -Wextra -std=c11 -Iinclude src/c/Interfaz.c build/ecualizador_asm.o -lm -o build/ecualizador_c
```

### Compilación desde Pruebas/ (área de desarrollo)

Si trabajas en `Pruebas/` y compilas ahí:

```bash
cd Pruebas
nasm -f elf64 Ecualizador.asm -o objeto.o
gcc Interfaz.c objeto.o ../include/dr_wav.h -lm -o ecualizador
```

### Compilación con debugging (flags extra)

```bash
mkdir -p build
nasm -f elf64 -g -F dwarf src/asm/Ecualizador.asm -o build/ecualizador_asm.o
gcc -Wall -Wextra -std=c11 -g -Iinclude src/c/Interfaz.c build/ecualizador_asm.o -lm -o build/ecualizador_c
```

## Ejecución

### Interfaz gráfica (GUI)

```bash
python3 app.py
```

Requisito: `pip install customtkinter`

### Binario C compilado

```bash
./build/ecualizador_c
```

Entrada: `assets/audio/entrada_prueba.wav`

Salida: `assets/audio/salida_ecualizador.wav`

## Construcción del ejecutable con PyInstaller

Desde raíz:

```bash
# Compilar primero el binario C
mkdir -p build
nasm -f elf64 src/asm/Ecualizador.asm -o build/ecualizador.o
gcc -Wall -Wextra -std=c11 -Iinclude src/c/Interfaz.c build/ecualizador.o -lm -o build/libecualizador.so

# Empaquetar con PyInstaller
python3 -m PyInstaller --onefile --add-binary "build/libecualizador.so:." --collect-all customtkinter app.py
```

## Notas de desarrollo

- **Carpeta `Pruebas/`**: Área de desarrollo personal. No se sincroniza con git (ver `.gitignore`).
- **Carpeta `src/`**: Código fuente profesional con copias de trabajo desde `Pruebas/`.
- **Carpeta `build/`**: Se limpia antes de compilar (`rm -rf build/`).
- Si modificas la firma de una rutina ASM, actualiza el `extern` correspondiente en C.
- Los audios grandes se ignoran en git (ver `.gitignore`).