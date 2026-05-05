# Ecualizador de Audio en Ensamblador x86-64

Ecualizador profesional de 3 bandas de frecuencia (bajos, medios, agudos) implementado con **x86-64 Assembly (NASM)**, **C**, y **Python** con interfaz gráfica moderna.

## 📋 Descripción del Proyecto

Este proyecto implementa un ecualizador de audio de 3 bandas con dos opciones de procesamiento:

- **MODO C**: Implementación en C puro (referencia confiable)
- **MODO ASM**: Implementación optimizada en x86-64 assembly con SIMD

El ecualizador procesa archivos WAV estéreo y permite ajustar la ganancia en dB de cada banda de frecuencia (rango: -20dB a +20dB).

### Tecnología

1. **Ensamblador x86_64 (NASM)** - Procesamiento optimizado de muestras de audio con SIMD
2. **C99** - Interfaz, cálculo de coeficientes, I/O de archivos WAV
3. **Python 3.12** - Interfaz gráfica moderna con customtkinter
4. **dr_wav.h** - Librería de lectura/escritura WAV (header-only)

---

## 🖥️ Requisitos del Sistema

### Software

- **SO**: Linux x86-64 (Ubuntu 20.04+, Debian 11+)
- **Python**: 3.12 o superior
- **Compiladores**: 
  - NASM (assembler x86-64)
  - GCC (compilador C)
- **Procesador**: Intel/AMD con soporte SSE4 (prácticamente todos los modernos)

### Instalación de Dependencias

**Ubuntu/Debian:**

```bash
sudo apt-get update
sudo apt-get install python3 python3-pip python3-tk nasm gcc
```

**Opcional (para desarrollo):**

```bash
sudo apt-get install build-essential python3-dev
```

---

## 📁 Estructura del Proyecto

```
Ecualizador_Ensamblador/
├── README.md                        # Este archivo
├── INSTRUCCIONES_EJECUCION.txt      # Guía completa de ejecución
├── Pruebas/
│   ├── app.py                       # Interfaz gráfica Python
│   ├── Ecualizador.asm             # Código ASM optimizado (SIMD)
│   ├── InterfazDual.c              # Interfaz C dual-mode
│   ├── dr_wav.h                    # Librería WAV I/O
│   ├── libecualizador.so           # Librería compartida compilada
│   ├── ecualizador/                # Entorno virtual Python
│   ├── dist/
│   │   └── EcualizadorASM          # Ejecutable empaquetado (PyInstaller)
│   └── [archivos WAV para procesar]
```

---

## 🚀 Guía Rápida de Inicio

### 1️⃣ Preparación del Entorno

```bash
cd ~/Documentos/Ecualizador_Ensamblador/Pruebas
source ecualizador/bin/activate
```

### 2️⃣ Compilación (si editas código)

```bash
nasm -f elf64 Ecualizador.asm -o Ecualizador.o && \
gcc -fPIC -c InterfazDual.c -o InterfazDual.o && \
gcc -shared -o libecualizador.so Ecualizador.o InterfazDual.o -lm
```

### 3️⃣ Ejecutar Interfaz Gráfica

```bash
python3 app.py
```

**Uso:**
- Clic en área gris superior para seleccionar archivo WAV
- Ajusta sliders: Bajos (100Hz), Medios (1kHz), Agudos (5kHz)
- Elige modo: "Usar C" o "Usar ASM"
- Clic "GUARDAR" para procesar
- Archivo de salida: `{NombreCanción}_{Modo}.wav`

### 4️⃣ Ejecutar Binario Empaquetado (sin Python)

```bash
cd dist
./EcualizadorASM
```

---

## 🔧 Compilación Detallada

### Paso 1: Compilar ASM

```bash
nasm -f elf64 Ecualizador.asm -o Ecualizador.o
```

Genera: `Ecualizador.o` (object file)

### Paso 2: Compilar C

```bash
gcc -fPIC -c InterfazDual.c -o InterfazDual.o
```

Flags:
- `-fPIC`: Position Independent Code (necesario para librería compartida)
- `-c`: Solo compilar, no linkar

### Paso 3: Crear Librería Compartida

```bash
gcc -shared -o libecualizador.so Ecualizador.o InterfazDual.o -lm
```

Flags:
- `-shared`: Generar librería compartida
- `-lm`: Linkar librería matemática

**Verificar compilación:**

```bash
ls -lh libecualizador.so
nm libecualizador.so | grep procesar
```

---

## 🎵 Cómo Funciona Internamente

### Flujo de Procesamiento

```
Archivo WAV (entrada)
        ↓
    [app.py]
        ↓
[libecualizador.so]
        ↓
    ┌─────────────────────┐
    │  InterfazDual.c     │
    │ - Carga WAV         │
    │ - Calcula coefs     │
    │ - Llama ASM o C     │
    │ - Guarda WAV        │
    └─────────────────────┘
        ↓
    ┌─────────────────────┐
    │ procesar_ecualizador_ASM (SIMD)  o  procesar_ecualizador_C
    │ - Cascada 3 bandas  │
    │ - Filtro Biquad     │
    │ - Procesa por muestra│
    └─────────────────────┘
        ↓
Archivo WAV (salida)
```

### Algoritmo Biquad (Cascada de Filtros)

Cada muestra pasa por 3 filtros en serie:

```
Entrada (L, R)
    ↓
[Filtro BAJOS @ 100Hz]
    ↓
[Filtro MEDIOS @ 1kHz]
    ↓
[Filtro AGUDOS @ 5kHz]
    ↓
Salida (L, R)
```

**Fórmula (tiempo discreto):**

```
y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
```

Donde:
- `x[n]`, `x[n-1]`, `x[n-2]`: Entradas (actual y 2 previas)
- `y[n]`, `y[n-1]`, `y[n-2]`: Salidas (actual y 2 previas)
- `b0, b1, b2, a1, a2`: Coeficientes (calculados por `calcular_biquad()`)

### Optimización en ASM

- Usa registros SSE4 (xmm0-xmm15)
- Procesa ambos canales (L, R) en paralelo como vectores
- Mantiene estado por canal, por banda
- Evita conversión de punto flotante innecesaria

---

## 🎛️ Interfaz Gráfica

### Controles

| Control | Rango | Función |
|---------|-------|---------|
| Slider Bajos | -20 a +20 dB | Atenúa/amplifica @ 100Hz |
| Slider Medios | -20 a +20 dB | Atenúa/amplifica @ 1kHz |
| Slider Agudos | -20 a +20 dB | Atenúa/amplifica @ 5kHz |
| Botón "Usar C" | - | Activa modo C (referencia) |
| Botón "Usar ASM" | - | Activa modo ASM (optimizado) |
| Botón "GUARDAR" | - | Procesa y guarda archivo |

### Salida

Los archivos se guardan como:
- **Modo C**: `{NombreCanción}_C.wav`
- **Modo ASM**: `{NombreCanción}_Asm.wav`
- **Versionado**: Si existe, agrega sufijo (v2, v3, etc.)

---

## 🐛 Solución de Problemas

### "No encuentra libecualizador.so"

```bash
# Verifica que exista
ls -l libecualizador.so

# Si no existe, recompila (ver sección Compilación)
nasm -f elf64 Ecualizador.asm -o Ecualizador.o && \
gcc -fPIC -c InterfazDual.c -o InterfazDual.o && \
gcc -shared -o libecualizador.so Ecualizador.o InterfazDual.o -lm
```

### "ModuleNotFoundError: No module named 'customtkinter'"

```bash
source ecualizador/bin/activate
pip install customtkinter
python3 app.py
```

### "AttributeError: no function for ctypes interface"

```bash
# Verifica que existan los símbolos
nm libecualizador.so | grep procesar

# Debe mostrar ambos:
# procesar_ecualizador_ASM
# Procesar_Ecualizador_C

# Si no, recompila ASM y C
```

### "Audio con distorsión en ASM"

1. Verifica que Ecualizador.asm tenga cascada correcta (línea ~56)
2. Debe mostrar: `movups xmm0, xmm10` DENTRO del bucle de bandas
3. Recompila
4. Prueba nuevamente

### "Ejecutable no inicia"

```bash
# Dale permisos
chmod +x dist/EcualizadorASM

# Ejecuta desde terminal
./dist/EcualizadorASM

# Si error "cannot open shared object"
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
./dist/EcualizadorASM
```

---

## 📦 Empaquetamiento con PyInstaller

Crear ejecutable standalone:

```bash
rm -rf dist build
ecualizador/bin/python -m PyInstaller --onefile \
    --name EcualizadorASM \
    --add-binary "libecualizador.so:." \
    --collect-all customtkinter \
    app.py
```

**Resultado:**
- Ejecutable en `dist/EcualizadorASM`
- Tamaño: ~13MB
- No requiere Python instalado
- Compatible con cualquier Linux x86-64

---

## ✅ Comparación de Modos

Para verificar que ambos modos funcionan correctamente:

1. Carga mismo archivo WAV
2. Ajusta sliders de forma idéntica en ambos modos
3. Genera salida en modo C: `CancionName_C.wav`
4. Genera salida en modo ASM con mismos ajustes: `CancionName_Asm.wav`
5. Compara audio (escucha o analiza con Audacity)
6. **Ambos deben sonar IDÉNTICOS**

---

## 📚 Comandos Útiles

### Ver símbolos exportados

```bash
nm libecualizador.so | grep -i procesar
```

### Validar sintaxis ASM

```bash
nasm -f elf64 Ecualizador.asm -o /dev/null
```

### Información de archivo WAV

```bash
python3 << 'EOF'
import wave
w = wave.open('archivo.wav')
print(f"Canales: {w.getnchannels()}")
print(f"Frecuencia: {w.getframerate()} Hz")
print(f"Muestras: {w.getnframes()}")
print(f"Duración: {w.getnframes() / w.getframerate():.2f} segundos")
EOF
```

### Crear tono de prueba

```bash
python3 << 'EOF'
import wave, math
sr = 44100
freq = 440  # La (A4)
duration = 5

with wave.open('test_tone.wav', 'wb') as w:
    w.setnchannels(2)
    w.setsampwidth(2)
    w.setframerate(sr)
    for i in range(int(sr * duration)):
        val = int(32767 * 0.3 * math.sin(2 * math.pi * freq * i / sr))
        w.writeframes(val.to_bytes(2, 'little', signed=True) * 2)
print("test_tone.wav creado")
EOF
```

---

## 🎓 Detalles Técnicos Adicionales

### Estructura BiquadBand (52 bytes)

```c
typedef struct {
    float b0, b1, b2, a1, a2;       // 20 bytes (coeficientes)
    float x1L, x1R, x2L, x2R;       // 16 bytes (entrada previa L/R)
    float y1L, y1R, y2L, y2R;       // 16 bytes (salida previa L/R)
} BiquadBand;  // Total: 52 bytes
```

### Cálculo de Coeficientes (Fórmula Bristow-Johnson)

Implementado en `calcular_biquad()`:

```c
Q = 0.707f  // Ancho de banda estándar (Butterworth)
omega = 2 * PI * freq / sr
sin(omega), cos(omega) - componentes del filtro
a0, a1, a2, b0, b1, b2 - coeficientes normalizados
```

---

## 📖 Documentación Completa

Para instrucciones aún más detalladas, consulta:

📄 **[INSTRUCCIONES_EJECUCION.txt](INSTRUCCIONES_EJECUCION.txt)** - Guía paso a paso con 7 secciones completas

---

## 🤝 Soporte

Si encuentras problemas:

1. Lee la sección "🐛 Solución de Problemas"
2. Verifica que cumplas con Requisitos del Sistema
3. Consulta INSTRUCCIONES_EJECUCION.txt para pasos detallados
4. Recompila (sección Compilación Detallada)

---

## 📝 Licencia

Este proyecto es de código abierto.

---

**Última actualización:** 5 de mayo de 2026  
**Versión:** 2.0 (Dual-mode ASM + C)
