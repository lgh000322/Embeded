#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// height and width of the boundary 
const int width = 80; 
const int height = 20; 

// Snake head coordinates of snake (x-axis, y-axis) 
int x, y; 
// Food coordinates 
int fruitCordX, fruitCordY; 
// variable to store the score of he player 
int playerScore; 
// Array to store the coordinates of snake tail (x-axis, y-axis) 
int snakeTailX[100], snakeTailY[100]; 
// variable to store the length of the snake's tail 
int snakeTailLen; 
// for storing snake's moving direction 
enum snakesDirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; 
// direction variable 
enum snakesDirection sDir; 
// boolean variable for checking game is over or not 
bool isGameOver; 

// Function to initialize game variables 
void GameInit() 
{ 
    isGameOver = false; 
    sDir = STOP; 
    x = width / 2; 
    y = height / 2; 
    fruitCordX = rand() % width; 
    fruitCordY = rand() % height; 
    playerScore = 0; 
} 

// Function for creating the game board & rendering 
void GameRender(char *playerName) 
{ 
    system("clear"); // Clear the console 

    // Creating top walls with '-' 
    for (int i = 0; i < width + 2; i++) 
        printf("-"); 
    printf("\n"); 

    for (int i = 0; i < height; i++) { 
        for (int j = 0; j <= width; j++) { 
            // Creating side walls with '|' 
            if (j == 0 || j == width) 
                printf("|"); 
            // Creating snake's head with 'O' 
            else if (i == y && j == x) 
                printf("O"); 
            // Creating the snake's food with '#' 
            else if (i == fruitCordY && j == fruitCordX) 
                printf("#"); 
            // Creating snake's tail with 'o' 
            else { 
                bool prTail = false; 
                for (int k = 0; k < snakeTailLen; k++) { 
                    if (snakeTailX[k] == j && snakeTailY[k] == i) { 
                        printf("o"); 
                        prTail = true; 
                    } 
                } 
                if (!prTail) 
                    printf(" "); 
            } 
        } 
        printf("\n"); 
    } 

    // Creating bottom walls with '-' 
    for (int i = 0; i < width + 2; i++) 
        printf("-"); 
    printf("\n"); 

    // Display player's score 
    printf("%s's Score: %d\n", playerName, playerScore); 
    printf("Up: 8 / Down: 2 / Left: 4 / Right: 6\n");
} 

// Function for updating the game state 
void UpdateGame() 
{ 
    int headX = x;
    int headY = y;
    int prevX = snakeTailX[0]; 
    int prevY = snakeTailY[0]; 
    int prev2X, prev2Y;

    switch (sDir) { 
        case LEFT: 
            x--;
            if (x < 0) {x = 0; return;}
            break; 
        case RIGHT: 
            x++; 
            if (x >= width) {x = width-1; return;}
            break; 
        case UP: 
            y--; 
            if (y < 0) {y = 0; return;}
            break; 
        case DOWN: 
            y++; 
            if (y >= height) {y = height-1; return;}
            break; 
    } 

    snakeTailX[0] = headX; 
    snakeTailY[0] = headY; 

    for (int i = 1; i < snakeTailLen; i++) { 
        prev2X = snakeTailX[i]; 
        prev2Y = snakeTailY[i]; 
        snakeTailX[i] = prevX; 
        snakeTailY[i] = prevY; 
        prevX = prev2X; 
        prevY = prev2Y; 
    } 

    // Checks for collision with the tail (o) 
    for (int i = 0; i < snakeTailLen; i++) { 
        if (snakeTailX[i] == x && snakeTailY[i] == y) 
            isGameOver = true; 
    } 

    // Checks for snake's collision with the food (#) 
    if (x == fruitCordX && y == fruitCordY) { 
        playerScore += 10; 
        fruitCordX = rand() % width; 
        fruitCordY = rand() % height; 
        snakeTailLen++; 
    } 
} 

// Function to handle user input 
void UserInput() 
{ 
    int kbhit;
    scanf("%d", &kbhit);
    switch (kbhit) { 
        case 4: 
            sDir = LEFT; 
            break; 
        case 6: 
            sDir = RIGHT; 
            break; 
        case 8: 
            sDir = UP; 
            break; 
        case 2: 
            sDir = DOWN; 
            break; 
        case 0: 
            isGameOver = true; 
            break; 
    } 
} 

// Main function / game looping function 
int main() 
{ 
    char playerName[100]; 
    printf("Enter your name: "); 
    scanf("%s", playerName); 

    srand(time(0)); 
    GameInit(); 
    while (!isGameOver) { 
        GameRender(playerName); 
        UserInput(); 
        UpdateGame(); 
    } 

    return 0; 
}
