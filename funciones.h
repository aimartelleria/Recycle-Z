#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <SDL_timer.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define FILAS 100
#define COLUMNAS 100
#define TAMANO_CELDA 32
#define MAX_LINE_LENGTH 1024
#define MAPA_WIDTH 3200
#define MAPA_HEIGHT 3200
#define NUM_ANIMATIONS 4
#define NUM_FRAMES_PER_ANIMATION 6
#define NUM_ENEMIGOS 25
#define NUM_ENEMY_ANIMATIONS 4
#define NUM_ENEMY_FRAMES_PER_ANIMATION 3
#define MAX_PROYECTILES 1
#define DISTANCIA_MAXIMA_PROYECTIL 500.0f  // Puedes ajustar este valor según tus necesidades

typedef struct
{
    float x;
    float y;
    int ancho;
    int alto;
    int biziak;
    int abiadura;
} Jugador;

typedef struct
{
    float x;
    float y;
    int ancho;
    int alto;
    int currentAnimation;
    int currentFrame;
    int activo;  // Agregamos una propiedad para saber si el enemigo está activo
    int abiadura;
} Enemigo;

// Estructura para las balas (Proyectiles)
typedef struct
{
    float x;
    float y;
    float direccionX;
    float direccionY;
    int activo;
    int currentAnimation;  // Nueva propiedad para la animación de la bala
    int abiadura;
    int kaltea;
} Proyectil;

// Estructura para representar un botón
typedef struct
{
    SDL_Rect rect; // Rectángulo que define la posición y el tamaño del botón
    SDL_Texture* texture; // Textura que representa la imagen del botón
} Button;

void cargarTexturas();

void dibujarPantallaGameOver();

void dibujarPantallaZorionak();

void inicializarMatriz();

void matrizea_bueltatu();

int colisionConParedes(Jugador* jugador, int nuevaX, int nuevaY);

void dibujarMatriz(int camaraX, int camaraY);

void dibujarJugador(Jugador* jugador, int camaraX, int camaraY, int currentAnimation, int currentFrame);

void dibujarMapaEnVentana(int camaraX, int camaraY);

void dibujarEnemigos(Enemigo* enemigos, int numEnemigos, int camaraX, int camaraY);

int colisionConParedesEnemigo(Enemigo* enemigo, int nuevaX, int nuevaY);

void moverEnemigos(Enemigo* enemigos, int numEnemigos, Jugador* jugador);

int colisionConEnemigos(Jugador* jugador, Enemigo* enemigos, int numEnemigos);

// Función para actualizar las balas
void actualizarProyectiles(Jugador* jugador);

// Función para dibujar las balas
void dibujarProyectiles(int camaraX, int camaraY);

void dispararProyectil(Jugador* jugador, float direccionX, float direccionY);

void colisionProyectilEnemigo(Enemigo* enemigos, int numEnemigos);

void mapa_pantailaratu();

int initSDLMix();

int loadSounds();

// Función para cargar una imagen como textura
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename);

// Función para crear un botón
Button createButton(int x, int y, int width, int height, SDL_Renderer* renderer, const char* filename);

void hasi();

void itxi();

void renderTexture(SDL_Texture* texture, int x, int y);

void kargatu_kredituak();

void kargatu_kontrolak();
#endif
