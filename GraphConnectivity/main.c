#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Глобальные переменные
int V, E;               // количество вершин и рёбер
int **adjMatrix = NULL; // матрица смежности

// ------------------------------------------------------------
// 1. Чтение матрицы инцидентности из файла
// ------------------------------------------------------------
void readIncidenceMatrix(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    fscanf(f, "%d %d", &V, &E);
    printf("Vertices: %d, Edges: %d\n", V, E);

    // Выделение памяти под матрицу инцидентности (временная)
    int **incMatrix = (int **)malloc(V * sizeof(int *));
    for (int i = 0; i < V; i++) {
        incMatrix[i] = (int *)malloc(E * sizeof(int));
    }

    // Чтение матрицы инцидентности
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < E; j++) {
            fscanf(f, "%d", &incMatrix[i][j]);
        }
    }
    fclose(f);

    // Вывод матрицы инцидентности в консоль (с нумерацией от 1)
    printf("\n=== Incidence Matrix from file ===\n");
    printf("    ");
    for (int j = 0; j < E; j++) printf("e%-2d ", j + 1);
    printf("\n");
    for (int i = 0; i < V; i++) {
        printf("v%-2d ", i + 1);
        for (int j = 0; j < E; j++) {
            printf("%-3d ", incMatrix[i][j]);
        }
        printf("\n");
    }

    // --------------------------------------------------------
    // 2. Преобразование матрицы инцидентности в матрицу смежности
    // --------------------------------------------------------
    adjMatrix = (int **)malloc(V * sizeof(int *));
    for (int i = 0; i < V; i++) {
        adjMatrix[i] = (int *)calloc(V, sizeof(int));
    }

    // Для каждого ребра находим две инцидентные вершины
    for (int j = 0; j < E; j++) {
        int u = -1, v = -1;
        for (int i = 0; i < V; i++) {
            if (incMatrix[i][j] == 1) {
                if (u == -1) u = i;
                else v = i;
            }
        }
        if (u != -1 && v != -1) {
            adjMatrix[u][v] = 1;
            adjMatrix[v][u] = 1;
        } else if (u != -1) {
            adjMatrix[u][u] = 1;
        }
    }

    // Освобождение временной матрицы
    for (int i = 0; i < V; i++) free(incMatrix[i]);
    free(incMatrix);

    // Вывод матрицы смежности (с нумерацией от 1)
    printf("\n=== Adjacency Matrix (stored in memory) ===\n");
    printf("    ");
    for (int j = 0; j < V; j++) printf("v%-2d ", j + 1);
    printf("\n");
    for (int i = 0; i < V; i++) {
        printf("v%-2d ", i + 1);
        for (int j = 0; j < V; j++) {
            printf("%-3d ", adjMatrix[i][j]);
        }
        printf("\n");
    }
}

// ------------------------------------------------------------
// 3. Проверка связности графа (обход в глубину)
// ------------------------------------------------------------
void dfs(int v, bool *visited) {
    visited[v] = true;
    for (int i = 0; i < V; i++) {
        if (adjMatrix[v][i] && !visited[i]) {
            dfs(i, visited);
        }
    }
}

bool isConnected() {
    bool *visited = (bool *)calloc(V, sizeof(bool));
    dfs(0, visited);
    for (int i = 0; i < V; i++) {
        if (!visited[i]) {
            free(visited);
            return false;
        }
    }
    free(visited);
    return true;
}

// ------------------------------------------------------------
// 4. Генерация DOT-файла (нумерация вершин с 1)
// ------------------------------------------------------------
void generateDot(const char *dotFilename) {
    FILE *f = fopen(dotFilename, "w");
    if (!f) {
        printf("Error creating DOT file\n");
        return;
    }

    fprintf(f, "graph G {\n");
    fprintf(f, "    node [shape=circle];\n");

    for (int i = 0; i < V; i++) {
        for (int j = i; j < V; j++) {
            if (adjMatrix[i][j]) {
                fprintf(f, "    %d -- %d;\n", i + 1, j + 1);
            }
        }
    }

    fprintf(f, "}\n");
    fclose(f);
    printf("\nDOT file created: %s\n", dotFilename);
}

// ------------------------------------------------------------
// 5. Запуск Graphviz
// ------------------------------------------------------------
void renderGraph(const char *dotFilename, const char *outputFilename) {
    char command[512];
    sprintf(command, "dot -Tpng \"%s\" -o \"%s\"", dotFilename, outputFilename);
    int result = system(command);
    if (result == 0) {
        printf("Graph image saved: %s\n", outputFilename);
    } else {
        printf("Graphviz error. Open DOT file online: https://dreampuf.github.io/GraphvizOnline/\n");
    }
}

// ------------------------------------------------------------
// 6. Освобождение памяти
// ------------------------------------------------------------
void freeGraph() {
    if (adjMatrix) {
        for (int i = 0; i < V; i++) {
            free(adjMatrix[i]);
        }
        free(adjMatrix);
    }
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {
    const char *inputFile = "C:\\dz2\\graph.txt";
    const char *dotFile = "C:\\dz2\\graph.dot";
    const char *outputImage = "C:\\dz2\\graph.png";

    // 1. Чтение и преобразование
    readIncidenceMatrix(inputFile);

    // 2. Проверка связности
    printf("\n=== Connectivity check ===\n");
    if (isConnected()) {
        printf("Graph is CONNECTED\n");
    } else {
        printf("Graph is DISCONNECTED\n");
    }

    // 3. Визуализация
    generateDot(dotFile);
    renderGraph(dotFile, outputImage);

    // 4. Очистка памяти
    freeGraph();

    printf("\nPress Enter to exit...");
    getchar();
    getchar();
    return 0;
}
