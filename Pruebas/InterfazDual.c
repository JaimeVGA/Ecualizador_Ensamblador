#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define M_PI 3.14159265358979323846

#pragma pack(push, 1) 
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
#pragma pack(pop) 


extern void procesar_ecualizador_ASM(float* pSamples, long totalSamples, EqConfig* config);

void procesar_ecualizador_C(float* muestras, long total_muestras, EqConfig* config) {
    
    float L,R, bajos_salidaL, bajos_salidaR, medios_salidaL, medios_salidaR, agudos_salidaL, agudos_salidaR;
    for (long i = 0; i < total_muestras; i += 2) {//avanza de dos en dos porque trabajamso con los dos canales
        
        L = muestras[i];     // Canal Izquierdo
        R = muestras[i + 1]; // Canal Derecho

        //Primero aplicamos la formula a los bajos
        bajos_salidaL = (config->bajos.b0 * L) + (config->bajos.b1 * config->bajos.x1L) + (config->bajos.b2 * config->bajos.x2L) 
                        - (config->bajos.a1 * config->bajos.y1L) - (config->bajos.a2 * config->bajos.y2L);
        
        bajos_salidaR = (config->bajos.b0 * R) + (config->bajos.b1 * config->bajos.x1R) + (config->bajos.b2 * config->bajos.x2R) 
                        - (config->bajos.a1 * config->bajos.y1R) - (config->bajos.a2 * config->bajos.y2R);

        // actualizamos cada dato de los bajos, es decir, vamos cambiando las salidass
        config->bajos.x2L = config->bajos.x1L; 
        config->bajos.x1L = L;
        config->bajos.y2L = config->bajos.y1L; 
        config->bajos.y1L = bajos_salidaL;
        config->bajos.x2R = config->bajos.x1R; 
        config->bajos.x1R = R;
        config->bajos.y2R = config->bajos.y1R; 
        config->bajos.y1R = bajos_salidaR;


        //aplicamos la formula a los medios usando las salidas de los bajos
        medios_salidaL = (config->medios.b0 * bajos_salidaL) + (config->medios.b1 * config->medios.x1L) + (config->medios.b2 * config->medios.x2L) 
                         - (config->medios.a1 * config->medios.y1L) - (config->medios.a2 * config->medios.y2L);
        
        medios_salidaR = (config->medios.b0 * bajos_salidaR) + (config->medios.b1 * config->medios.x1R) + (config->medios.b2 * config->medios.x2R) 
                         - (config->medios.a1 * config->medios.y1R) - (config->medios.a2 * config->medios.y2R);

        // Actualizamos los datos de losm medios
        config->medios.x2L = config->medios.x1L; 
        config->medios.x1L = bajos_salidaL;
        config->medios.y2L = config->medios.y1L; 
        config->medios.y1L = medios_salidaL;
        config->medios.x2R = config->medios.x1R; 
        config->medios.x1R = bajos_salidaR;
        config->medios.y2R = config->medios.y1R; 
        config->medios.y1R = medios_salidaR;


        //ahora actualizamos la banda de agudos, usando las salidas de los medios
        agudos_salidaL = (config->agudos.b0 * medios_salidaL) + (config->agudos.b1 * config->agudos.x1L) + (config->agudos.b2 * config->agudos.x2L) 
                         - (config->agudos.a1 * config->agudos.y1L) - (config->agudos.a2 * config->agudos.y2L);
        
        agudos_salidaR = (config->agudos.b0 * medios_salidaR) + (config->agudos.b1 * config->agudos.x1R) + (config->agudos.b2 * config->agudos.x2R) 
                         - (config->agudos.a1 * config->agudos.y1R) - (config->agudos.a2 * config->agudos.y2R);

        //del mismo modo actsualizamos lso datos de los agudos
        config->agudos.x2L = config->agudos.x1L; 
        config->agudos.x1L = medios_salidaL;
        config->agudos.y2L = config->agudos.y1L; 
        config->agudos.y1L = agudos_salidaL;
        config->agudos.x2R = config->agudos.x1R; 
        config->agudos.x1R = medios_salidaR;
        config->agudos.y2R = config->agudos.y1R; 
        config->agudos.y1R = agudos_salidaR;


        // la salida de los agudos seran la entradas para la siguiente muestra
        muestras[i]     = agudos_salidaL;
        muestras[i + 1] = agudos_salidaR;
    }
}

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

// Esta es la función que Python va a llamar
int ecualizar_archivo_ASM(const char* ruta_entrada, const char* ruta_salida, float gBajos, float gMedios, float gAgudos) {
    unsigned int channels, sampleRate;
    drwav_uint64 totalFrameCount;
    EqConfig config = {0}; // Limpiar basura de memoria

    // 1. CARGAR AUDIO
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(ruta_entrada, &channels, &sampleRate, &totalFrameCount, NULL);
    if (!pSampleData || channels == 0) {
        return -1; // Código de error para Python: Archivo no encontrado
    }

    // 2. CALCULAR CONSTANTES
    calcular_biquad(&config.bajos,  100.0f,  (float)sampleRate, gBajos);
    calcular_biquad(&config.medios, 1000.0f, (float)sampleRate, gMedios);
    calcular_biquad(&config.agudos, 5000.0f, (float)sampleRate, gAgudos);

    // 3. PROCESAR EN ENSAMBLADOR
    long totalSamples = (long)(totalFrameCount * channels);
    procesar_ecualizador_ASM(pSampleData, totalSamples, &config);

    // 4. GUARDAR RESULTADO
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT; 
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    drwav wav;
    if (!drwav_init_file_write(&wav, ruta_salida, &format, NULL)) {
        drwav_free(pSampleData, NULL);
        return -2; // Código de error para Python: Error al guardar
    }

    drwav_write_pcm_frames(&wav, totalFrameCount, pSampleData);
    drwav_uninit(&wav);
    drwav_free(pSampleData, NULL);
    
    return 0; 
}

int Procesar_Ecualizador_C(const char* ruta_entrada, const char* ruta_salida, float gBajos, float gMedios, float gAgudos) {
    unsigned int channels, sampleRate;
    drwav_uint64 totalFrameCount;
    EqConfig config = {0}; // Limpiar basura de memoria

    // 1. CARGAR AUDIO
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(ruta_entrada, &channels, &sampleRate, &totalFrameCount, NULL);
    if (!pSampleData || channels == 0) {
        return -1; // Código de error para Python: Archivo no encontrado
    }

    // 2. CALCULAR CONSTANTES
    calcular_biquad(&config.bajos,  100.0f,  (float)sampleRate, gBajos);
    calcular_biquad(&config.medios, 1000.0f, (float)sampleRate, gMedios);
    calcular_biquad(&config.agudos, 5000.0f, (float)sampleRate, gAgudos);

    // 3. PROCESAR EN C
    long totalSamples = (long)(totalFrameCount * channels);
    procesar_ecualizador_C(pSampleData, totalSamples, &config);

    // 4. GUARDAR RESULTADO
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT; 
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    drwav wav;
    if (!drwav_init_file_write(&wav, ruta_salida, &format, NULL)) {
        drwav_free(pSampleData, NULL);
        return -2; // Código de error para Python: Error al guardar
    }

    drwav_write_pcm_frames(&wav, totalFrameCount, pSampleData);
    drwav_uninit(&wav);
    drwav_free(pSampleData, NULL);
    
    return 0; 
} 