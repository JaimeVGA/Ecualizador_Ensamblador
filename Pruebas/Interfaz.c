#include <stdio.h>
#include <stdlib.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"


// RDI = pSamples, RSI = totalFrames, RDX = channels, XMM0 = volume_gain
extern void procesar_volumen(float* pSamples, drwav_uint64 totalFrames, unsigned int channels, float volume_gain);

int main() {
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalFrameCount;
        
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32("/home/jaimevga/Documentos/Ecualizador_Ensamblador/Audios Prueba/entrada.wav", &channels, &sampleRate, &totalFrameCount, NULL);

    printf("Procesando en Ensamblador (Frames: %llu, Canales: %u)...\n", (unsigned long long)totalFrameCount, channels);

    // 2. Llamar a tu función en ensamblador
    // Nota: El volumen 1.5f viajará en el registro XMM0
    procesar_volumen(pSampleData, totalFrameCount, channels, 0.2f);

    // 3. Guardar el resultado
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    // Cambiamos drwav_open_file_write por drwav_init_file_write que es lo que pide tu versión
    drwav wav; 
    if (!drwav_init_file_write(&wav, "/home/jaimevga/Documentos/Ecualizador_Ensamblador/Audios Prueba/salida.wav", &format, NULL)) {
        fprintf(stderr, "Error al crear el archivo de salida.\n");
        drwav_free(pSampleData, NULL);
        return 1;
    }

    drwav_write_pcm_frames(&wav, totalFrameCount, pSampleData);
    
    // En lugar de drwav_close, usamos drwav_uninit para esta API
    drwav_uninit(&wav);

    printf("¡Éxito! Archivo guardado.\n");

    drwav_free(pSampleData, NULL);
    return 0;
}