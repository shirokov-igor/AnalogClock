#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include <math.h>

const int WIDTH = 1280, HEIGHT = 1024;

const int SECONDS_IN_DAY = 86400, 
          SECONDS_IN_HOUR = 3600, 
          SECONDS_IN_MINUTE = 60,
          HOUR_OFFSET = 3;

void fill_circle(int xCenter, int yCenter, int radius, SDL_Renderer* renderer)
{
    int x = 0;
    int y = radius;
    int delta = 3 - 2*radius;
    while(x<=y)
    {
        SDL_RenderDrawLine(renderer, xCenter - x , yCenter - y, xCenter + x, yCenter - y);
        SDL_RenderDrawLine(renderer, xCenter - y , yCenter - x, xCenter + y, yCenter - x);
        SDL_RenderDrawLine(renderer, xCenter - y , yCenter + x, xCenter + y, yCenter + x);
        SDL_RenderDrawLine(renderer, xCenter - x , yCenter + y, xCenter + x, yCenter + y);
        if (delta < 0)
        {
            delta += 4*x+6;
        }
        else
        {
            delta += 4*(x-y)+10;
            y--;
        }
        x++;
    }
}

void draw_clock(int xCenter, int yCenter,
                int xHr, int yHr,
                int xMin,int yMin,
                int xSec,int ySec,
                int radius, int dotradius,
                int smdotpos, int smdotrad,
                int border_w, 
                SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0x82,0xE5,0xD1,0xFF);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xC0,0xC0,0xC0,0xFF);
    fill_circle(xCenter, yCenter, radius, renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0xFF);
    fill_circle(xCenter, yCenter, radius-border_w, renderer);
    
    SDL_SetRenderDrawColor(renderer, 0xC0,0xC0,0xC0,0xFF);
    int xOff = 2;
    int yOff = 2;
    for(int i=1; i<13; i++)
    {
        fill_circle(xCenter + round(smdotpos * cos((M_PI*i)/6.0)-(M_PI/2.0)+xOff), 
                    yCenter + round(smdotpos * sin((M_PI*i)/6.0)-(M_PI/2.0)+yOff), 
                    smdotrad, renderer);
    }

    SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x00,0xFF);
    SDL_RenderDrawLine(renderer, xCenter, yCenter, xHr, yHr);
    SDL_RenderDrawLine(renderer, xCenter, yCenter, xMin, yMin);

    SDL_SetRenderDrawColor(renderer, 0xFF,0x00,0x00,0xFF);
    SDL_RenderDrawLine(renderer, xCenter, yCenter, xSec, ySec);

    SDL_SetRenderDrawColor(renderer, 0xC0,0xC0,0xC0,0xFF);
    fill_circle(xCenter, yCenter, dotradius, renderer);
}

void sec_to_arrows(int* x, int* y, int radius, int xCenter, int yCenter, int sec, int ms) 
{
    int millis = sec * 1000 + ms;
    double secRadian = ((360.0 / 60000.0) * millis * M_PI)/180.0;
    *x = xCenter + (int) round((radius * cos(secRadian - (M_PI / 2.0))));
    *y = yCenter + (int) round((radius * sin(secRadian - (M_PI / 2.0))));

}

void min_to_arrows(int* x, int* y, int radius, int xCenter, int yCenter, int min, int sec) 
{
    int seconds = min * 60 + sec;
    double minRadian = ((360.0 / 3600.0) * seconds * M_PI)/180.0;
    *x = xCenter + (int) round((radius * cos(minRadian - (M_PI / 2.0))));
    *y = yCenter + (int) round((radius * sin(minRadian - (M_PI / 2.0))));
}

void hour_to_arrows(int* x, int* y, int radius, int xCenter, int yCenter, int hr, int min) 
{
    if (hr > 12)
    {
        hr -= 12;
    }
    int minutes = hr*60 + min;
    double hrRadian = ((360.0 / 720.0) * minutes * M_PI)/180.0;
    *x = xCenter + (int) round((radius * cos(hrRadian - (M_PI / 2.0))));
    *y = yCenter + (int) round((radius * sin(hrRadian - (M_PI / 2.0))));
}

void get_time(struct timeval* now, int* hr, int* min, int* sec, int* ms)
{
    long curSecInDay = now->tv_sec % SECONDS_IN_DAY;
    *hr = (curSecInDay / SECONDS_IN_HOUR) + HOUR_OFFSET;
    *min = (curSecInDay % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    *sec = (curSecInDay % SECONDS_IN_HOUR) % SECONDS_IN_MINUTE;
    *ms = now->tv_usec/1000;
}

int main(int argc, char** argv)
{
    struct timeval nowTime;
    int mvstep = 10;
    int rszstep = 10;
    bool mvflag[4] = {0,0,0,0};

    int nowSec = 0;
    int nowMin = 0;
    int nowHour = 0;
    int nowMs = 0;

    int xClock = WIDTH/2;
    int yClock = HEIGHT/2;
    int radClock = 0;
    int maxRadClock = 0;
    int minRadClock = 0;
    if(HEIGHT < WIDTH)
    {
        radClock = HEIGHT/4;
        maxRadClock = HEIGHT/2 - (2*rszstep);
        minRadClock = HEIGHT/4;
    }
    else
    {
        radClock = WIDTH/4;
        maxRadClock = WIDTH/2 - (2*rszstep);
        minRadClock = WIDTH/4;
    }
    
    int bordClock = 4;
    int dotRadClock = 5;

    int xSecond = 0;
    int ySecond = 0;
    int radSecond = radClock - bordClock - 10;

    int xMinute = 0;
    int yMinute = 0;
    int radMinute = radClock - bordClock - 15;

    int xHour = 0;
    int yHour = 0;
    int radHour = (radClock - bordClock)/2;

    int smdotpos = radClock - bordClock - 13;
    int smdotrad = 3;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Часы", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL)
    {
        printf("ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *screenRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (screenRenderer == NULL)
    {
        printf("ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetRenderDrawColor(screenRenderer, 0x82,0xE5,0xD1,0xFF);
    SDL_RenderClear(screenRenderer);
    
    printf("Initialisation successful.\n");

    SDL_Event event;
    while(1)
    {
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT)
        {
            break;
        }
        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                        mvflag[0] = 1;
                        break;
                    case SDLK_DOWN:
                        mvflag[1] = 1;
                        break;
                    case SDLK_LEFT:
                        mvflag[2] = 1;
                        break;
                    case SDLK_RIGHT:
                        mvflag[3] = 1;
                        break;
                    case SDLK_KP_PLUS:
                        if (
                            ((radClock + rszstep) < maxRadClock)        &&
                            ((yClock - radClock - mvstep)>0)            && 
                            ((HEIGHT - (yClock + radClock + mvstep))>0) &&
                            ((xClock - radClock - mvstep)>0)            && 
                            ((WIDTH - (xClock + radClock + mvstep))>0)
                           )
                           {
                            radClock  += rszstep;
                            radHour   += rszstep;
                            radMinute += rszstep;
                            radSecond += rszstep;
                            smdotpos  += rszstep;
                           }
                        break;
                    case SDLK_KP_MINUS:
                        if ((radClock - rszstep) >= minRadClock)
                        {
                            radClock  -= rszstep;
                            radHour   -= rszstep;
                            radMinute -= rszstep;
                            radSecond -= rszstep;
                            smdotpos  -= rszstep;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                        mvflag[0] = 0;
                        break;
                    case SDLK_DOWN:
                        mvflag[1] = 0;
                        break;
                    case SDLK_LEFT:
                        mvflag[2] = 0;
                        break;
                    case SDLK_RIGHT:
                        mvflag[3] = 0;
                        break;
                    default:
                        break;
                }
                break;
        }
        
        if(mvflag[0] && ((yClock - radClock - mvstep)>0)){yClock-=mvstep;}
        if(mvflag[1] && ((HEIGHT - (yClock + radClock + mvstep))>0)){yClock+=mvstep;}
        if(mvflag[2] && ((xClock - radClock - mvstep)>0)){xClock-=mvstep;}
        if(mvflag[3] && ((WIDTH - (xClock + radClock + mvstep))>0)){xClock+=mvstep;}

        gettimeofday(&nowTime, NULL);
        get_time(&nowTime, &nowHour, &nowMin, &nowSec, &nowMs);
        sec_to_arrows(&xSecond, &ySecond, radSecond, xClock, yClock, nowSec, nowMs);
        min_to_arrows(&xMinute, &yMinute, radMinute, xClock, yClock, nowMin, nowSec);
        hour_to_arrows(&xHour, &yHour, radHour, xClock, yClock, nowHour, nowMin);
        draw_clock(xClock, yClock, 
                   xHour,   yHour,
                   xMinute, yMinute,
                   xSecond, ySecond,
                   radClock, dotRadClock,
                   smdotpos, smdotrad,
                   bordClock,
                   screenRenderer);
        SDL_RenderPresent(screenRenderer);
    }
    printf("Quitting...\n");
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(screenRenderer);
    SDL_Quit();
    return EXIT_SUCCESS;
}

