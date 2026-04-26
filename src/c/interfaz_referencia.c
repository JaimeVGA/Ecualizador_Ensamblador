#include <stdio.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define INPUT_AUDIO_PATH "assets/audio/entrada.wav"
#define OUTPUT_AUDIO_PATH "assets/audio/salida_referencia.wav"

// Tu función en ensamblador
extern void procesar_ecualizador(float* pSamples, drwav_uint64 totalFrames, unsigned int channels, float bass_gain);

int main() {
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalFrameCount;
    
    // 1. Cargar el archivo WAV (dr_wav lo convierte a float automáticamente)
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(INPUT_AUDIO_PATH, &channels, &sampleRate, &totalFrameCount, NULL);

    if (pSampleData == NULL) return 1;

    // 2. Llamar a tu "bestia" en ensamblador
    // Pasamos el puntero, el tamaño y, por ejemplo, la ganancia que viene de Python
    procesar_ecualizador(pSampleData, totalFrameCount, channels, 1.5f);

    // 3. Guardar el resultado
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    drwav* pWavWrite = drwav_open_file_write(OUTPUT_AUDIO_PATH, &format, NULL);
    drwav_write_pcm_frames(pWavWrite, totalFrameCount, pSampleData);
    drwav_close(pWavWrite);

    drwav_free(pSampleData, NULL);
    return 0;
}