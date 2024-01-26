#define _CRT_SECURE_NO_WARNINGS
#include "funciones.h"

Mix_Chunk* clickSound;  // Declarar el sonido
char kolisio_matrizea[FILAS][COLUMNAS][MAX_LINE_LENGTH];
int matriz[FILAS][COLUMNAS];
int vidaspawn = 45; // Número inicial de vidas
int puntos = 0;
// Texturas para las animaciones de las balas
SDL_Texture* projectileImages[NUM_ANIMATIONS];

Proyectil proyectiles[MAX_PROYECTILES];

SDL_Texture* mapaTexture;
SDL_Texture* colisionTexture;
SDL_Texture* playerImages[NUM_ANIMATIONS * NUM_FRAMES_PER_ANIMATION];
SDL_Texture* enemyImages[NUM_ENEMY_ANIMATIONS * NUM_ENEMY_FRAMES_PER_ANIMATION];
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* Kredituak_Panela;
SDL_Texture* Hasiera_botoia;
SDL_Texture* Settings_botoia;
SDL_Texture* Kredituak_botoia;
SDL_Texture* Hasierako_menu;
SDL_Renderer* renderer2;
Mix_Music* musicaDeFondo = NULL;
Mix_Chunk* sonidoDisparo = NULL;
Jugador jugador = { 500.0f, 3000.0f, TAMANO_CELDA, TAMANO_CELDA, 3,2 }; // 3 vidas iniciales
TTF_Font* font; // Declaración global de la fuente
SDL_Color textColor; // Declaración global de textColor
SDL_Texture* imagenGameOver = NULL;
SDL_Texture* imagenZorionak;

void hasi()
{
    //Inicializar SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    // Inicializar SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "SDL_image no pudo inicializarse! SDL_image Error: %s\n", IMG_GetError());
    }

    //Inicializar SDL_ttf
    if (TTF_Init() < 0)
    {
        fprintf(stderr, "Error al inicializar SDL_ttf: %s\n", TTF_GetError());
    }

    // Crear ventana
    window = SDL_CreateWindow("Jokoa", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1792, 1024, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    // Crear renderizador
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    }
}

void itxi()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
SDL_Texture* cargarImagen(const char* ruta)
{
    SDL_Surface* surface = IMG_Load(ruta);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void cargarTexturas()
{
    SDL_Surface* imagenSurface = NULL; // Declarar las variables fuera de los bloques condicionales
    SDL_Surface* loadedSurface = NULL;

    if (vidaspawn <= 0)
    {
        imagenSurface = IMG_Load("Segundo_Mapa.png");
    }
    else
    {
        // Cargar el primer mapa por defecto
        imagenSurface = IMG_Load("Primer_Mapa.png");
    }

    if (imagenSurface == NULL)
    {
        printf("Error al cargar la imagen: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    // Escalar la imagen al nuevo tamaño del mapa
    SDL_Surface* mapaSurface = SDL_CreateRGBSurface(0, MAPA_WIDTH, MAPA_HEIGHT, 32, 0, 0, 0, 0);
    SDL_BlitScaled(imagenSurface, NULL, mapaSurface, NULL);
    SDL_FreeSurface(imagenSurface);

    // Crear una textura desde la superficie del mapa escalado
    mapaTexture = SDL_CreateTextureFromSurface(renderer, mapaSurface);
    SDL_FreeSurface(mapaSurface);

    if (mapaTexture == NULL)
    {
        printf("Error al crear la textura: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }
    // Cargar música de fondo
    musicaDeFondo = Mix_LoadMUS("Musika/BackGroundMusic.mp3");
    if (musicaDeFondo == NULL)
    {
        printf("Error al cargar la música de fondo: %s\n", Mix_GetError());
    }

    // Cargar efecto de sonido de disparo
    sonidoDisparo = Mix_LoadWAV("Musika/BalaSoinua.mp3");
    if (sonidoDisparo == NULL)
    {
        printf("Error al cargar el sonido de disparo: %s\n", Mix_GetError());
    }

    // Inicializar SDL_ttf
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf no pudo inicializarse: %s\n", TTF_GetError());
        exit(1);
    }

    // Cargar la fuente
    font = TTF_OpenFont("Fonts/Roboto.ttf", 24);
    if (!font)
    {
        printf("Error al cargar la fuente: %s\n", TTF_GetError());
        exit(1);
    }

    // Establecer color del texto
    textColor = (SDL_Color){ 255, 255, 255, 255 }; // Color del texto (blanco)

    // Crear una textura para las colisiones (invisibles)
    SDL_Surface* colisionSurface = SDL_CreateRGBSurface(0, TAMANO_CELDA, TAMANO_CELDA, 32, 0, 0, 0, 0);
    SDL_SetColorKey(colisionSurface, SDL_TRUE, SDL_MapRGB(colisionSurface->format, 0, 0, 0));
    SDL_FillRect(colisionSurface, NULL, SDL_MapRGBA(colisionSurface->format, 0, 0, 0, 0));  // Colisión invisible

    colisionTexture = SDL_CreateTextureFromSurface(renderer, colisionSurface);
    SDL_FreeSurface(colisionSurface);

    if (colisionTexture == NULL)
    {
        printf("Error al crear la textura de colisiones: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    // Cargar las imágenes del jugador
    char animacionNombres[NUM_ANIMATIONS][20] = { "Gora", "Behera", "Ezkerra", "Eskubi" };
    char animacionNumber[NUM_FRAMES_PER_ANIMATION][20] = { "Bat", "Bi", "Hiru", "Lau", "Bost", "Sei" };
    char imagePath[100];
    for (int i = 0; i < NUM_ANIMATIONS; ++i)
    {
        for (int j = 0; j < NUM_FRAMES_PER_ANIMATION; ++j)
        {
            // Crear la ruta de la imagen basada en el nombre y número de frame
            sprintf(imagePath, "images/%s%s.png", animacionNombres[i], animacionNumber[j]);

            // Cargar la imagen
            SDL_Surface* loadedSurface = IMG_Load(imagePath);
            if (loadedSurface == NULL)
            {
                printf("Error al cargar la imagen %s: %s\n", imagePath, IMG_GetError());
                // Puedes agregar más manejo de errores o salir del bucle aquí si lo deseas
            }
            else
            {
                // Asignar la imagen cargada a la textura correspondiente
                playerImages[i * NUM_FRAMES_PER_ANIMATION + j] = SDL_CreateTextureFromSurface(renderer, loadedSurface);
                SDL_FreeSurface(loadedSurface);  // Liberar la superficie temporal
            }
        }
    }// Cargar las imágenes de los enemigos
    char enemyAnimationNames[NUM_ENEMY_ANIMATIONS][20] = { "z-Gora", "z-Behera", "z-Ezk", "z-Esk" };
    char enemyAnimationNumber[NUM_ENEMY_FRAMES_PER_ANIMATION][20] = { "Bat", "Bi", "Hiru" };
    char enemyImagePath[100];

    for (int i = 0; i < NUM_ENEMY_ANIMATIONS; ++i)
    {
        for (int j = 0; j < NUM_ENEMY_FRAMES_PER_ANIMATION; ++j)
        {
            // Crear la ruta de la imagen basada en el nombre y número de frame
            sprintf(enemyImagePath, "images/%s%s.png", enemyAnimationNames[i], enemyAnimationNumber[j]);

            // Cargar la imagen
            SDL_Surface* enemyLoadedSurface = IMG_Load(enemyImagePath);
            if (enemyLoadedSurface == NULL)
            {
                printf("Error al cargar la imagen %s: %s\n", enemyImagePath, IMG_GetError());
                // Puedes agregar más manejo de errores o salir del bucle aquí si lo deseas
            }
            else
            {
                // Asignar la imagen cargada a la textura correspondiente
                enemyImages[i * NUM_ENEMY_FRAMES_PER_ANIMATION + j] = SDL_CreateTextureFromSurface(renderer, enemyLoadedSurface);
                SDL_FreeSurface(enemyLoadedSurface);  // Liberar la superficie temporal
            }
        }
    }
    // Cargar las imágenes de las balas
    char projectileImagePaths[NUM_ANIMATIONS][100];
    char projectileImageNumber[NUM_ANIMATIONS][20] = { "BalaGora", "BalaBehera", "BalaEskubi", "BalaEzkerra" };

    for (int i = 0; i < NUM_ANIMATIONS; ++i)
    {
        // Crear la ruta de la imagen basada en el número de frame
        sprintf(projectileImagePaths[i], "images/%s.png", projectileImageNumber[i]);

        // Cargar la imagen
        SDL_Surface* projectileLoadedSurface = IMG_Load(projectileImagePaths[i]);
        if (projectileLoadedSurface == NULL)
        {
            printf("Error al cargar la imagen %s: %s\n", projectileImagePaths[i], IMG_GetError());
            // Puedes agregar más manejo de errores o salir del bucle aquí si lo deseas
        }
        else
        {
            // Asignar la imagen cargada a la textura correspondiente
            projectileImages[i] = SDL_CreateTextureFromSurface(renderer, projectileLoadedSurface);
            SDL_FreeSurface(projectileLoadedSurface);  // Liberar la superficie temporal
        }
    }
    imagenGameOver = cargarImagen("Gameover.png");
    imagenZorionak = cargarImagen("Zorionak.png");
}

void dibujarPantallaGameOver()
{
    // Limpiar la ventana
    SDL_RenderClear(renderer);

    // Obtener las dimensiones de la ventana
    int ventanaAncho, ventanaAlto;
    SDL_GetWindowSize(window, &ventanaAncho, &ventanaAlto);

    // Renderizar la imagen de Game Over en pantalla completa
    SDL_Rect destRect = { 0, 0, ventanaAncho, ventanaAlto };
    SDL_RenderCopy(renderer, imagenGameOver, NULL, &destRect);

    // Presentar la ventana
    SDL_RenderPresent(renderer);
}

void dibujarPantallaZorionak()
{
    // Limpiar la ventana
    SDL_RenderClear(renderer);

    // Obtener las dimensiones de la ventana
    int ventanaAncho, ventanaAlto;
    SDL_GetWindowSize(window, &ventanaAncho, &ventanaAlto);

    // Renderizar la imagen "zorionak" en pantalla completa
    SDL_Rect destRect = { 0, 0, ventanaAncho, ventanaAlto };
    SDL_RenderCopy(renderer, imagenZorionak, NULL, &destRect);  // Asegúrate de tener imagenZorionak cargada

    // Presentar la ventana
    SDL_RenderPresent(renderer);
}

void inicializarMatriz()
{
    // Llena la matriz con ceros
    for (int i = 0; i < FILAS; ++i)
    {
        for (int j = 0; j < COLUMNAS; ++j)
        {
            matriz[i][j] = 0;
        }
    }
}

void matrizea_bueltatu()
{
    int fila = 0, kolumna = 0;
    FILE* archivo;
    char linea[MAX_LINE_LENGTH];
    if (vidaspawn <= 0)
    {
        archivo = fopen("Mapa-2_Kolisioak.txt", "r");
    }
    else
    {
        archivo = fopen("Kolisiok-Matrizeak.txt", "r");
    }

    // Verifica si el archivo se abrió correctamente
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
        exit(1);
    }

    // Lee línea por línea del archivo
    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        // Elimina el carácter de nueva línea si está presente
        linea[strcspn(linea, "\n")] = '\0';

        // Utiliza strtok para dividir la línea en campos basados en comas
        char* token = strtok(linea, ",");
        kolumna = 0;  // Reinicia la fila para cada nueva línea

        while (token != NULL && kolumna < COLUMNAS)
        {
            strcpy(kolisio_matrizea[fila][kolumna], token);
            matriz[fila][kolumna] = atoi(token); // Convierte a entero y almacena en la matriz
            kolumna++;

            // Obtiene el siguiente token
            token = strtok(NULL, ",");
        }

        fila++;
    }

    // Cierra el archivo
    fclose(archivo);
}

int colisionConParedes(Jugador* jugador, int nuevaX, int nuevaY)
{
    int celdaIzquierda = nuevaX / TAMANO_CELDA;
    int celdaDerecha = (nuevaX + jugador->ancho - 1) / TAMANO_CELDA;
    int celdaArriba = nuevaY / TAMANO_CELDA;
    int celdaAbajo = (nuevaY + jugador->alto - 1) / TAMANO_CELDA;

    for (int i = celdaArriba; i <= celdaAbajo; ++i)
    {
        for (int j = celdaIzquierda; j <= celdaDerecha; ++j)
        {
            if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS)
            {
                if (matriz[i][j] == 1)
                {
                    return 1;  // Hay colisión
                }
            }
        }
    }

    return 0;  // No hay colisión
}

void dibujarMatriz(int camaraX, int camaraY)
{
    for (int i = 0; i < FILAS; ++i)
    {
        for (int j = 0; j < COLUMNAS; ++j)
        {
            if (matriz[i][j] == 1)
            {
                SDL_Rect celdaRect = { j * TAMANO_CELDA - camaraX, i * TAMANO_CELDA - camaraY, TAMANO_CELDA, TAMANO_CELDA };
                SDL_RenderCopy(renderer, colisionTexture, NULL, &celdaRect);
            }
        }
    }
}

void dibujarJugador(Jugador* jugador, int camaraX, int camaraY, int currentAnimation, int currentFrame)
{
    SDL_Rect jugadorRect = { (int)jugador->x - camaraX, (int)jugador->y - camaraY, jugador->ancho, jugador->alto };

    // Obtener el índice de la textura del jugador basado en la animación actual
    int textureIndex = currentAnimation * NUM_FRAMES_PER_ANIMATION + currentFrame;

    // Renderizar la textura del personaje
    SDL_RenderCopy(renderer, playerImages[textureIndex], NULL, &jugadorRect);
}

void dibujarMapaEnVentana(int camaraX, int camaraY)
{
    // Renderizar la textura del mapa en la ventana
    SDL_Rect mapaRect = { -camaraX, -camaraY, MAPA_WIDTH, MAPA_HEIGHT };
    SDL_RenderCopy(renderer, mapaTexture, NULL, &mapaRect);
}

void dibujarEnemigos(Enemigo* enemigos, int numEnemigos, int camaraX, int camaraY)
{
    for (int i = 0; i < numEnemigos; ++i)
    {
        SDL_Rect enemigoRect = { (int)enemigos[i].x - camaraX, (int)enemigos[i].y - camaraY, enemigos[i].ancho, enemigos[i].alto };

        // Obtener el índice de la textura del enemigo basado en la animación actual
        int textureIndex = enemigos[i].currentAnimation * NUM_ENEMY_FRAMES_PER_ANIMATION + enemigos[i].currentFrame;

        // Renderizar la textura del enemigo
        SDL_RenderCopy(renderer, enemyImages[textureIndex], NULL, &enemigoRect);
    }
}

int colisionConParedesEnemigo(Enemigo* enemigo, int nuevaX, int nuevaY)
{
    int celdaIzquierda = nuevaX / TAMANO_CELDA;
    int celdaDerecha = (nuevaX + enemigo->ancho - 1) / TAMANO_CELDA;
    int celdaArriba = nuevaY / TAMANO_CELDA;
    int celdaAbajo = (nuevaY + enemigo->alto - 1) / TAMANO_CELDA;

    for (int i = celdaArriba; i <= celdaAbajo; ++i)
    {
        for (int j = celdaIzquierda; j <= celdaDerecha; ++j)
        {
            if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS)
            {
                if (matriz[i][j] == 1)
                {
                    return 1;  // Hay colisión
                }
            }
        }
    }

    return 0;  // No hay colisión
}

void moverEnemigos(Enemigo* enemigos, int numEnemigos, Jugador* jugador)
{
    for (int i = 0; i < numEnemigos; ++i)
    {
        // Calcular la dirección hacia el jugador
        float direccionX = jugador->x - enemigos[i].x;
        float direccionY = jugador->y - enemigos[i].y;

        // Normalizar la dirección
        float magnitud = sqrt(direccionX * direccionX + direccionY * direccionY);
        if (magnitud != 0)
        {
            direccionX /= magnitud;
            direccionY /= magnitud;
        }

        // Mover el enemigo en la dirección normalizada
        float nuevaX = enemigos[i].x + direccionX * enemigos[i].abiadura;
        float nuevaY = enemigos[i].y + direccionY * enemigos[i].abiadura;

        // Verificar colisiones con paredes
        if (!colisionConParedesEnemigo(&enemigos[i], nuevaX, nuevaY))
        {
            // Verificar colisiones con otros enemigos
            int colisionEnemigo = 0;
            for (int j = 0; j < numEnemigos; ++j)
            {
                if (i != j)
                {
                    if (nuevaX < enemigos[j].x + enemigos[j].ancho &&
                        nuevaX + enemigos[i].ancho > enemigos[j].x &&
                        nuevaY < enemigos[j].y + enemigos[j].alto &&
                        nuevaY + enemigos[i].alto > enemigos[j].y)
                    {
                        colisionEnemigo = 1;  // Hay colisión con otro enemigo
                        break;
                    }
                }
            }

            // Si no hay colisiones, actualizar la posición del enemigo
            if (!colisionEnemigo)
            {
                enemigos[i].x = nuevaX;
                enemigos[i].y = nuevaY;
            }
        }

        // Actualizar la animación y el fotograma
        enemigos[i].currentFrame = (SDL_GetTicks() / 100) % NUM_ENEMY_FRAMES_PER_ANIMATION;

        // Elegir la animación según la dirección (arriba, abajo, izquierda, derecha)
        if (fabs(direccionX) > fabs(direccionY))
        {
            if (direccionX > 0)
            {
                enemigos[i].currentAnimation = 3; // Derecha
            }
            else
            {
                enemigos[i].currentAnimation = 2; // Izquierda
            }
        }
        else
        {
            if (direccionY > 0)
            {
                enemigos[i].currentAnimation = 1; // Abajo
            }
            else
            {
                enemigos[i].currentAnimation = 0; // Arriba
            }
        }
    }
}

int colisionConEnemigos(Jugador* jugador, Enemigo* enemigos, int numEnemigos)
{
    for (int i = 0; i < numEnemigos; ++i)
    {
        if (jugador->x < enemigos[i].x + enemigos[i].ancho &&
            jugador->x + jugador->ancho > enemigos[i].x &&
            jugador->y < enemigos[i].y + enemigos[i].alto &&
            jugador->y + jugador->alto > enemigos[i].y)
        {
            return 1;  // Hay colisión con un enemigo
        }
    }

    return 0;  // No hay colisión con enemigos
}

void actualizarProyectiles(Jugador* jugador)
{
    for (int i = 0; i < MAX_PROYECTILES; ++i)
    {
        if (proyectiles[i].activo)
        {
            // Actualizar la posición del proyectil
            proyectiles[i].x += proyectiles[i].direccionX * proyectiles[i].abiadura;
            proyectiles[i].y += proyectiles[i].direccionY * proyectiles[i].abiadura;

            // Calcular la distancia entre la posición actual del proyectil y la posición del jugador
            float distanciaRecorrida = sqrt(pow(proyectiles[i].x - jugador->x, 2) + pow(proyectiles[i].y - jugador->y, 2));

            // Desactivar el proyectil si sale de la pantalla o alcanza la distancia máxima
            if (proyectiles[i].x < 0 || proyectiles[i].x >= MAPA_WIDTH ||
                proyectiles[i].y < 0 || proyectiles[i].y >= MAPA_HEIGHT ||
                distanciaRecorrida >= DISTANCIA_MAXIMA_PROYECTIL)
            {
                proyectiles[i].activo = 0;
            }
        }
    }
}

void dibujarProyectiles(int camaraX, int camaraY)
{
    for (int i = 0; i < MAX_PROYECTILES; ++i)
    {
        if (proyectiles[i].activo)
        {
            // Obtener el índice de la textura de la bala basado en la animación actual
            int textureIndex = proyectiles[i].currentAnimation;

            // Dibujar la bala
            SDL_Rect proyectilRect = { (int)proyectiles[i].x - camaraX, (int)proyectiles[i].y - camaraY, 8, 8 };
            SDL_RenderCopy(renderer, projectileImages[textureIndex], NULL, &proyectilRect);
        }
    }
}

void dispararProyectil(Jugador* jugador, float direccionX, float direccionY)
{
    // Verificar si ya hay un proyectil activo
    for (int i = 0; i < MAX_PROYECTILES; ++i)
    {
        if (proyectiles[i].activo)
        {
            return;  // Ya hay un proyectil activo, no disparamos otro
        }
    }

    for (int i = 0; i < MAX_PROYECTILES; ++i)
    {
        if (!proyectiles[i].activo)
        {
            // Activar el proyectil y establecer su posición y dirección
            proyectiles[i].activo = 1;
            proyectiles[i].x = jugador->x + jugador->ancho / 2;
            proyectiles[i].y = jugador->y + jugador->alto / 2;
            proyectiles[i].direccionX = direccionX;
            proyectiles[i].direccionY = direccionY;
            proyectiles[i].abiadura = 5;
            proyectiles[i].kaltea = 1;

            break;  // Salir del bucle una vez que se ha disparado el proyectil
        }
    }
}

void colisionProyectilEnemigo(Enemigo* enemigos, int numEnemigos)
{
    for (int i = 0; i < MAX_PROYECTILES; ++i)
    {
        if (proyectiles[i].activo)
        {
            for (int j = 0; j < numEnemigos; ++j)
            {
                if (enemigos[j].activo)
                {
                    if (proyectiles[i].x < enemigos[j].x + enemigos[j].ancho &&
                        proyectiles[i].x + 5 > enemigos[j].x &&  // Considerando un ancho de 5 para la bala
                        proyectiles[i].y < enemigos[j].y + enemigos[j].alto &&
                        proyectiles[i].y + 5 > enemigos[j].y)    // Considerando un alto de 5 para la bala
                    {
                        // Colisión detectada, desactivar bala y enemigo
                        proyectiles[i].activo = 0;
                        enemigos[j].activo = 0;

                        // Reubicar al enemigo en una posición aleatoria
                        enemigos[j].x = rand() % 500;
                        enemigos[j].y = rand() % 500;
                        enemigos[j].activo = 1;  // Activar al enemigo nuevamente
                        puntos++;
                    }
                }
            }

            int celdaX = (proyectiles[i].x + 2) / TAMANO_CELDA; // Ajustar el valor de 2 según el tamaño de las balas
            int celdaY = (proyectiles[i].y + 2) / TAMANO_CELDA; // Ajustar el valor de 2 según el tamaño de las balas

            if (celdaX >= 0 && celdaX < COLUMNAS && celdaY >= 0 && celdaY < FILAS)
            {
                if (matriz[celdaY][celdaX] == 2)
                {
                    // Colisión detectada con el número 2, desactivar bala y restar una vida
                    proyectiles[i].activo = 0;
                    vidaspawn = vidaspawn - proyectiles[i].kaltea;

                    // Si las vidas llegan a 0, el juego termina
                    if (vidaspawn <= 0)
                    {
                        printf("Destruiste el Spawn felicidades\n");
                    }
                }
                else if (matriz[celdaY][celdaX] == 1)
                {
                    // Colisión detectada con el número 1, desactivar bala
                    proyectiles[i].activo = 0;
                }
            }
        }
    }
}

int initSDLMix()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }
    return 1;
}

int loadSounds()
{
    clickSound = Mix_LoadWAV("Musika/mejora.wav");  // Reemplaza con la ruta correcta
    if (clickSound == NULL)
    {
        printf("Failed to load click sound! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }
    return 1;
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == NULL)
    {
        fprintf("Failed to load image %s. SDL_Error: %s\n", filename, SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

Button createButton(int x, int y, int width, int height, SDL_Renderer* renderer, const char* filename)
{
    Button button;
    button.rect = (SDL_Rect){ x, y, width, height };
    button.texture = loadTexture(renderer, filename);
    return button;
}

void renderTexture(SDL_Texture* texture, int x, int y)
{
    SDL_Rect destRect = { x, y, 0, 0 };
    SDL_QueryTexture(texture, NULL, NULL, &destRect.w, &destRect.h);
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
}

void kargatu_kredituak()
{
    int quit = 0;

    // Cargar la pantalla de créditos
    SDL_DestroyTexture(Hasierako_menu);
    SDL_DestroyTexture(Hasiera_botoia);
    SDL_DestroyTexture(Kredituak_botoia);
    SDL_DestroyTexture(Settings_botoia);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Cargar la nueva imagen de fondo para los créditos
    SDL_Texture* CreditBackground = loadTexture(renderer, "images/Kredituak_Panela.png");
    renderTexture(CreditBackground, 80, 35);

    // Renderizar un botón de retroceso al menú
    SDL_Texture* BackButton = loadTexture(renderer, "images/Exit_Button.png");
    renderTexture(BackButton, 1400, 825);

    SDL_RenderPresent(renderer);

    // Bucle para la pantalla de créditos
    int creditScreenQuit = 0;
    while (!creditScreenQuit)
    {
        SDL_Event creditEvent;
        while (SDL_PollEvent(&creditEvent))
        {
            if (creditEvent.type == SDL_QUIT)
            {
                creditScreenQuit = 1;
                quit = 1;  // Salir de ambos bucles si se cierra la ventana
            }
            else if (creditEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                int creditMouseX = 0, creditMouseY = 0;
                SDL_GetMouseState(&creditMouseX, &creditMouseY);

                // Verificar clic en el botón de retroceso
                if (creditMouseX >= 1400 && creditMouseX <= 1599 && creditMouseY >= 825 && creditMouseY <= 923)
                {
                    creditScreenQuit = 1;  // Salir del bucle de la pantalla de créditos
                }
            }
        }
    }

    // Liberar recursos
    SDL_DestroyTexture(CreditBackground);
    SDL_DestroyTexture(BackButton);

    // Volver a cargar el menú principal
    Hasierako_menu = loadTexture(renderer, "images/Menu(PBL).png");
    Hasiera_botoia = loadTexture(renderer, "images/Jokatu.png");
    Kredituak_botoia = loadTexture(renderer, "images/Kedituak.png");
    Settings_botoia = loadTexture(renderer, "images/Settings.png");
}

void kargatu_kontrolak()
{
    int quit = 0;

    // Cargar la pantalla de créditos
    SDL_DestroyTexture(Hasierako_menu);
    SDL_DestroyTexture(Hasiera_botoia);
    SDL_DestroyTexture(Kredituak_botoia);
    SDL_DestroyTexture(Settings_botoia);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Cargar la nueva imagen de fondo para los créditos
    SDL_Texture* CreditBackground = loadTexture(renderer, "images/image.png");
    renderTexture(CreditBackground, 80, 35);

    // Renderizar un botón de retroceso al menú
    SDL_Texture* BackButton = loadTexture(renderer, "images/Exit_Button.png");
    renderTexture(BackButton, 1400, 825);

    SDL_RenderPresent(renderer);

    // Bucle para la pantalla de créditos
    int creditScreenQuit = 0;
    while (!creditScreenQuit)
    {
        SDL_Event creditEvent;
        while (SDL_PollEvent(&creditEvent))
        {
            if (creditEvent.type == SDL_QUIT)
            {
                creditScreenQuit = 1;
                quit = 1;  // Salir de ambos bucles si se cierra la ventana
            }
            else if (creditEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                int creditMouseX = 0, creditMouseY = 0;
                SDL_GetMouseState(&creditMouseX, &creditMouseY);

                // Verificar clic en el botón de retroceso
                if (creditMouseX >= 1400 && creditMouseX <= 1599 && creditMouseY >= 825 && creditMouseY <= 923)
                {
                    creditScreenQuit = 1;  // Salir del bucle de la pantalla de créditos
                }
            }
        }
    }

    // Liberar recursos
    SDL_DestroyTexture(CreditBackground);
    SDL_DestroyTexture(BackButton);

    // Volver a cargar el menú principal
    Hasierako_menu = loadTexture(renderer, "images/Menu(PBL).png");
    Hasiera_botoia = loadTexture(renderer, "images/Jokatu.png");
    Kredituak_botoia = loadTexture(renderer, "images/Kedituak.png");
    Settings_botoia = loadTexture(renderer, "images/Settings.png");
}

void mapa_pantailaratu()
{
    cargarTexturas();
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_PlayMusic(musicaDeFondo, -1);
    inicializarMatriz();
    matrizea_bueltatu();

    Jugador jugador = { 500.0f, 3000.0f, TAMANO_CELDA, TAMANO_CELDA, 3,2 }; // 3 vidas iniciales
    // Enemigos
    int numEnemigos = NUM_ENEMIGOS;
    Enemigo enemigos[NUM_ENEMIGOS];
    for (int i = 0; i < NUM_ENEMIGOS; ++i)
    {
        int spawnX, spawnY;
        do
        {
            spawnX = rand() % COLUMNAS;
            spawnY = rand() % FILAS;
        } while (matriz[spawnY][spawnX] != 0);

        enemigos[i].x = spawnX * TAMANO_CELDA;
        enemigos[i].y = spawnY * TAMANO_CELDA;
        enemigos[i].ancho = TAMANO_CELDA;
        enemigos[i].alto = TAMANO_CELDA;
        enemigos[i].currentAnimation = 0; // Establecer animación inicial (puedes ajustar según tus necesidades)
        enemigos[i].currentFrame = 0;     // Establecer frame inicial
        enemigos[i].activo = 1;            // Activar enemigo
        enemigos[i].abiadura = 1;
    }

    int kont_velocidad = 0, kont_balas = 0, kont_vida = 0;

    const char* imagen1_pos[5] =
    {
        "images/Velocidad1.png",
        "images/Velocidad2.png",
        "images/Velocidad3.png",
        "images/Velocidad4.png",
        "images/Velocidad5.png"
    };

    const char* imagen2_pos[5] =
    {
        "images/Balas1.png",
        "images/Balas2.png",
        "images/Balas3.png",
        "images/Balas4.png",
        "images/Balas5.png"
    };

    const char* imagen3_pos[5] =
    {
        "images/Vida1.png",
        "images/Vida2.png",
        "images/Vida3.png",
        "images/Vida4.png",
        "images/Vida5.png"
    };

    if (!initSDLMix())
    {
        SDL_Quit();
        return -1;
    }

    SDL_Window* secondaryWindow = SDL_CreateWindow("Ventana Secundaria", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_HIDDEN);
    SDL_Renderer* secondaryRenderer = SDL_CreateRenderer(secondaryWindow, -1, SDL_RENDERER_ACCELERATED);

    if (secondaryWindow == NULL)
    {
        fprintf("Failed to create window. SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    if (secondaryRenderer == NULL)
    {
        fprintf("Failed to create renderer. SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(secondaryWindow);
        SDL_Quit();
        return -1;
    }

    if (!loadSounds())
    {
        Mix_Quit();
        SDL_Quit();
        return -1;
    }

    Button buttons[3];
    buttons[0] = createButton(500, 110, 280, 71, secondaryRenderer, "images/Upgrade1.png");
    buttons[1] = createButton(500, 260, 280, 71, secondaryRenderer, "images/Upgrade2.png");
    buttons[2] = createButton(500, 410, 280, 71, secondaryRenderer, "images/Upgrade3.png");

    if (buttons[0].texture == NULL || buttons[1].texture == NULL || buttons[2].texture == NULL)
    {
        SDL_DestroyRenderer(secondaryRenderer);
        SDL_DestroyWindow(secondaryWindow);
        SDL_Quit();
        return -1;
    }

    // Definir las posiciones de las imágenes
    SDL_Rect destRect1 = { 100, 100, 336, 112 };
    SDL_Rect destRect2 = { 100, 250, 336, 112 };
    SDL_Rect destRect3 = { 100, 400, 336, 112 };

    // Cargar texturas desde las rutas especificadas
    SDL_Texture* texture1 = loadTexture(secondaryRenderer, imagen1_pos[kont_velocidad]);
    SDL_Texture* texture2 = loadTexture(secondaryRenderer, imagen2_pos[kont_balas]);
    SDL_Texture* texture3 = loadTexture(secondaryRenderer, imagen3_pos[kont_vida]);

    if (texture1 == NULL || texture2 == NULL || texture3 == NULL)
    {
        // Manejar el fallo al cargar las texturas
        SDL_DestroyTexture(texture1);
        SDL_DestroyTexture(texture2);
        SDL_DestroyTexture(texture3);
        SDL_DestroyRenderer(secondaryRenderer);
        SDL_DestroyWindow(secondaryWindow);
        SDL_Quit();
        return -1;
    }
    // Bucle principal
    int salir = 0;
    SDL_Event evento;
    int window = 0;
    int spawn = 0;
    int vida = 0;

    // Bucle principal
    while (!salir)
    {
        if ((vidaspawn <= 0) && (spawn == 0))
        {
            cargarTexturas();
            matrizea_bueltatu();
            spawn++;
            Mix_HaltMusic();
        }
        // Actualizar la posición de la cámara basada en la posición del jugador
        int camaraX = (int)jugador.x - WINDOW_WIDTH / 2 + jugador.ancho / 2;
        int camaraY = (int)jugador.y - WINDOW_HEIGHT / 2 + jugador.alto / 2;

        // Manejar eventos
        while (SDL_PollEvent(&evento) != 0)
        {
            if (evento.type == SDL_QUIT)
            {
                // Se detectó el evento de cierre de la ventana
                salir = 1;
            }
            else if (evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_e)
            {
                // Mostrar/ocultar la ventana secundaria al presionar la tecla "E"
                if (SDL_GetWindowFlags(secondaryWindow) & SDL_WINDOW_HIDDEN)
                {
                    SDL_ShowWindow(secondaryWindow);
                    window = 1;
                }
                else
                {
                    SDL_HideWindow(secondaryWindow);
                    window = 0;
                }
            }
        }
        const Uint8* estadoTeclado = SDL_GetKeyboardState(NULL);

        // Guardar la posición anterior del jugador
        float xAnterior = jugador.x;
        float yAnterior = jugador.y;

        // Movimiento del jugador
        if (estadoTeclado[SDL_SCANCODE_A])
        {
            jugador.x -= jugador.abiadura;
        }
        if (estadoTeclado[SDL_SCANCODE_D])
        {
            jugador.x += jugador.abiadura;
        }
        if (estadoTeclado[SDL_SCANCODE_W])
        {
            jugador.y -= jugador.abiadura;
        }
        if (estadoTeclado[SDL_SCANCODE_S])
        {
            jugador.y += jugador.abiadura;
        }

        // Verificar colisiones y revertir movimiento si es necesario
        if (colisionConParedes(&jugador, jugador.x, jugador.y) ||
            colisionConParedes(&jugador, jugador.x + jugador.ancho - 1, jugador.y) ||
            colisionConParedes(&jugador, jugador.x, jugador.y + jugador.alto - 1) ||
            colisionConParedes(&jugador, jugador.x + jugador.ancho - 1, jugador.y + jugador.alto - 1))
        {
            // Hubo una colisión, revertir movimiento
            jugador.x = xAnterior;
            jugador.y = yAnterior;
        }

        if (estadoTeclado[SDL_SCANCODE_I])
        {
            dispararProyectil(&jugador, 0, -1);  // Disparar hacia arriba
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }
        if (estadoTeclado[SDL_SCANCODE_J])
        {
            dispararProyectil(&jugador, -1, 0);  // Disparar hacia la izquierda
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }
        if (estadoTeclado[SDL_SCANCODE_K])
        {
            dispararProyectil(&jugador, 0, 1);  // Disparar hacia abajo
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }
        if (estadoTeclado[SDL_SCANCODE_L])
        {
            dispararProyectil(&jugador, 1, 0);  // Disparar hacia la derecha
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }
        if (estadoTeclado[SDL_SCANCODE_O])
        {
            dispararProyectil(&jugador, 1, -1);  // Disparar hacia la derecha
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }
        if (estadoTeclado[SDL_SCANCODE_PERIOD])
        {
            dispararProyectil(&jugador, 1, 1);  // Disparar hacia la derecha
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }
        if (estadoTeclado[SDL_SCANCODE_U])
        {
            dispararProyectil(&jugador, -1, -1);  // Disparar hacia la derecha
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }
        if (estadoTeclado[SDL_SCANCODE_M])
        {
            dispararProyectil(&jugador, -1, 1);  // Disparar hacia la derecha
            Mix_PlayChannel(-1, sonidoDisparo, 0);
        }

        if (vidaspawn > 0)
        {
            moverEnemigos(enemigos, numEnemigos, &jugador);
        }

        // Limpiar el renderizador
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibujar elementos en la ventana
        dibujarMapaEnVentana(camaraX, camaraY);
        dibujarMatriz(camaraX, camaraY);

        if (vidaspawn > 0)
        {
            // Verificar colisión con enemigos
            if (colisionConEnemigos(&jugador, enemigos, numEnemigos))
            {
                jugador.biziak--; // Reducir una vida
                printf("Harrapatuta! Bizi bat gutxiago daukazu, falta zaizkizunak: %d\n", jugador.biziak);

                // Resetea la posición del jugador o realiza cualquier otra acción necesaria
                jugador.x = 500.0f;
                jugador.y = 3000.0f;

            }
        }
        if (vidaspawn <= 0)
        {
            if (estadoTeclado[SDL_SCANCODE_RETURN])
            {
                dibujarPantallaZorionak();
                SDL_Delay(5000);
                if (estadoTeclado[SDL_SCANCODE_RETURN])
                {
                    SDL_Quit();
                }
            }

        }
        if ((jugador.biziak <= 0) && (vida == 0))
        {
            dibujarPantallaGameOver();
            Mix_HaltMusic();

            // Esperar a que se presione Enter para cerrar el juego
            SDL_Event event;
            int esperarEnter = 1;
            while (esperarEnter)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
                    {
                        salir = 1;
                        SDL_Quit();
                    }
                }
                vida++;
            }
        }
        // Definir la animación actual y el frame actual
        int currentAnimation = 0;
        int currentFrame = 0;

        // Ajustar la animación y el frame en función del movimiento
        if (estadoTeclado[SDL_SCANCODE_A] || estadoTeclado[SDL_SCANCODE_D] ||
            estadoTeclado[SDL_SCANCODE_W] || estadoTeclado[SDL_SCANCODE_S])
        {
            if (estadoTeclado[SDL_SCANCODE_A])
            {
                currentAnimation = 2;
            }
            if (estadoTeclado[SDL_SCANCODE_D])
            {
                currentAnimation = 3;
            }
            if (estadoTeclado[SDL_SCANCODE_W])
            {
                currentAnimation = 0;
            }
            if (estadoTeclado[SDL_SCANCODE_S])
            {
                currentAnimation = 1;
            }

            // Ciclo de animación
            currentFrame = (SDL_GetTicks() / 100) % NUM_FRAMES_PER_ANIMATION;
        }
        char text[50];
        sprintf(text, "Biziak: %d | Txanponak: %d", jugador.biziak, puntos);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);

        if (textSurface == NULL)
        {
            printf("No se pudo crear la superficie de texto: %s\n", TTF_GetError());
            // Manejar el error
        }
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture == NULL)
        {
            printf("No se pudo crear la textura de texto: %s\n", SDL_GetError());
            // Manejar el error
        }

        // Definir la posición y tamaño del marcador
        SDL_Rect scoreRect = { 10, 10, textSurface->w, textSurface->h };

        // Dibujar el marcador en la ventana
        SDL_RenderCopy(renderer, textTexture, NULL, &scoreRect);

        SDL_FreeSurface(textSurface); // No necesitas la superficie una vez que tienes la textura
        if (window == 1)
        {
            if ((estadoTeclado[SDL_SCANCODE_1]) && (puntos >= 10) && (kont_velocidad < 4))
            {
                SDL_Delay(200);
                kont_velocidad++;
                jugador.abiadura;
                puntos = puntos - 10;
                SDL_DestroyTexture(texture1);
                texture1 = loadTexture(secondaryRenderer, imagen1_pos[kont_velocidad]);
                Mix_PlayChannel(-1, clickSound, 0);

            }
            if ((estadoTeclado[SDL_SCANCODE_2]) && (puntos >= 10) && (kont_balas < 4))
            {
                SDL_Delay(200);
                kont_balas++;
                for (int i = 0; i < MAX_PROYECTILES; i++)
                {
                    proyectiles[i].abiadura++;
                    proyectiles[i].kaltea++;
                }
                puntos = puntos - 10;
                SDL_DestroyTexture(texture2);
                texture2 = loadTexture(secondaryRenderer, imagen2_pos[kont_balas]);
                Mix_PlayChannel(-1, clickSound, 0);

            }
            if ((estadoTeclado[SDL_SCANCODE_3]) && (puntos >= 10) && (kont_vida < 4))
            {

                SDL_Delay(200);
                kont_vida++;
                jugador.biziak++;
                puntos = puntos - 10;
                SDL_DestroyTexture(texture3);
                texture3 = loadTexture(secondaryRenderer, imagen3_pos[kont_vida]);
                Mix_PlayChannel(-1, clickSound, 0);

            }


            if (texture1 == NULL || texture2 == NULL || texture3 == NULL)
            {
                // Manejar el fallo al cargar las texturas
                SDL_DestroyTexture(texture1);
                SDL_DestroyTexture(texture2);
                SDL_DestroyTexture(texture3);
                SDL_DestroyRenderer(secondaryRenderer);
                SDL_DestroyWindow(secondaryWindow);
                SDL_Quit();
                return -1;
            }
        }

        if (vidaspawn > 0)
        {
            dibujarEnemigos(enemigos, numEnemigos, camaraX, camaraY);
        }
        actualizarProyectiles(&jugador);

        dibujarProyectiles(camaraX, camaraY);

        // Verificar colisión entre proyectiles y enemigos
        colisionProyectilEnemigo(enemigos, numEnemigos);



        // Dibujar al jugador
        dibujarJugador(&jugador, camaraX, camaraY, currentAnimation, currentFrame);

        // Actualizar la ventana
        SDL_RenderPresent(renderer);

        // Limpiar el renderer
        SDL_SetRenderDrawColor(secondaryRenderer, 20, 15, 40, 0);
        SDL_RenderClear(secondaryRenderer);

        // Renderizar los botones
        for (int i = 0; i < 3; ++i)
        {
            SDL_RenderCopy(secondaryRenderer, buttons[i].texture, NULL, &buttons[i].rect);
        }
        // Dibujar las imágenes en las posiciones especificadas
        SDL_RenderCopy(secondaryRenderer, texture1, NULL, &destRect1);
        SDL_RenderCopy(secondaryRenderer, texture2, NULL, &destRect2);
        SDL_RenderCopy(secondaryRenderer, texture3, NULL, &destRect3);
        SDL_DestroyTexture(textTexture); // Limpia la textura después de usarla
        // Presentar el renderer
        SDL_RenderPresent(secondaryRenderer);
        SDL_Delay(10);

    }
    Mix_FreeChunk(clickSound);
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyRenderer(secondaryRenderer);
    SDL_DestroyWindow(secondaryWindow);
    Mix_FreeChunk(sonidoDisparo);
    Mix_FreeMusic(musicaDeFondo);
    Mix_Quit();
    SDL_Quit();
}