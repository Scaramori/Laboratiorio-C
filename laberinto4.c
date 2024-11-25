#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> 

#define WIDTH 15
#define HEIGHT 15

#define WALL '#'
#define PATH ' '
#define START 'S'
#define PLAYER '@'
#define KEY 'K'
#define EXIT 'E'

#define INITIAL_TIME_LIMIT 60 // en segundos

char maze[HEIGHT][WIDTH];
int playerX, playerY; // Posición del jugador
int hasKey = 0;       // Indica si el jugador tiene la llave
int level = 1;        // Nivel i

// Direcciones de movimiento
int dx[] = {0, 0, 1, -1}; // W, S, D, A
int dy[] = {1, -1, 0, 0}; // W, S, D, A

// Inicializa el laberinto con paredes
void initializeMaze() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            maze[y][x] = WALL;
        }
    }
}

// Imprime el laberinto junto con la posición del jugador y el tiempo restante
void printMaze(double timeRemaining) {
    system("cls"); // Limpia la consola (Windows)
    printf("Nivel: %d\n", level);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == playerX && y == playerY)
                printf("%c", PLAYER);
            else
                printf("%c", maze[y][x]);
        }
        printf("\n");
    }
    printf(hasKey ? "Tienes la llave. ¡Busca la salida (E)!\n" : "Encuentra la llave (K) primero.\n");
    printf("Tiempo restante: %.1f segundos\n", timeRemaining);
}

// Genera el laberinto usando DFS
void generateMaze(int x, int y) {
    maze[y][x] = PATH;
    int directions[] = {0, 1, 2, 3};

    // Mezclar direcciones aleatoriamente
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int temp = directions[i];
        directions[i] = directions[r];
        directions[r] = temp;
    }

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[directions[i]] * 2;
        int ny = y + dy[directions[i]] * 2;

        if (nx > 0 && nx < WIDTH - 1 && ny > 0 && ny < HEIGHT - 1 && maze[ny][nx] == WALL) {
            maze[y + dy[directions[i]]][x + dx[directions[i]]] = PATH;
            generateMaze(nx, ny);
        }
    }
}

// Coloca objetos (inicio, llave, salida)
void placeObject(char object, int *x, int *y) {
    while (1) {
        int px = rand() % WIDTH;
        int py = rand() % HEIGHT;
        if (maze[py][px] == PATH) {
            maze[py][px] = object;
            if (x && y) {
                *x = px;
                *y = py;
            }
            break;
        }
    }
}

// Maneja el movimiento del jugador
int movePlayer(char direction) {
    int dirIndex;

    // Determinar dirección según la tecla presionada
    if (direction == 's') dirIndex = 0; // Abajo
    else if (direction == 'w') dirIndex = 1; // Arriba
    else if (direction == 'd') dirIndex = 2; // Derecha
    else if (direction == 'a') dirIndex = 3; // Izquierda
    else return 0; // 

    // Nueva posición propuesta
    int newX = playerX + dx[dirIndex];
    int newY = playerY + dy[dirIndex];

    // Verificar límites del laberinto
    if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) {
        return 0; // Posición fuera de límites, no mover
    }

    // Verificar si es un muro
    if (maze[newY][newX] == WALL) {
        return 0; // Es un muro, no mover
    }

    // Actualizar posición del jugador
    playerX = newX;
    playerY = newY;

    // Interacciones con objetos
    if (maze[newY][newX] == KEY) {
        hasKey = 1; // Recoge la llave
        maze[newY][newX] = PATH; // Limpia el camino
    } else if (maze[newY][newX] == EXIT) {
        if (hasKey) {
            return 1; // Nivel completado
        } else {
            printf("Necesitas la llave para salir.\n");
        }
    }

    return 0; // Nivel no completado
}


void generateNewLevel() {
    initializeMaze();
    generateMaze(1, 1);

    // Reiniciar estado del jugador
    hasKey = 0;

    // Colocar jugador, llave y salida
    placeObject(START, &playerX, &playerY);
    placeObject(KEY, NULL, NULL);
    placeObject(EXIT, NULL, NULL);
}

void showMenu() {
    system("cls"); // Limpia la consola
    printf("==============================\n");
    printf("      ¡BIENVENIDO A MAZE!     \n");
    printf("==============================\n");
    printf("Instrucciones:\n");
    printf(" - Usa las teclas:\n");
    printf("     W -> Arriba\n");
    printf("     S -> Abajo\n");
    printf("     A -> Izquierda\n");
    printf("     D -> Derecha\n");
    printf(" - Encuentra la llave (K).\n");
    printf(" - Luego, dirígete a la salida (E).\n");
    printf(" - Tienes un tiempo limitado para completar el nivel.\n");
    printf("\nPresiona cualquier tecla para empezar...\n");
    _getch(); // Esperar a que el usuario presione una tecla
}

int main() {
    srand(time(NULL));
    showMenu();

    // Variables para el cronómetro
    double timeLimit = INITIAL_TIME_LIMIT; // Tiempo inicial
    clock_t startTime;

    while (1) {
        // Generar un nuevo nivel
        generateNewLevel();

        // Inicializar cronómetro
        startTime = clock();

        // Bucle principal del nivel
        while (1) {
            double elapsedTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
            double timeRemaining = timeLimit - elapsedTime;

            if (timeRemaining <= 0) {
                printf("¡Se acabó el tiempo! Has perdido en el nivel %d.\n", level);
                return 0; // Termina el juego
            }

            printMaze(timeRemaining);
            char input = _getch(); // Captura el movimiento del jugador

            if (movePlayer(input)) {
                level++; // Incrementa el nivel
                timeLimit *= 0.9; // Opcional: reducir tiempo para el siguiente nivel
                break; // Salir para generar nuevo nivel
            }
        }
    }

    return 0;
}
