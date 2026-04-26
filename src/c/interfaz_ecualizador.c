#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define INPUT_AUDIO_PATH "assets/audio/entrada_prueba.wav"
#define OUTPUT_AUDIO_PATH "assets/audio/salida_ecualizador.wav"


#define M_PI 3.14159265358979323846


// Estructura exacta para el Ensamblador (Offsets de 52 bytes)

typedef struct {
    float b0, b1, b2, a1, a2;       // 20 bytes
    float x1L, x1R, x2L, x2R;       // 16 bytes
    float y1L, y1R, y2L, y2R;       // 16 bytes
} BiquadBand; // Total exacto: 52 bytes

typedef struct {
    BiquadBand bajos;
    BiquadBand medios;
    BiquadBand agudos;
} EqConfig;



extern void procesar_ecualizador(float* pSamples, long totalSamples, EqConfig* config);

// --- MATEMÁTICA DE FILTROS ---
void calcular_biquad(BiquadBand* banda, float freq, float sr, float dbGanancia) {
    // 1. Configuración de la "Campana"
  
    float Q = 0.707f; 
    
    // 2. Magnitud (A)

    float A = powf(10.0f, dbGanancia / 40.0f);

    // 3. Ubicación en el espectro (Omega)
   
    float omega = 2.0f * (float)M_PI * freq / sr;
    float sn = sinf(omega);
    float cs = cosf(omega);

    // 4. Parámetro de resonancia (Alpha)
   
    float alpha = sn / (2.0f * Q);

    // 5. Generación de Coeficientes Crudos
    // Aplicamos las fórmulas de Robert Bristow-Johnson para un Peaking EQ.

    float b0_raw = 1.0f + (alpha * A);
    float b1_raw = -2.0f * cs;
    float b2_raw = 1.0f - (alpha * A);
    float a0_raw = 1.0f + (alpha / A);
    float a1_raw = -2.0f * cs;
    float a2_raw = 1.0f - (alpha / A);

    // 6. Normalización;todo está dividio por a0.
    banda->b0 = b0_raw / a0_raw;
    banda->b1 = b1_raw / a0_raw;
    banda->b2 = b2_raw / a0_raw;
    banda->a1 = a1_raw / a0_raw;
    banda->a2 = a2_raw / a0_raw;
    
    // Limpiar estados anteriores (para evitar ruidos de memoria basura)
    banda->x1L = banda->x1R = banda->x2L = banda->x2R = 0;
    banda->y1L = banda->y1R = banda->y2L = banda->y2R = 0;
}

int main() {
    unsigned int channels, sampleRate;
    drwav_uint64 totalFrameCount;
    EqConfig config;

    // 1. CARGAR AUDIO
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(INPUT_AUDIO_PATH, &channels, &sampleRate, &totalFrameCount, NULL);
    if (!pSampleData) {
        printf("Error: No se encontro '%s'\n", INPUT_AUDIO_PATH);
        return 1;
    }

    // 2. PEDIR DATOS POR TECLADO
    float gBajos, gMedios, gAgudos;
    printf("=== ECUALIZADOR 3 BANDAS (ASM) ===\n");
    printf("Ganancia Bajos  (100Hz) en dB: "); scanf("%f", &gBajos);
    printf("Ganancia Medios (1kHz)  en dB: "); scanf("%f", &gMedios);
    printf("Ganancia Agudos (5kHz)  en dB: "); scanf("%f", &gAgudos);

    // 3. CALCULAR CONSTANTES
    calcular_biquad(&config.bajos,  100.0f,  (float)sampleRate, gBajos);
    calcular_biquad(&config.medios, 1000.0f, (float)sampleRate, gMedios);
    calcular_biquad(&config.agudos, 5000.0f, (float)sampleRate, gAgudos);

    printf ("Coeficientes calculados \n");
    // 4. PROCESAR EN ENSAMBLADOR
    long totalSamples = (long)(totalFrameCount * channels);
    procesar_ecualizador(pSampleData, totalSamples, &config);

    printf ("proceso terminado \n");
// 5. GUARDAR RESULTADO
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT; 
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    drwav wav;
    printf("Intentando inicializar archivo de salida...\n");
    if (!drwav_init_file_write(&wav, OUTPUT_AUDIO_PATH, &format, NULL)) {
        fprintf(stderr, "Error critico: No se pudo crear el archivo '%s'.\n", OUTPUT_AUDIO_PATH);
        return 1;
    }

    printf("Escribiendo frames en el disco...\n");
    drwav_uint64 framesEscritos = drwav_write_pcm_frames(&wav, totalFrameCount, pSampleData);
    
    if (framesEscritos != totalFrameCount) {
        printf("Advertencia: Se escribieron menos frames de los esperados.\n");
    }

    drwav_uninit(&wav);
    printf("Archivo cerrado correctamente.\n");

    drwav_free(pSampleData, NULL);
    return 0;
}