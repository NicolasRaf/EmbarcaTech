#include "analogic.h"

void selectDirectionWindRose(uint16_t x, uint16_t y, char *dir) {
    if (y > 3000 && x > 2000 && x < 3000) {
        strcpy(dir, "Norte");
    } else if (y < 1000 && x > 2000 && x < 3000) {
        strcpy(dir, "Sul");
    } else if (x > 3000 && y > 2000 && y < 3000) {
        strcpy(dir, "Leste");
    } else if (x < 1000 && y > 2000 && y < 3000) {
        strcpy(dir, "Oeste");
    } else if (x > 3000 && y > 3000) {
        strcpy(dir, "Nordeste");
    } else if (x < 1000 && y > 3000) {
        strcpy(dir, "Noroeste");
    } else if (x > 3000 && y < 1000) {
        strcpy(dir, "Sudeste");
    } else if (x < 1000 && y < 1000) {
        strcpy(dir, "Sudoeste");
    } else {
        strcpy(dir, "Centro");
    }
}