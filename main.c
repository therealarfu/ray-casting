#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define DEADZONE 0.1

int main(int argc, char *argv[])
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    SDL_Window *window = SDL_CreateWindow(
        "Ray Casting",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_bool running = SDL_TRUE;
    SDL_Event event;

    int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,1},
        {1,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,2,0,3,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,3,3,3,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,3,3,3,0,0,1},
        {1,0,0,0,4,4,0,4,4,0,0,0,0,0,0,0,2,0,3,0,0,0,0,1},
        {1,0,0,0,4,0,0,0,4,0,0,0,0,0,0,0,2,0,3,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,0,0,0,0,1},
        {1,0,0,0,4,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,4,4,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Player state
    float speed = 3.0f;
    float rotspeed = 2.0f; // degrees per second
    double posX = 2.0f;
    double posY = 2.0f;
    double dirX = -1.0f;
    double dirY = 0.0f;
    double planeX = 0.0f;
    double planeY = 0.66f;

    // Main loop
    uint32_t t0 = SDL_GetTicks(); // Initializing Delta Time

    // Controller
    SDL_GameController *controller = NULL;
    if (SDL_NumJoysticks() > 0)
    {
        controller = SDL_GameControllerOpen(0);
    }
    double moveX = 0.0f;
    double moveY = 0.0f;
    double rotX = 0.0f;
    while (running)
    {
        uint32_t t1 = SDL_GetTicks();
        float dt = (t1 - t0) / 1000.0f;
        if (dt > 0.05f)
        {
            dt = 0.05f;
        }
        t0 = t1;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) 
            {
                running = SDL_FALSE;
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        running = SDL_FALSE;
                        break;
                }
                
            }
        }

        SDL_RenderClear(renderer);
        // DDA
        for(int x = 0; x < SCREEN_WIDTH; x++)
        {
            double cameraX = 2 * x / (float) SCREEN_WIDTH - 1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = (int) posX;
            int mapY = (int) posY;

            double sideDistX;
            double sideDistY;
            double deltaDistX;
            double deltaDistY;
            double perpWallDist;

            int stepX;
            int stepY;
            int hit = 0;
            int side;

            if (rayDirX == 0)
            {
                deltaDistX = 1e30; // Avoid division by zero
                stepX = 0;
            }
            else
            {
                deltaDistX = fabs(1 / rayDirX);
                if (rayDirX < 0)
                {
                    stepX = -1;
                    sideDistX = (posX - mapX) * deltaDistX;
                }
                else
                {
                    stepX = 1;
                    sideDistX = (mapX + 1.0 - posX) * deltaDistX;
                }
            }

            if (rayDirY == 0)
            {
                deltaDistY = 1e30; // Avoid division by zero
                stepY = 0;
            }
            else
            {
                deltaDistY = fabs(1 / rayDirY);
                if (rayDirY < 0)
                {
                    stepY = -1;
                    sideDistY = (posY - mapY) * deltaDistY;
                }
                else
                {
                    stepY = 1;
                    sideDistY = (mapY + 1.0 - posY) * deltaDistY;
                }
            }

            while (hit == 0)
            {
                if (sideDistX < sideDistY) // Hit the X side
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } 
                else // Hit the Y side
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                if (worldMap[mapX][mapY] > 0) // There is a wall at this position
                {
                    hit = 1;
                }
            }

            if (side == 0) // We use the perpendicular distance to avoid fish-eye effect
            {
                perpWallDist = sideDistX - deltaDistX;
            }
            else
            {
                perpWallDist = sideDistY - deltaDistY;
            }

            // Drawing the vertical stripe
            int lineHeight = (int) (SCREEN_HEIGHT / perpWallDist); // The height of the line
            int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
            if (drawStart < 0)
            {
                drawStart = 0;
            }
            int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
            if (drawEnd >= SCREEN_HEIGHT)
            {
                drawEnd = SCREEN_HEIGHT - 1;
            }

            SDL_Color color;
            switch(worldMap[mapX][mapY])
            {
                case 1: color = (SDL_Color){255, 0, 0, 255}; break; // Red
                case 2: color = (SDL_Color){0, 255, 0, 255}; break; // Green
                case 3: color = (SDL_Color){0, 0, 255, 255}; break; // Blue
                case 4: color = (SDL_Color){255, 255, 255, 255}; break; // White
                default: color = (SDL_Color){255, 255, 0, 255}; break; // Yellow
            }

            if (side == 1)
            {
                color.r = (uint8_t)(color.r * 0.5);
                color.g = (uint8_t)(color.g * 0.5); 
                color.b = (uint8_t)(color.b * 0.5);
            }

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);

        }

        int has_controller = controller != NULL;
        if (controller)
        {
            moveY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0;
            moveX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0;
            rotX  = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0;

            if (fabs(moveY) < DEADZONE) moveY = 0.0;
            if (fabs(moveX) < DEADZONE) moveX = 0.0;
            if (fabs(rotX)  < DEADZONE) rotX  = 0.0;
        }

        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        int up    = keystate[SDL_SCANCODE_W] || moveY < 0 || (has_controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP));
        int down  = keystate[SDL_SCANCODE_S] || moveY > 0 || (has_controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN));
        int left  = keystate[SDL_SCANCODE_A]  || moveX < 0 || (has_controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT));
        int right = keystate[SDL_SCANCODE_D] || moveX > 0 || (has_controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT));
        int turnLeft  = keystate[SDL_SCANCODE_LEFT] || rotX < 0 || (has_controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)); 
        int turnRight = keystate[SDL_SCANCODE_RIGHT] || rotX > 0 || (has_controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER));

        if (up)
        {
            if (worldMap[(int)(posX + dirX * speed * dt)][(int) posY] == 0)
            {
                posX += dirX * speed * dt;
            }
            if (worldMap[(int)(posX)][(int) (posY + dirY * speed * dt)] == 0)
            {
                posY += dirY * speed * dt;
            }
        }
        if (down)
        {
            if (worldMap[(int)(posX - dirX * speed * dt)][(int) posY] == 0)
            {
                posX -= dirX * speed * dt;
            }
            if (worldMap[(int)(posX)][(int) (posY - dirY * speed * dt)] == 0)
            {
                posY -= dirY * speed * dt;
            }
        }
        if (left)
        {
            if (worldMap[(int)(posX - planeX * speed * dt)][(int) posY] == 0)
            {
                posX -= planeX * speed * dt;
            }
            if (worldMap[(int)(posX)][(int) (posY - planeY * speed * dt)] == 0)
            {
                posY -= planeY * speed * dt;
            }
        }
        if (right)
        {
            if (worldMap[(int)(posX + planeX * speed * dt)][(int) posY] == 0)
            {
                posX += planeX * speed * dt;
            }
            if (worldMap[(int)(posX)][(int) (posY + planeY * speed * dt)] == 0)
            {
                posY += planeY * speed * dt;
            }
        }
        if (turnLeft)
        {
            // Rotate left -> for that we will use the rotation matrix to rotate the direction and plane vectors
            double oldDirX = dirX;
            dirX = dirX * cos(rotspeed * dt) - dirY * sin(rotspeed * dt);
            dirY = oldDirX * sin(rotspeed * dt) + dirY * cos(rotspeed * dt);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotspeed * dt) - planeY * sin(rotspeed * dt);
            planeY = oldPlaneX * sin(rotspeed * dt) + planeY * cos(rotspeed * dt);
        }
        if (turnRight)
        {
            // Rotate right
            double oldDirX = dirX;
            dirX = dirX * cos(-rotspeed * dt) - dirY * sin(-rotspeed * dt);
            dirY = oldDirX * sin(-rotspeed * dt) + dirY * cos(-rotspeed * dt);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotspeed * dt) - planeY * sin(-rotspeed * dt);
            planeY = oldPlaneX * sin(-rotspeed * dt) + planeY * cos(-rotspeed * dt);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    SDL_GameControllerClose(controller);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
