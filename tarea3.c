#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tdas/list.h"
#include "tdas/extra.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include <string.h>

// Definición de la estructura para el estado del puzzle
typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    List* actions; //Secuencia de movimientos para llegar al estado
} State;

bool esEstadoFinal(State* estado) {
    int objetivo[3][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado->square[i][j] != objetivo[i][j]) {
                return false;
            }
        }
    }
    return true;
}

int distancia_L1(State* state) {
    int ev=0;
    int k=1;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++){
            int val=state->square[i][j];
            if (val==0) continue;
            int ii=(val)/3;
            int jj=(val)%3;
            ev+= abs(ii-i) + abs(jj-j);
        }
    return ev;
}


// Función para imprimir el estado del puzzle
void imprimirEstado(const State *estado) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado->square[i][j] == 0)
                printf("x "); // Imprime un espacio en blanco para el espacio vacío
            else
                printf("%d ", estado->square[i][j]);
        }
        printf("\n");
    }
}

State* transicion(State* actual, int accion) {
    State* nuevo_estado = (State*) malloc(sizeof(State));
    memcpy(nuevo_estado->square, actual->square, 3 * 3 * sizeof(int));
    nuevo_estado->x = actual->x;
    nuevo_estado->y = actual->y;
    nuevo_estado->actions = list_copy(actual->actions);

    int x = actual->x;
    int y = actual->y;
    int nuevo_x = x, nuevo_y = y;

    switch (accion) {
        case 1: // Mover arriba
            nuevo_x = x - 1;
            break;
        case 2: // Mover abajo
            nuevo_x = x + 1;
            break;
        case 3: // Mover izquierda
            nuevo_y = y - 1;
            break;
        case 4: // Mover derecha
            nuevo_y = y + 1;
            break;
        default:
            return NULL;
    }

    if (nuevo_x >= 0 && nuevo_x < 3 && nuevo_y >= 0 && nuevo_y < 3) {
        nuevo_estado->square[x][y] = nuevo_estado->square[nuevo_x][nuevo_y];
        nuevo_estado->square[nuevo_x][nuevo_y] = 0;
        nuevo_estado->x = nuevo_x;
        nuevo_estado->y = nuevo_y;
        int* accion_ptr = malloc(sizeof(int)); // Asignar memoria para la acción
        *accion_ptr = accion; // Almacenar la acción en la memoria asignada
        list_append(nuevo_estado->actions, accion_ptr); // Pasar el puntero a list_append
        return nuevo_estado;
    }

    free(nuevo_estado);
    return NULL;
}

void dfs(State* inicial) {
    Stack* pila = stack_create(NULL);

    stack_push(pila, inicial);

    int iteraciones = 0;

    while (!stack_is_empty(pila)) {
        State* actual = (State*)stack_pop(pila);
        iteraciones++;

        if (esEstadoFinal(actual)) {
            printf("Solución encontrada en %d iteraciones.\n", iteraciones);

            List* acciones = actual->actions;
            for (int i = 0; i < list_size(acciones); i++) {
                int* accion = (int*)list_get(acciones, i);
                imprimirEstado(transicion(inicial, *accion));
            }

            stack_clean(pila);
            return;
        }

        for (int accion = 1; accion <= 4; accion++) {
            State* nuevo_estado = transicion(actual, accion);
            if (nuevo_estado != NULL) {
                stack_push(pila, nuevo_estado);
            }
        }

        free(actual);
    }

    printf("No se encontró una solución.\n");

    stack_clean(pila);
}

void bfs(State* inicial) {
    Queue* cola = queue_create(NULL);
    queue_insert(cola, inicial);

    int iteraciones = 0;

    while (!queue_is_empty(cola)) {
        State* actual = (State*)queue_remove(cola);
        iteraciones++;

        if (esEstadoFinal(actual)) {
            printf("Solución encontrada en %d iteraciones.\n", iteraciones);

            List* acciones = actual->actions;
            for (int i = 0; i < list_size(acciones); i++) {
                int* accion = (int*)list_get(acciones, i);
                imprimirEstado(transicion(inicial, *accion));
            }

            queue_clean(cola);
            return;
        }

        for (int accion = 1; accion <= 4; accion++) {
            State* nuevo_estado = transicion(actual, accion);
            if (nuevo_estado != NULL) {
                queue_insert(cola, nuevo_estado);
            }
        }

        free(actual);
    }

    printf("No se encontró una solución.\n");

    queue_clean(cola);
}


int main() {
    // Estado inicial del puzzle
    State estado_inicial = {
        {{0, 2, 8}, // Primera fila (0 representa espacio vacío)
         {1, 3, 4}, // Segunda fila
         {6, 5, 7}, // Tercera fila
         },  
        0, 1   // Posición del espacio vacío (fila 0, columna 1)
    };
    estado_inicial.actions = list_create();

    // Imprime el estado inicial
    printf("Estado inicial del puzzle:\n");
    imprimirEstado(&estado_inicial);

    printf("Distancia L1:%d\n", distancia_L1(&estado_inicial));

    char opcion;
    do {
        printf("\n***** EJEMPLO MENU ******\n");
        puts("========================================");
        puts("     Escoge método de búsqueda");
        puts("========================================");

        puts("1) Búsqueda en Profundidad");
        puts("2) Buscar en Anchura");
        puts("3) Buscar Mejor Primero");
        puts("4) Salir");

        printf("Ingrese su opción: ");
        scanf(" %c", &opcion);

        switch (opcion) {
        case '1':
          dfs(&estado_inicial);
          break;
        case '2':
          bfs(&estado_inicial);
          break;
        case '3':
          //best_first(estado_inicial);
          break;
        }
        presioneTeclaParaContinuar();
        limpiarPantalla();

  } while (opcion != '4');

  return 0;
}
