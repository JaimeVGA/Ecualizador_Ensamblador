#include <stdio.h>
#include <stdlib.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

// Definimos la estructura para que C sepa cuánta memoria reservar
typedef struct {
    float b0, b1, b2, a1, a2;
    float x1L, x1R, x2L, x2R, y1L, y1R, y2L, y2R;
} BiquadBand;

typedef struct {
    BiquadBand bajos;
    BiquadBand medios;
    BiquadBand agudos;
} EqConfig;

// Interfaz de prueba para verificar desde C el flujo completo del ecualizador: carga WAV, procesa en ASM y guarda el resultado.
// Cambiamos el extern para que coincida con el nuevo motor de 3 bandas
extern void procesar_ecualizador(float* pSamples, long totalSamples, EqConfig* config);

int main() {
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalFrameCount;
    
    // 1. CARGA CON VALIDACIÓN
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32("/home/jaimevga/Documentos/Ecualizador_Ensamblador/Audios Prueba/entrada.wav", &channels, &sampleRate, &totalFrameCount, NULL);

    if (pSampleData == NULL) {
        fprintf(stderr, "ERROR: No se pudo leer el archivo. Revisa la ruta.\n");
        return 1;
    }

    // 2. CONFIGURACIÓN DEL ECUALIZADOR
    EqConfig miConfig = {0}; // Inicializar todo en 0.0f
    
    // Aquí es donde meterías los 5 coeficientes de cada banda
    // Por ahora, b0=1.0 para que el audio pase sin cambios
    miConfig.bajos.b0 = 1.0f;
    miConfig.medios.b0 = 2.0f;
    miConfig.agudos.b0 = 0.0f;

    // Calculamos total de muestras reales (IMPORTANTE para estéreo)
    long totalSamples = (long)(totalFrameCount * channels);

    printf("Procesando: %u canales, %llu frames (%ld muestras totales)\n", channels, totalFrameCount, totalSamples);

    // 3. LLAMADA AL MOTOR DE ENSAMBLADOR
    procesar_ecualizador(pSampleData, totalSamples, &miConfig);

    // 4. GUARDADO
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    drwav wav; 
    if (!drwav_init_file_write(&wav, "/home/jaimevga/Documentos/Ecualizador_Ensamblador/Audios Prueba/salida.wav", &format, NULL)) {
        fprintf(stderr, "Error al crear el archivo de salida.\n");
        drwav_free(pSampleData, NULL);
        return 1;
    }

    drwav_write_pcm_frames(&wav, totalFrameCount, pSampleData);
    drwav_uninit(&wav);

    printf("¡Éxito! Archivo guardado correctamente.\n");

    drwav_free(pSampleData, NULL);
    return 0;
}