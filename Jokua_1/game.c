#define _CRT_SECURE_NO_WARNINGS
#include "funciones.h"

SDL_Renderer* renderer;
SDL_Texture* Kredituak_Panela;
SDL_Texture* Hasiera_botoia;
SDL_Texture* Settings_botoia;
SDL_Texture* Kredituak_botoia;
SDL_Texture* Hasierako_menu;

int  main()
{
    hasi();

    Hasierako_menu = loadTexture(renderer, "images/Menu(PBL).png");
    Hasiera_botoia = loadTexture(renderer, "images/Jokatu.png");
    Kredituak_botoia = loadTexture(renderer, "images/Kedituak.png");
    Settings_botoia = loadTexture(renderer, "images/Settings.png");
    Kredituak_Panela = loadTexture(renderer, "images/Kredituak_Panela.png");

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int quit = 0;
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                // Se detectó el evento de cierre de la ventana
                quit = 1;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX = 0, mouseY = 0;


                SDL_GetMouseState(&mouseX, &mouseY);
                printf("%d %d", mouseX, mouseY);
                if (mouseX >= 770 && mouseX <= 1022 && mouseY >= 750 && mouseY <= 830)
                {
                    SDL_DestroyTexture(Hasierako_menu);
                    SDL_DestroyTexture(Hasiera_botoia);
                    SDL_DestroyTexture(Kredituak_botoia);
                    SDL_DestroyTexture(Settings_botoia);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                    mapa_pantailaratu();
                }
                else if (mouseX >= 400 && mouseX <= 639 && mouseY >= 750 && mouseY <= 827)
                {
                    kargatu_kredituak();
                }
                else if (mouseX >= 1050 && mouseX >= 1200 && mouseY >= 750 && mouseY <= 827)
                {

                    kargatu_kontrolak();
                }
            }
        }
        SDL_RenderClear(renderer);

        renderTexture(Hasierako_menu, 0, 25);
        renderTexture(Hasiera_botoia, 770, 750);
        renderTexture(Kredituak_botoia, 400, 750);
        renderTexture(Settings_botoia, 1140, 750);

        SDL_RenderPresent(renderer);
    }

    itxi();

    return 0;
}