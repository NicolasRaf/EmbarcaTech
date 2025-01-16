#ifndef MUSIC_H
#define MUSIC_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Definindo os pinos do alto-falante
const int speakPin1 = 21;
const int speakPin2 = 10;

// Definindo as frequências das notas
const int e5 = 659;  // E5
const int c4 = 261;  // C4
const int a5 = 880;
const int c5 = 523;
const int f5 = 698;
const int g5 = 784;
const int b4 = 466;
const int c5s = 554;

int volume = 2; // Volume percentual
int currentSpeaker = speakPin1;

// Estrutura para armazenar o estado da música
typedef struct {
    int noteIndex;
    int duration;
    bool playing;
    int notes[17];
} MusicState;

// Estado inicial da música
MusicState musicState = {
    .noteIndex = 0,
    .duration = 200,
    .playing = false,
    .notes = {a5, c5, f5, c5, a5, c5, f5, c5, a5, c5, f5, c5, b4, c5s, f5, g5, a5}
};

// Função de configuração inicial
void setup();

// Função para tocar uma nota com volume
void playNoteWithVolume(int frequency, int duration, int volumePercent);

// Função de callback para tocar a música
bool musicCallback(repeating_timer_t *timer);

// Função para iniciar a música
void playMusic();

// Função para tocar um som de confirmação
void playConfirmSound();

// Função para tocar um som de erro
void playErrorSound();

#endif // MUSIC_H
