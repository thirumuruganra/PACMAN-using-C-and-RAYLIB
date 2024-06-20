#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "raylib.h"

#define MAZE_WIDTH 28
#define MAZE_HEIGHT 31
#define FRIGHTENED_TIME 9
#define SCATTER_TIME 6

typedef enum Direction {
    STOP,
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    int row;
    int col;
} Vector2Int;

typedef enum GameScreen { 
    DESCRIBE,
    DIFFICULTY,
    DIFF_SELECTION,
    LOADING,
    GAMEPLAY,
    REMAININGLIVES,
    GAMEOVER,
    GAMEWIN
} GameScreen;
GameScreen CurrentScreen = DESCRIBE;

int level = 0;
int hscore = 0;
int filescore = 0;
int lives = 3;
bool livesDecreased = false;
bool isDead = false;
int currentDeathFrame = 0;  

bool superPelletEaten = false;
int superPelletCount = 0;
int oldSuperPelletCount = 0;

float SPEED = 2.0f;
float ghostSPEED = 15.0f;
float frightenedSPEED = 20.0f;
bool restart = false;
bool score5000 = false;

int ghostEatenCount = 0;
int oldGhostEatenCount = 0;

bool isfrightenedblinky = false;
bool isfrightenedpinky = false;
bool isfrightenedinky = false;
bool isfrightenedclyde = false;

const char initialmaze[MAZE_HEIGHT][MAZE_WIDTH] = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#O####.#####.##.#####.####O#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.##### ## #####.######",
    "     #.##### ## #####.#     ",
    "     #.##          ##.#     ",
    "     #.## ###__### ##.#     ",
    "######.## #      # ##.######",
    "      .   #      #   .      ",
    "######.## #      # ##.######",
    "     #.## ######## ##.#     ",
    "     #.##          ##.#     ",
    "     #.## ######## ##.#     ",
    "######.## ######## ##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#",
    "#...##................##...#",
    "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#.##########.##.##########.#",
    "#O.......................O.#",
    "############################"
};

char maze[MAZE_HEIGHT][MAZE_WIDTH];
    
void drawMaze(char maze[MAZE_HEIGHT][MAZE_WIDTH]);
int avoidRotation(Vector2 *pos, Direction *direction, Direction *buffer_direction, char maze[MAZE_HEIGHT][MAZE_WIDTH], char Char);
int countPellets(char maze[MAZE_HEIGHT][MAZE_WIDTH]);
void updatePosition(Vector2 *pos, Direction *direction, Direction *buffer_direction, char maze[MAZE_HEIGHT][MAZE_WIDTH], char Char);
void checkCollisionWithPellets(Vector2 pos, char maze[MAZE_HEIGHT][MAZE_WIDTH], int *score,Sound *sound1, Sound *sup_siren);
void drawPacman(Vector2 pos, Direction direction, Texture2D sprite);
void drawScore(int score);
void drawlives(Texture2D spriteOpen);
bool isValidPosition(Vector2Int position);
bool ghostcollisionpacman(Vector2 pos, Vector2Int ghostPos);
Vector2Int* findShortestPath(Vector2Int start, Vector2Int target, int* pathLength);
Vector2Int ConvertToVector2Int(Vector2 pos);
Vector2Int RandomPos(Vector2 pos, Vector2Int ghostPos);
void moveGhost(Texture2D ghost, Vector2Int* ghostPos, Vector2Int targetPosghost, int* ghostMoveCounter, Vector2Int** path, int* pathIndex, int* pathLength, int speed);

int main(void)
{
    const int screenWidth = MAZE_WIDTH * 20;
    const int screenHeight = MAZE_HEIGHT * 20;
    const int frameRate = 60;

    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            maze[i][j] = initialmaze[i][j];
        }
    }

    InitWindow(MAZE_WIDTH * 20, MAZE_HEIGHT * 23, "PAC-MAN");
    InitAudioDevice();
    
    Vector2 pos = {(float)screenWidth / 2 + 10, (float)(screenHeight / 2 + 40)};
    Vector2 pacmanPos, pacmanDeathPos;
    Vector2Int blinkyPos = {14, 11}, pinkyPos = {14, 12}, inkyPos = {14, 13}, clydePos = {14, 14};

    Vector2Int scatterblinkyPos = {1, 2}, scatterpinkyPos = {28, 1}, scatterinkyPos = {28, 26}, scatterclydePos = {1, 26};

    Vector2Int distclyde;
    Vector2Int targetPosblinky = ConvertToVector2Int(pos), targetPospinky = ConvertToVector2Int(pos), targetPosinky = ConvertToVector2Int(pos), targetPosclyde = ConvertToVector2Int(pos);
    Vector2Int targetPosblinkyfrightened, targetPospinkyfrightened, targetPosinkyfrightened ,targetPosclydefrightened;
    Vector2Int targetPosblinkyEaten = {14, 11}, targetPospinkyEaten = {14, 11}, targetPosinkyEaten = {14, 12}, targetPosclydeEaten = {14, 13};
    int pathLengthblinky = 0, pathLengthpinky = 0, pathLengthinky = 0, pathLengthclyde = 0;
    int pathIndexblinky = 0, pathIndexpinky = 0, pathIndexinky = 0, pathIndexclyde = 0;
    Vector2Int *pathblinky = NULL, *pathpinky = NULL, *pathinky = NULL, *pathclyde = NULL;
    int blinkyMoveCounter = 0, pinkyMoveCounter = 0, inkyMoveCounter = 0, clydeMoveCounter = 0;

    Texture2D deathFrames[11];
    for (int i = 0; i < 11; i++) {
        char filename[20];
        sprintf(filename, "resources/sprites/pac/deathAnim/death%d.png", i+1);
        deathFrames[i] = LoadTexture(filename);
    }

    Texture2D blinky = LoadTexture("resources/sprites/ghosts/blinky1.png");
    Texture2D pinky = LoadTexture("resources/sprites/ghosts/pinky1.png");
    Texture2D inky = LoadTexture("resources/sprites/ghosts/inky1.png");
    Texture2D clyde = LoadTexture("resources/sprites/ghosts/clyde1.png");
    Texture2D frightened = LoadTexture("resources/sprites/ghosts/frightened.png");
    Texture2D frightenedwhite = LoadTexture("resources/sprites/ghosts/frightenedwhite.png");
    Texture2D frightenednormal = frightened;
    Texture2D eyes = LoadTexture("resources/sprites/ghosts/eyes.png");

    Texture2D originalblinky = blinky; 
    Texture2D originalpinky = pinky; 
    Texture2D originalinky = inky;
    Texture2D originalclyde = clyde; 

    Texture2D spriteOpen = LoadTexture("resources/sprites/pac/pacWide1.png"); 
    Texture2D spriteClosed = LoadTexture("resources/sprites/pac/pacNarrow1.png");
    Texture2D sprite = spriteClosed;

    Texture2D pacmanloadingtext = LoadTexture("resources/pacmantext.png");
    Texture2D blinkyloading = LoadTexture("resources/sprites/ghosts/blinky.png");
    Texture2D pinkyloading = LoadTexture("resources/sprites/ghosts/pinky.png");
    Texture2D inkyloading = LoadTexture("resources/sprites/ghosts/inky.png");
    Texture2D clydeloading = LoadTexture("resources/sprites/ghosts/clyde.png");
    Texture2D gameover = LoadTexture("resources/gameover.png");
    Texture2D gamewin = LoadTexture("resources/gamewin.png");

    Sound intro = LoadSound("resources/Audio/take_on_me_small.mp3");
    Sound sound1 = LoadSound("resources/Audio/munch_1.wav");
    Sound siren = LoadSound("resources/Audio/siren_2.wav"); 
    Sound Startmusic = LoadSound("resources/Audio/start.mp3");
    Sound sup_siren = LoadSound("resources/Audio/sup_pellet.mp3");
    Sound death = LoadSound("resources/Audio/death.mp3");

    SetTargetFPS(frameRate);

    Direction direction = STOP;
    Direction buffer_direction = STOP;
    int frameCounter = 0;
    int frameCounter2 = 0;
    int displaycounter = 0;
    int frightenedTimer = 0;
    int scatterTimer = 0;
    int waitTimer = 0;
    int score = 0;
    int x = 0;
    int ghostblink = 0;
    bool isGameOver = false;
    double livesDisplayStartTime = 0;
    const double displayDuration = 3.0;
    
    while (!WindowShouldClose() && !isGameOver) {
        double currentTime = GetTime(); 

        switch(CurrentScreen) {
            case DESCRIBE: {
                displaycounter++;
                if (displaycounter%30==0) {
                    x++;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    CurrentScreen = DIFFICULTY;
                }
            } break;

            case DIFFICULTY:
            {
                if(IsKeyPressed(KEY_ONE)){
                    level = 1;
                    ghostSPEED = 14.0f;
                    CurrentScreen = DIFF_SELECTION;
                } 
                else if(IsKeyPressed(KEY_TWO)){
                    level = 2;
                    ghostSPEED = 10.0f;
                    CurrentScreen = DIFF_SELECTION;
                }
                else if(IsKeyPressed(KEY_THREE)){
                    level = 3;
                    ghostSPEED = 9.0f;
                    frightenedSPEED = 15.0f;
                    CurrentScreen = DIFF_SELECTION;
                }
            }break;
            
            case DIFF_SELECTION:
            {
                displaycounter++;
                if (displaycounter%30==0) {
                    x++;
                }
                if(IsKeyPressed(KEY_ENTER)){
                    CurrentScreen = LOADING;
                }
            }break;
            
            case LOADING: {
                frameCounter2++;
                if (frameCounter2 > 243) {
                    CurrentScreen = GAMEPLAY;
                }
            } break;

            case GAMEOVER: {
                if(IsKeyPressed(KEY_Y)) {
                    restart = true;
                    CurrentScreen = GAMEPLAY;
                }
                else if (IsKeyPressed(KEY_N)) {
                    isGameOver = true;
                }
            }
            
            case GAMEWIN: {   
                if(IsKeyPressed(KEY_Y)) {
                    restart = true;
                    CurrentScreen = GAMEPLAY;
                }
                else if (IsKeyPressed(KEY_N)) {
                    isGameOver = true;
                }
            }
            default : break;
        }
        
        if (restart) {
            scatterTimer = 0;
            frightenedTimer = 0;
            isfrightenedblinky = false;
            isfrightenedinky = false;
            isfrightenedpinky = false;
            isfrightenedclyde = false;
            lives = 3;
            score = 0;
            for (int i = 0; i < MAZE_HEIGHT; i++) {
                for (int j = 0; j < MAZE_WIDTH; j++) {
                    maze[i][j] = initialmaze[i][j];
                }
            }
            direction = STOP;
            pos = (Vector2){(float)screenWidth / 2 + 10, (float)(screenHeight / 2 + 40)};
            blinkyPos = (Vector2Int){14, 11};
            pinkyPos = (Vector2Int){14, 12}; 
            inkyPos = (Vector2Int){14, 13}; 
            clydePos = (Vector2Int){15, 11};

            pathLengthblinky = 0; pathLengthpinky = 0; pathLengthinky = 0; pathLengthclyde = 0;
            pathIndexblinky = 0; pathIndexpinky = 0; pathIndexinky = 0; pathIndexclyde = 0;
            pathblinky = NULL; pathpinky = NULL; pathinky = NULL; pathclyde = NULL;
            blinkyMoveCounter = 0; pinkyMoveCounter = 0; inkyMoveCounter = 0; clydeMoveCounter = 0;

            isDead = false;
            livesDecreased = false;
            restart = false;
            CurrentScreen = GAMEPLAY;
        }

        if (frameCounter == frameRate / 4) {
            sprite = spriteOpen;
            if (!IsSoundPlaying(Startmusic) && CurrentScreen == GAMEPLAY) {
                SetSoundVolume(siren, 0.7f);
                PlaySound(siren);
            }
            if (!IsSoundPlaying(sup_siren)) {
                SetSoundVolume(siren, 0.7f);
            }
            else {
                SetSoundVolume(siren, 0.01f);
            }
        }
        if (frameCounter == frameRate / 2) {
            sprite = spriteClosed;
            frameCounter = 0;
        }

        if (!IsSoundPlaying(intro) && (CurrentScreen == DESCRIBE || CurrentScreen == DIFFICULTY || CurrentScreen == DIFF_SELECTION)) {
            PlaySound(intro);
            SetSoundVolume(intro, 0.7f);
        }
        if (CurrentScreen == LOADING) {
            StopSound(intro);
        }

        if (!IsSoundPlaying(death) && isDead && CurrentScreen == GAMEPLAY) {
            PlaySound(death);
            SetSoundVolume(death, 0.5f);
        }
        if (CurrentScreen == REMAININGLIVES) {
            StopSound(death);
        }
        if (IsSoundPlaying(death)) {
            SetSoundVolume(siren, 0.01f);
        }
                  
        BeginDrawing();
        ClearBackground(BLACK);

        switch(CurrentScreen) {   
            case DESCRIBE:
            {
                if (displaycounter > 60)
                {
                    DrawText("CHARACTER / NICKNAME", 130, 40, 25, GRAY);
                    if (displaycounter > 120)
                    {
                        DrawText("SHADOW  -  BLINKY", 210, 100, 25, RED);
                        DrawTexture(blinkyloading, 130, 100, WHITE);
                        if (displaycounter > 180)
                        {
                            DrawText("SPEEDY  -  PINKY", 210, 200, 25, PINK);
                            DrawTexture(pinkyloading, 130, 200, WHITE);
                            if (displaycounter > 240)
                            {
                                DrawText("BASHFUL  -  INKY", 210, 300, 25, SKYBLUE);
                                DrawTexture(inkyloading, 130, 300, WHITE);
                                if (displaycounter > 300)
                                {
                                    DrawText("POKEY  -  CLYDE", 210, 400, 25, ORANGE);
                                    DrawTexture(clydeloading, 130, 400, WHITE);
                                    if (displaycounter > 360)
                                    {
                                        DrawCircle(200, 480, 3, YELLOW);
                                        DrawText(" - 10 points", 250, 470, 25, GRAY);
                                        DrawCircle(200, 540, 9, RED);
                                        DrawText(" - 50 points", 250, 530, 25, GRAY);
                                    }
                                }
                            }
                        }
                    }
                }
                if (displaycounter > 360 && (x % 2 == 0)) {
                    DrawText("PRESS ENTER TO CONTINUE", 90, 630, 28, YELLOW);
                }
            } break;

            case DIFFICULTY:
            {    
                DrawText("SELECT A DIFFICULTY:", 100, 80, 28, YELLOW);                    
                DrawText("1. EASY MODE", 120, 230, 28, YELLOW);
                DrawText("2. NORMAL MODE", 120, 330, 28, YELLOW);
                DrawText("3. HARD MODE", 120, 430, 28, YELLOW);
            } break;
                
            case DIFF_SELECTION:
            {
                if (level == 1){
                    DrawText("1. EASY MODE", 120, 230, 28, GREEN);                    
                }
                else if (level == 2){
                    DrawText("2. NORMAL MODE", 120, 330, 28, GREEN);
                }
                else if (level == 3){
                    DrawText("3. HARD MODE", 120, 430, 28, GREEN);                   
                }
                if (displaycounter > 300 && (x % 2 == 0)) {
                    DrawText("PRESS ENTER TO START", 100, 530, 28, YELLOW);
                }
            } break;
                    
            case LOADING:
            {   
                DrawTexture(pacmanloadingtext, 97, 100, WHITE);
                DrawText("LOADING GAME...", 200, 380, 20, YELLOW);
                drawPacman(pos,direction,sprite);
            } break;

            case GAMEPLAY:
            {   
                bool condition = (!superPelletEaten) && 
                                (ghostcollisionpacman(pos, blinkyPos) || 
                                ghostcollisionpacman(pos, pinkyPos) || 
                                ghostcollisionpacman(pos, inkyPos) || 
                                ghostcollisionpacman(pos, clydePos));

                if (condition) {
                    if (!livesDecreased) {
                        lives--;
                        livesDecreased = true;
                        isDead = true;
                        waitTimer = 0;  
                        currentDeathFrame = 0;

                        blinkyPos = (Vector2Int){-1, -1};
                        pinkyPos = (Vector2Int){-1, -1};
                        inkyPos = (Vector2Int){-1, -1};
                        clydePos = (Vector2Int){-1, -1};

                        pacmanDeathPos = pos;
                    }
                }

                if (isDead) {
                    waitTimer++;
                    if (waitTimer % 11 == 0) {
                        currentDeathFrame++;
                        if (currentDeathFrame >= 11) {
                            currentDeathFrame = 10;
                        }
                    }

                    drawPacman(pacmanDeathPos, STOP, deathFrames[currentDeathFrame]);

                    if (waitTimer >= 120) {
                        if (lives > 0) {
                            CurrentScreen = REMAININGLIVES;
                            livesDisplayStartTime = currentTime;
                            isDead = false;

                            blinkyPos = (Vector2Int){14, 11};
                            pinkyPos = (Vector2Int){14, 12}; 
                            inkyPos = (Vector2Int){14, 13}; 
                            clydePos = (Vector2Int){15, 11};

                            pathLengthblinky = 0; pathLengthpinky = 0; pathLengthinky = 0; pathLengthclyde = 0;
                            pathIndexblinky = 0; pathIndexpinky = 0; pathIndexinky = 0; pathIndexclyde = 0;
                            pathblinky = NULL; pathpinky = NULL; pathinky = NULL; pathclyde = NULL;
                            blinkyMoveCounter = 0; pinkyMoveCounter = 0; inkyMoveCounter = 0; clydeMoveCounter = 0;
                        }
                        if (lives <= 0) {
                            CurrentScreen = GAMEOVER;
                        }
                        waitTimer = 0;
                    }
                }

                if (!condition) {
                    livesDecreased = false;
                }
            } break;
            
            case REMAININGLIVES: 
            {
                DrawText(TextFormat("Remaining Lives: %i", lives), 100, 250, 40, RED);
                StopSound(siren);

                pos.x = (float)screenWidth / 2 + 10; 
                pos.y = (float)(screenHeight / 2 + 40);
                direction = STOP;
                targetPosblinky.row = 14;
                targetPosblinky.col = 11;
                targetPospinky.col = 14;
                targetPospinky.row = 12;
                targetPosinky.row = 14;
                targetPosinky.col = 13;
                targetPosclyde.row = 15;
                targetPosclyde.col = 11;
                
                scatterTimer = 0;

                while ((blinkyPos.row != targetPosblinky.row || blinkyPos.col != targetPosblinky.col) || 
                (pinkyPos.row != targetPospinky.row || pinkyPos.col != targetPospinky.col) ||
                (inkyPos.row != targetPosinky.row || inkyPos.col != targetPosinky.col) ||
                (clydePos.row != targetPosclyde.row || clydePos.col != targetPosclyde.col)) {
                    moveGhost(blinky, &blinkyPos, targetPosblinky, &blinkyMoveCounter, &pathblinky, &pathIndexblinky, &pathLengthblinky, ghostSPEED);
                    moveGhost(pinky, &pinkyPos, targetPospinky, &pinkyMoveCounter, &pathpinky, &pathIndexpinky, &pathLengthpinky, ghostSPEED);
                    moveGhost(inky, &inkyPos, targetPosinky, &inkyMoveCounter, &pathinky, &pathIndexinky, &pathLengthinky, ghostSPEED);
                    moveGhost(clyde, &clydePos, targetPosclyde, &clydeMoveCounter, &pathclyde, &pathIndexclyde, &pathLengthclyde, ghostSPEED);
                }
                if (currentTime - livesDisplayStartTime >= displayDuration)
                    CurrentScreen = GAMEPLAY;
            } break;

            case GAMEOVER: 
            {   
                DrawTexture(gameover, 200, 180, WHITE);
                DrawText("DO YOU WISH TO REPLAY?", 110, 530, 25, DARKBLUE);
                DrawText("PRESS Y/N", 210, 560, 25, WHITE);
                StopSound(siren);
            } break;

            case GAMEWIN: 
            {   
                DrawTexture(gamewin, 40, 200, WHITE);
                DrawText("DO YOU WISH TO REPLAY?", 110, 530, 25, RED);
                DrawText("PRESS Y/N", 210, 560, 25, WHITE);
                StopSound(siren);
            } break;
            default: break;
        }
        
        if (CurrentScreen == GAMEPLAY) {
            char Char1 = '#';
            if (!isDead) {
                updatePosition(&pos, &direction, &buffer_direction, maze, Char1);
                checkCollisionWithPellets(pos, maze, &score, &sound1, &sup_siren);
                drawPacman(pos, direction, sprite);
            }
            drawScore(score);
            drawlives(spriteOpen);

            if (countPellets(maze) == 0) {
                CurrentScreen = GAMEWIN;
            }
            else {
                drawMaze(maze);
            }
            
            if (isDead) {
                pathblinky = NULL;
                pathpinky = NULL;
                pathinky = NULL;
                pathclyde = NULL;
                moveGhost(blinky, &blinkyPos, blinkyPos, &blinkyMoveCounter, &pathblinky, &pathIndexblinky, &pathLengthblinky, 10);
                moveGhost(pinky, &pinkyPos, pinkyPos, &pinkyMoveCounter, &pathpinky, &pathIndexpinky, &pathLengthpinky, 10);
                moveGhost(inky, &inkyPos, inkyPos, &inkyMoveCounter, &pathinky, &pathIndexinky, &pathLengthinky, 10);
                moveGhost(clyde, &clydePos, clydePos, &clydeMoveCounter, &pathclyde, &pathIndexclyde, &pathLengthclyde, 10);
            }

            scatterTimer++;
            if (scatterTimer < SCATTER_TIME * frameRate) {
                moveGhost(blinky, &blinkyPos, scatterblinkyPos, &blinkyMoveCounter, &pathblinky, &pathIndexblinky, &pathLengthblinky, ghostSPEED);
                moveGhost(pinky, &pinkyPos, scatterpinkyPos, &pinkyMoveCounter, &pathpinky, &pathIndexpinky, &pathLengthpinky, ghostSPEED);
                moveGhost(inky, &inkyPos, scatterinkyPos, &inkyMoveCounter, &pathinky, &pathIndexinky, &pathLengthinky, ghostSPEED);
                moveGhost(clyde, &clydePos, scatterclydePos, &clydeMoveCounter, &pathclyde, &pathIndexclyde, &pathLengthclyde, ghostSPEED);
            }
            else {
                //targetPosblinky
                targetPosblinky = ConvertToVector2Int(pos);
                //targetPospinky
                targetPospinky.row = ConvertToVector2Int(pos).row+3;
                targetPospinky.col = ConvertToVector2Int(pos).col+3;
                //targetPosinky
                if (direction == RIGHT) {
                    pacmanPos.x = pos.x+2;
                    pacmanPos.y = pos.y;
                }
                else if (direction == LEFT) {
                    pacmanPos.x = pos.x-2;
                    pacmanPos.y = pos.y;
                }
                else if (direction == UP) {
                    pacmanPos.x = pos.x;
                    pacmanPos.y = pos.y+2;
                }
                else if (direction == DOWN) {
                    pacmanPos.x = pos.x;
                    pacmanPos.y = pos.y-2;
                } 
                targetPosinky.row = (abs(blinkyPos.row + (2*(ConvertToVector2Int(pacmanPos).row - blinkyPos.row)))%28);
                targetPosinky.col = (abs(blinkyPos.col + (2*(ConvertToVector2Int(pacmanPos).col - blinkyPos.col)))%31);
                //targetposclyde
                distclyde.row = ConvertToVector2Int(pos).row - clydePos.row;
                distclyde.col = ConvertToVector2Int(pos).col - clydePos.col;
                double dclyde = sqrt(pow(distclyde.row,2)+pow(distclyde.col,2));
                if (dclyde>8)
                    targetPosclyde = targetPospinky;
                else
                    targetPosclyde = scatterclydePos;
                
                if (superPelletCount > oldSuperPelletCount && superPelletEaten == true) {
                    isfrightenedblinky = true;
                    isfrightenedpinky = true;
                    isfrightenedinky = true;
                    isfrightenedclyde = true;
                    frightenedTimer = 0;
                    oldSuperPelletCount = superPelletCount;
                    oldGhostEatenCount = 0;
                    ghostEatenCount = 0;
                }
                if (frightenedTimer >= FRIGHTENED_TIME * frameRate && superPelletEaten == true) {
                    frightenedTimer = 0;
                    oldSuperPelletCount = superPelletCount;
                    superPelletEaten = false;
                    oldGhostEatenCount = 0;
                    ghostEatenCount = 0;
                    frightened = frightenednormal;
                    blinky = originalblinky;
                    pinky = originalpinky;
                    inky = originalinky;
                    clyde = originalclyde;
                }
                if (frightenedTimer >= (FRIGHTENED_TIME - 2) * frameRate) {
                    ghostblink++;
                    if (ghostblink % 30 == 0) {
                        frightened = frightenedwhite;
                    }
                    else if (ghostblink % 15 == 0) {
                        frightened = frightenednormal;
                    }
                }

                if (superPelletEaten) {
                    if ((ghostcollisionpacman(pos, blinkyPos) && isfrightenedblinky)  || (ghostcollisionpacman(pos, pinkyPos) && isfrightenedpinky) || (ghostcollisionpacman(pos, inkyPos) && isfrightenedinky) || (ghostcollisionpacman(pos, clydePos) && isfrightenedclyde)) {
                        if (oldGhostEatenCount < 4 && ghostEatenCount < 4) {
                            score += (200 * pow(2, ghostEatenCount));
                            ghostEatenCount++;
                        } 
                    }
                    if (ghostEatenCount > oldGhostEatenCount) {
                        oldGhostEatenCount = ghostEatenCount;
                    }

                    if (ghostcollisionpacman(pos, blinkyPos)) {
                        isfrightenedblinky = false;
                        blinky = eyes;
                        pathblinky = NULL;
                        moveGhost(blinky, &blinkyPos, targetPosblinkyEaten, &blinkyMoveCounter, &pathblinky, &pathIndexblinky, &pathLengthblinky, 8);
                    }
                    if (ghostcollisionpacman(pos, pinkyPos)) {
                        isfrightenedpinky = false;
                        pinky = eyes;
                        pathpinky = NULL;
                        moveGhost(pinky, &pinkyPos, targetPospinkyEaten, &pinkyMoveCounter, &pathpinky, &pathIndexpinky, &pathLengthpinky, 8);
                    }
                    if (ghostcollisionpacman(pos, inkyPos)) {
                        isfrightenedinky = false; 
                        inky = eyes;
                        pathinky = NULL;
                        moveGhost(inky, &inkyPos, targetPosinkyEaten, &inkyMoveCounter, &pathinky, &pathIndexinky, &pathLengthinky, 8);
                    }
                    if (ghostcollisionpacman(pos, clydePos)) {
                        isfrightenedclyde = false; 
                        clyde = eyes;
                        pathclyde = NULL;
                        moveGhost(clyde, &clydePos, targetPosclydeEaten, &clydeMoveCounter, &pathclyde, &pathIndexclyde, &pathLengthclyde, 8);
                    }

                    frightenedTimer++;
                    if (isfrightenedblinky) {
                        targetPosblinkyfrightened = RandomPos(pos, blinkyPos);
                        moveGhost(frightened, &blinkyPos, targetPosblinkyfrightened, &blinkyMoveCounter, &pathblinky, &pathIndexblinky, &pathLengthblinky, frightenedSPEED);
                    }
                    else {
                        moveGhost(blinky, &blinkyPos, targetPosblinky, &blinkyMoveCounter, &pathblinky, &pathIndexblinky, &pathLengthblinky, ghostSPEED);
                    }
                    if (isfrightenedpinky) {
                        targetPospinkyfrightened = RandomPos(pos, pinkyPos);
                        moveGhost(frightened, &pinkyPos, targetPospinkyfrightened, &pinkyMoveCounter, &pathpinky, &pathIndexpinky, &pathLengthpinky, frightenedSPEED);
                    }
                    else {
                        moveGhost(pinky, &pinkyPos, targetPospinky, &pinkyMoveCounter, &pathpinky, &pathIndexpinky, &pathLengthpinky, ghostSPEED);  
                    }
                    if (isfrightenedinky) {
                        targetPosinkyfrightened = RandomPos(pos, inkyPos);
                        moveGhost(frightened, &inkyPos, targetPosinkyfrightened, &inkyMoveCounter, &pathinky, &pathIndexinky, &pathLengthinky, frightenedSPEED);
                    }
                    else {
                        moveGhost(inky, &inkyPos, targetPosinky, &inkyMoveCounter, &pathinky, &pathIndexinky, &pathLengthinky, ghostSPEED); 
                    }
                    if (isfrightenedclyde) {
                        targetPosclydefrightened = RandomPos(pos, clydePos);
                        moveGhost(frightened, &clydePos, targetPosclydefrightened, &clydeMoveCounter, &pathclyde, &pathIndexclyde, &pathLengthclyde, frightenedSPEED);
                    }
                    else {
                        moveGhost(clyde, &clydePos, targetPosclyde, &clydeMoveCounter, &pathclyde, &pathIndexclyde, &pathLengthclyde, ghostSPEED);
                    }
                }
                else {
                    moveGhost(blinky, &blinkyPos, targetPosblinky, &blinkyMoveCounter, &pathblinky, &pathIndexblinky, &pathLengthblinky, ghostSPEED);
                    moveGhost(pinky, &pinkyPos, targetPospinky, &pinkyMoveCounter, &pathpinky, &pathIndexpinky, &pathLengthpinky, ghostSPEED);  
                    moveGhost(inky, &inkyPos, targetPosinky, &inkyMoveCounter, &pathinky, &pathIndexinky, &pathLengthinky, ghostSPEED); 
                    moveGhost(clyde, &clydePos, targetPosclyde, &clydeMoveCounter, &pathclyde, &pathIndexclyde, &pathLengthclyde, ghostSPEED);
                }
            }

            if (hscore <= score){
                hscore = score;
            }
            
            char *filename;
            switch(level) {
                case 1:
                    filename = "highscore_easy.txt";
                    break;
                case 2:
                    filename = "highscore_medium.txt";
                    break;
                case 3:
                    filename = "highscore_hard.txt";
                    break;
                default:
                    filename = "highscore.txt"; 
                    break;
            }
            
            FILE *f = fopen(filename, "r");
            
            if (f != NULL) {
                fscanf(f, "%d", &filescore);
                fclose(f);
            }

            if (hscore > filescore) {
                f = fopen(filename, "w");
                if (f != NULL) {
                    fprintf(f, "%d", hscore);
                    fclose(f);
                }
            }
        }

        if (score >= 5000 && lives < 3 && !score5000) {
            lives += 1;
            score5000 = true;
        }
        
        if (CurrentScreen == DIFF_SELECTION){
            PlaySound(Startmusic);
        }

        EndDrawing();

        frameCounter++;
    }

    if (pathblinky != NULL)
        free(pathblinky);
    if (pathpinky != NULL)
        free(pathpinky);
    if (pathinky != NULL)
        free(pathinky);
    if (pathclyde != NULL)
        free(pathclyde);

    UnloadTexture(spriteOpen);
    UnloadTexture(spriteClosed);
    UnloadTexture(blinky);
    UnloadTexture(pinky);
    UnloadTexture(inky);
    UnloadTexture(clyde);
    CloseAudioDevice();
    CloseWindow();        

    return 0;
}

void drawMaze(char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    for (int y = 0; y < MAZE_HEIGHT; y++)
    {
        for (int x = 0; x < MAZE_WIDTH; x++)
        {
            if (maze[y][x] == '#')
            {   
                Rectangle rec = {x*20, y*20, 20, 20};
                DrawRectangleRec(rec, BLACK);
                DrawRectangleLinesEx(rec, 5, DARKBLUE);
            }
            else if (maze[y][x] == '.')
            {
                DrawCircle(x*20+10, y*20+10, 2, YELLOW);
            }
            else if (maze[y][x] == 'O')
            {
                DrawCircle(x*20+10, y*20+10, 6, RED);
            }
            if (maze[y][x] == '_')
            {   
                Rectangle rec = {x*20, y*20, 20, 20};
                DrawRectangleRec(rec, BLACK);
                DrawRectangleLinesEx(rec, 5, WHITE);
            }
        }
    }
}

int avoidRotation(Vector2 *pos, Direction *direction, Direction *buffer_direction, char maze[MAZE_HEIGHT][MAZE_WIDTH], char Char) {
    if ((*direction == RIGHT && maze[(int)(pos->y)/20][(int)(pos->x + 20)/20] == Char)
    || (*direction == RIGHT && maze[(int)(pos->y)/20][(int)(pos->x + 20)/20] == '_')) {
        *buffer_direction = STOP;
    }
    else if ((*direction == LEFT && maze[(int)(pos->y)/20][(int)(pos->x - 20)/20] == Char)
    || (*direction == LEFT && maze[(int)(pos->y)/20][(int)(pos->x - 20)/20] == '_')) {
        *buffer_direction = STOP;
    }
    else if ((*direction == UP && maze[(int)(pos->y - 20)/20][(int)(pos->x)/20] == Char)
    || (*direction == UP && maze[(int)(pos->y - 20)/20][(int)(pos->x)/20] == '_')) {
        *buffer_direction = STOP;
    }
    else if ((*direction == DOWN && maze[(int)(pos->y + 20)/20][(int)(pos->x)/20] == Char)
    || (*direction == DOWN && maze[(int)(pos->y + 20)/20][(int)(pos->x)/20] == '_')) {
        *buffer_direction = STOP;
    }
    else
        return 0;
    return 1;
}

void updatePosition(Vector2 *pos, Direction *direction, Direction *buffer_direction, char maze[MAZE_HEIGHT][MAZE_WIDTH], char Char) {
    Vector2 newPos = *pos;
    float error_term = 7;

    if (pos->x > (int)((MAZE_WIDTH - 1) * 20)) {
        pos->x = 1;
    } else if (pos->x < 1) {
        pos->x = (MAZE_WIDTH - 1) * 20;
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        *buffer_direction = RIGHT;
        if (((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x + 20)/ 20] != Char) && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x + 20)/ 20] != Char))
        && ((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x + 20)/ 20] != '_') && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x + 20)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
        else
            avoidRotation(pos, direction, buffer_direction, maze, Char);
    } 
    if (IsKeyPressed(KEY_LEFT)) {
        *buffer_direction = LEFT;
        if (((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x - 20)/ 20] != Char) && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x - 20)/ 20] != Char))
        && ((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x - 20)/ 20] != '_') && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x - 20)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
        else
            avoidRotation(pos, direction, buffer_direction, maze, Char);
    } 
    if (IsKeyPressed(KEY_UP)) {
        *buffer_direction = UP;
        if (((maze[(int)(pos->y - 20)/ 20][(int)(pos->x + error_term)/ 20] != Char) && (maze[(int)(pos->y - 20)/ 20][(int)(pos->x - error_term)/ 20] != Char))
        && ((maze[(int)(pos->y - 20)/ 20][(int)(pos->x + error_term)/ 20] != '_') && (maze[(int)(pos->y - 20)/ 20][(int)(pos->x - error_term)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
        else
            avoidRotation(pos, direction, buffer_direction, maze, Char);
    } 
    if (IsKeyPressed(KEY_DOWN)) {
        *buffer_direction = DOWN;
        if (((maze[(int)(pos->y + 20)/ 20][(int)(pos->x + error_term)/ 20] != Char) && (maze[(int)(pos->y + 20)/ 20][(int)(pos->x - error_term)/ 20] != Char))
        && ((maze[(int)(pos->y + 20)/ 20][(int)(pos->x + error_term)/ 20] != '_') && (maze[(int)(pos->y + 20)/ 20][(int)(pos->x - error_term)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
        else
            avoidRotation(pos, direction, buffer_direction, maze, Char);
    }
    
    if (*buffer_direction == RIGHT) {
        if (((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x + 20)/ 20] != Char) && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x + 20)/ 20] != Char))
        && ((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x + 20)/ 20] != '_') && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x + 20)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
    } 
    if (*buffer_direction == LEFT) {
        if (((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x - 20)/ 20] != Char) && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x - 20)/ 20] != Char))
        && ((maze[(int)(pos->y - error_term)/ 20][(int)(pos->x - 20)/ 20] != '_') && (maze[(int)(pos->y + error_term)/ 20][(int)(pos->x - 20)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
    } 
    if (*buffer_direction == UP) {
        if (((maze[(int)(pos->y - 20)/ 20][(int)(pos->x + error_term)/ 20] != Char) && (maze[(int)(pos->y - 20)/ 20][(int)(pos->x - error_term)/ 20] != Char))
        && ((maze[(int)(pos->y - 20)/ 20][(int)(pos->x + error_term)/ 20] != '_') && (maze[(int)(pos->y - 20)/ 20][(int)(pos->x - error_term)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
    } 
    if (*buffer_direction == DOWN) {
        if (((maze[(int)(pos->y + 20)/ 20][(int)(pos->x + error_term)/ 20] != Char) && (maze[(int)(pos->y + 20)/ 20][(int)(pos->x - error_term)/ 20] != Char))
        && ((maze[(int)(pos->y + 20)/ 20][(int)(pos->x + error_term)/ 20] != '_') && (maze[(int)(pos->y + 20)/ 20][(int)(pos->x - error_term)/ 20] != '_'))) {
            *direction = *buffer_direction;
            *buffer_direction = STOP;
        }
    }    
 
    if (*direction == RIGHT) {
        newPos.x += SPEED; 
        if ((maze[(int)newPos.y / 20][(int)(newPos.x + error_term) / 20] != Char) && (maze[(int)newPos.y / 20][(int)(newPos.x + error_term) / 20] != '_')) {
            *pos = newPos;
        }
        else {
            newPos.x -= SPEED;
        }
    }
    else if (*direction == LEFT) {
        newPos.x -= SPEED;
        if ((maze[(int)newPos.y / 20][(int)(newPos.x - error_term)/ 20] != Char) && (maze[(int)newPos.y / 20][(int)(newPos.x - error_term)/ 20] != '_')) {
            *pos = newPos;
        }
        else {
            newPos.x += SPEED;
        }
    }        
    else if (*direction == UP) {
        newPos.y -= SPEED;
        if ((maze[(int)(newPos.y - error_term)/ 20][(int)newPos.x / 20] != Char) && (maze[(int)(newPos.y - error_term)/ 20][(int)newPos.x / 20] != '_')) {
            *pos = newPos;
        }
        else {
            newPos.y += SPEED;
        }
    }
    else if (*direction == DOWN) {
        newPos.y += SPEED; 
        if ((maze[(int)(newPos.y + error_term)/ 20][(int)newPos.x / 20] != Char) && (maze[(int)(newPos.y + error_term)/ 20][(int)newPos.x / 20] != '_')) {
            *pos = newPos;
        }
        else {
            newPos.y -= SPEED;
        }    
    }
}

void checkCollisionWithPellets(Vector2 pos, char maze[MAZE_HEIGHT][MAZE_WIDTH], int *score, Sound *sound1, Sound *sup_siren) {
    if (maze[(int)pos.y / 20][(int)pos.x / 20] == '.') {
        *score += 10;
        maze[(int)pos.y / 20][(int)pos.x / 20] = ' ';
        SetSoundVolume(*sound1, 0.8f);
        if (!IsSoundPlaying(*sup_siren))    {
            PlaySound(*sound1);
        }
    }

    if (maze[(int)pos.y / 20][(int)pos.x / 20] == 'O') {
        superPelletEaten = true;
        isfrightenedblinky = true;
        isfrightenedpinky = true;
        isfrightenedinky = true;
        isfrightenedclyde = true;
        ghostEatenCount = 0;
        oldGhostEatenCount = 0;
        superPelletCount++;
        *score += 50;
        maze[(int)pos.y / 20][(int)pos.x / 20] = ' ';
        SetSoundVolume(*sound1, 0.01f);
        SetSoundVolume(*sup_siren, 0.1f);
        PlaySound(*sup_siren);
    }
}

void drawPacman(Vector2 pos, Direction direction, Texture2D sprite) {
    Rectangle sourceRec = {0.0f, 0.0f, sprite.width, sprite.height};
    Vector2 origin = {(float)sprite.width / 2, (float)sprite.height / 2};
    Vector2 adjustedPos = {(float)((int)pos.x / 20) * 20 + 10, (float)((int)pos.y / 20) * 20 + 10};
    Rectangle pacmanRec = {adjustedPos.x, adjustedPos.y, sprite.width, sprite.height};

    float rotation = 0.0f;

    switch (direction) {
        case RIGHT: rotation = 0.0f; break;
        case LEFT: rotation = 180.0f; break;
        case UP: rotation = -90.0f; break;
        case DOWN: rotation = 90.0f; break;
        default: break;
    }

    DrawTexturePro(sprite, sourceRec, pacmanRec, origin, rotation, WHITE);
}

void drawScore(int score) {
    DrawText(TextFormat("Score: %i", score), 10, (MAZE_HEIGHT*23) - 60, 20, LIGHTGRAY);
    DrawText(TextFormat("High Score: %i", filescore), 10, (MAZE_HEIGHT*23) - 30, 20, LIGHTGRAY);
}

void drawlives(Texture2D spriteOpen) {
    int x = 25;
    for (int i = 0; i<lives; i++) {
        DrawTexture(spriteOpen, (MAZE_WIDTH*20)-(10+x), (MAZE_HEIGHT*23) - 60, WHITE);
        x+=25;
    }
}

bool isValidPosition(Vector2Int position) {
    if (position.row < 0 || position.col < 0 || position.row >= MAZE_HEIGHT || position.col >= MAZE_WIDTH)
        return false;
    return maze[position.row][position.col] != '#';
}

Vector2Int* findShortestPath(Vector2Int start, Vector2Int target, int* pathLength) {
    Vector2Int* queue = (Vector2Int*)malloc(sizeof(Vector2Int) * MAZE_HEIGHT * MAZE_WIDTH);
    bool visited[MAZE_HEIGHT][MAZE_WIDTH];
    memset(visited, false, sizeof(visited));
    int front = 0, rear = 0;

    Vector2Int parent[MAZE_HEIGHT][MAZE_WIDTH];
    memset(parent, -1, sizeof(parent));

    queue[rear++] = start;
    visited[start.row][start.col] = true;

    while (front < rear) {
        Vector2Int current = queue[front++];
        if (current.row == target.row && current.col == target.col) {
            Vector2Int* path = (Vector2Int*)malloc(sizeof(Vector2Int) * MAZE_HEIGHT * MAZE_WIDTH);
            int length = 0;
            while (!(current.row == start.row && current.col == start.col)) {
                path[length++] = current;
                current = parent[current.row][current.col];
            }
            path[length++] = start;

            *pathLength = length;
            Vector2Int* reversedPath = (Vector2Int*)malloc(sizeof(Vector2Int) * length);
            for (int i = 0; i < length; i++) {
                reversedPath[i] = path[length - 1 - i];
            }
            free(path);
            free(queue);
            return reversedPath;
        }

        Vector2Int directions[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        for (int i = 0; i < 4; i++) {
            Vector2Int next = {current.row + directions[i].row, current.col + directions[i].col};
            if (isValidPosition(next) && !visited[next.row][next.col]) {
                queue[rear++] = next;
                visited[next.row][next.col] = true;
                parent[next.row][next.col] = current;
            }
        }
    }

    free(queue);
    return NULL;
}

Vector2Int  ConvertToVector2Int(Vector2 pos) {
    Vector2Int vec2Int;
    vec2Int.row = (int)(pos.y / 20); 
    vec2Int.col = (int)(pos.x / 20);
    return vec2Int;
}

void moveGhost(Texture2D ghost, Vector2Int* ghostPos, Vector2Int targetPosghost, int* ghostMoveCounter, Vector2Int** path, int* pathIndex, int* pathLength, int speed) {
    if (*path == NULL || *pathIndex >= *pathLength) {
        if (*path != NULL) {
            free(*path);
        }
        *path = findShortestPath(*ghostPos, targetPosghost, pathLength);
        *pathIndex = 0;
    }

    (*ghostMoveCounter)++;
    if (*ghostMoveCounter >= speed) {
        if (*path != NULL && *pathIndex < *pathLength) {
            *ghostPos = (*path)[*pathIndex];
            (*pathIndex)++;
        }
        *ghostMoveCounter = 0;
    }
    DrawTexture(ghost, ghostPos->col * 20, ghostPos->row * 20, WHITE);
}

Vector2Int RandomPos(Vector2 pos, Vector2Int ghostPos) {
    Vector2Int distfright, distcorner;
    Vector2Int corner[4] = {{1, 1},{29, 26},{29, 1},{1, 26}};
    ghostPos.row = GetRandomValue(1,27);
    ghostPos.col = GetRandomValue(1,30);
    distfright.row = abs(ConvertToVector2Int(pos).row - ghostPos.row);
    distfright.col = abs(ConvertToVector2Int(pos).col - ghostPos.col);
    double dfright = sqrt(pow(distfright.row,2)+pow(distfright.col,2));
    if (dfright>8) {
        return distfright;
    }
    for (int i=0; i<3; i++) {
        distcorner.row = abs(ConvertToVector2Int(pos).row - corner[i].row);
        distcorner.col = abs(ConvertToVector2Int(pos).col - corner[i].col);
        double dcorner = sqrt(pow(distcorner.row,2)+pow(distcorner.col,2));
        if (dcorner<8) {
            if (i==3) {
                return corner[i-1];
            }
            return corner[i+1];
        }
    }
    return corner[GetRandomValue(0, 3)];
}

bool ghostcollisionpacman(Vector2 pos, Vector2Int ghostPos) {
    Vector2Int pacmanPos = ConvertToVector2Int(pos);
    bool flag;
    if (pacmanPos.row == ghostPos.row && pacmanPos.col == ghostPos.col) {
        flag = true;
    }
    else {
        flag = false;
    }
    return flag;
}

int countPellets(char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    int count = 0;
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            if (maze[i][j] == '.' || maze[i][j] == 'O') {
                count++;
            }
        }
    }
    return count;
}