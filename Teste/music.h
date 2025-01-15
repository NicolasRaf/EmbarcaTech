#ifndef MUSIC_H
#define MUSIC_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

const int speakPin1 = 21;
const int speakPin2 = 10;

const int e5 = 659; // E5
const int c4 = 261; // C4
const int a5 = 880;
const int c5 = 523;
const int f5 = 698;
const int g5 = 784;
const int b4 = 466;
const int c5s = 554;

int volume = 2; // Volume percentual
int currentSpeaker = speakPin1;

// Estrutura para armazenar estado da música
typedef struct {
    int noteIndex;
    int duration;
    bool playing;
    int notes[17];
} MusicState;

MusicState musicState = {
    .noteIndex = 0,
    .duration = 300,
    .playing = false,
    .notes = {a5, c5, f5, c5, a5, c5, f5, c5, a5, c5, f5, c5, b4, c5s, f5, g5, a5}
};

// Configuração inicial dos GPIOs
void setup() {
    #ifndef SETUP_START
    gpio_init(speakPin1);
    gpio_set_dir(speakPin1, GPIO_OUT);
    gpio_init(speakPin2);
    gpio_set_dir(speakPin2, GPIO_OUT);
    stdio_init_all();
    #define SETUP_START
    #endif
}


// Toca uma única nota com volume ajustado
void playNoteWithVolume(int frequency, int duration, int volumePercent) {
    float volume = volumePercent / 100.0;
    int period = 1000000 / frequency; // Período da onda em microssegundos
    int pulseWidth = period * volume;

    absolute_time_t startTime = get_absolute_time();
    while (absolute_time_diff_us(startTime, get_absolute_time()) < duration * 1000) {
        gpio_put(currentSpeaker, 1);
        busy_wait_us_32(pulseWidth);
        gpio_put(currentSpeaker, 0);
        busy_wait_us_32(period - pulseWidth);
    }

    // Alterna entre os alto-falantes
    currentSpeaker = (currentSpeaker == speakPin1) ? speakPin2 : speakPin1;
}

// Toca um som de confirmação
void playConfirmationSound() {
    playNoteWithVolume(g5, 100, volume); // Toca a nota G5 por 100ms
    busy_wait_us_32(50000);             // Pausa de 50ms
    playNoteWithVolume(a5, 100, volume); // Toca a nota A5 por 100ms
}

// Callback do alarme para tocar a próxima nota
bool musicCallback(repeating_timer_t *timer) {
    if (!musicState.playing || musicState.noteIndex >= 17) {
        musicState.playing = false;
        return false; // Cancela o alarme
    }

    int note = musicState.notes[musicState.noteIndex];
    playNoteWithVolume(note, musicState.duration, volume);
    musicState.noteIndex++;
    return true; // Continua o timer
}

// Inicia a música de forma não bloqueante
void playMusic() {
    setup();

    if (musicState.playing) {
        return; // Já está tocando
    }

    printf("Tocando Musica...\n");

    musicState.noteIndex = 0;
    musicState.playing = true;

    static repeating_timer_t timer;
    add_repeating_timer_ms(musicState.duration, musicCallback, NULL, &timer);
}

// Toca um som de confirmação
void playConfirmSound() {
    setup();
    int confirmNotes[] = {c5, e5, g5, c5}; // Sequência de notas de confirmação
    int confirmDuration = 200;             // Duração de cada nota em ms

    for (int i = 0; i < 4; i++) {          // Agora são 4 notas
        playNoteWithVolume(confirmNotes[i], confirmDuration, volume);
        busy_wait_ms(50); // Pequena pausa entre as notas
    }
}

// Toca um som de erro grave com frequências exatas
void playErrorSound() {
    setup();
    
    // Notas próximas entre si para criar um som de erro "ruidoso"
    int errorNotes[] = {c4, c4 + 10, c4 - 10};  // Frequências próximas para um efeito de erro
    int errorDuration = 150;         // Duração mais curta para um som mais abrupto

    for (int i = 0; i < 3; i++) {    // Três notas próximas para o som de erro
        playNoteWithVolume(errorNotes[i], errorDuration, volume);
        busy_wait_ms(50); // Pausa curta entre as notas para manter a sensação de erro rápido
    }
}

#endif // MUSIC_H