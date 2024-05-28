#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tdas/list.h"
#include "tdas/extra.h"
#include "tdas/stack.h"
#include "tdas/queue.h"

// Definición de la estructura para el estado del puzzle
typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    List* actions; //Secuencia de movimientos para llegar al estado
} State;

int is_final_state(State *estado) {
    int estado_final[3][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8}
    };

    // Comparar cada elemento del arreglo square del estado actual con el estado final
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if(estado->square[i][j] != estado_final[i][j]) 
                return 0; // Devolver falso si hay al menos un elemento diferente
        }
    }
    return 1; // Devolver verdadero si todos los elementos coinciden con el estado final
}

bool queue_is_empty(Queue* queue) {
    return list_size(queue) == 0;
}

bool stack_is_empty(Stack* stack) {
    return list_size(stack) == 0;
}

int distancia_L1(State* state) {
    int ev = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int val = state->square[i][j];
            if (val == 0) continue;
            int ii = val / 3;
            int jj = val % 3;
            ev += abs(ii - i) + abs(jj - j);
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
    if (!nuevo_estado) return NULL;

    memcpy(nuevo_estado->square, actual->square, 3 * 3 * sizeof(int));
    nuevo_estado->x = actual->x;
    nuevo_estado->y = actual->y;
    nuevo_estado->actions = list_create();

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
            list_clean(nuevo_estado->actions);
            free(nuevo_estado);
            return NULL;
    }

    if (nuevo_x >= 0 && nuevo_x < 3 && nuevo_y >= 0 && nuevo_y < 3) {
        nuevo_estado->square[x][y] = nuevo_estado->square[nuevo_x][nuevo_y];
        nuevo_estado->square[nuevo_x][nuevo_y] = 0;
        nuevo_estado->x = nuevo_x;
        nuevo_estado->y = nuevo_y;
        int* accion_ptr = malloc(sizeof(int));
        if (!accion_ptr) {
            list_clean(nuevo_estado->actions);
            free(nuevo_estado);
            return NULL;
        }
        *accion_ptr = accion;
        list_append(nuevo_estado->actions, accion_ptr);
        return nuevo_estado;
    }

    list_clean(nuevo_estado->actions);
    free(nuevo_estado);
    return NULL;
}

List* get_adj_states(State* state) {
    List* adj_states = list_create(); // Crear una lista para almacenar los estados adyacentes

    // Iterar sobre todas las acciones posibles (arriba, abajo, izquierda, derecha)
    for (int action = 1; action <= 4; action++) {
        State* new_state = transicion(state, action); // Obtener el nuevo estado resultante de aplicar la acción
        if (new_state != NULL) {
            list_append(adj_states, new_state); // Agregar el nuevo estado a la lista de estados adyacentes
        }
    }

    return adj_states; // Retornar la lista de estados adyacentes
}

void liberarState(State* estado) {
    if (estado) {
        if (estado->actions) {
            for (int i = 0; i < list_size(estado->actions); i++) {
                int* accion = (int*)list_get(estado->actions, i);
                free(accion);
            }
            list_clean(estado->actions);
        }
        free(estado);
    }
}
bool estado_igual(State* estado1, State* estado2) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado1->square[i][j] != estado2->square[i][j]) {
                return false;
            }
        }
    }
    return true;
}

bool estado_visitado(List* visitados, State* nuevo_estado) {
    for (int i = 0; i < list_size(visitados); i++) {
        State* estado_visitado = (State*)list_get(visitados, i);
        if (estado_igual(estado_visitado, nuevo_estado)) {
            return true;
        }
    }
    return false;
}

void dfs(State* inicial, int max_iteraciones) {
    Stack* pila = stack_create(NULL);
    stack_push(pila, inicial);

    List* visitados = list_create();

    int iteraciones = 0;

    while (!stack_is_empty(pila) && iteraciones < max_iteraciones) {
        State* actual = (State*)stack_pop(pila);
        iteraciones++;

        if (is_final_state(actual)) {
            printf("Solución encontrada en %d iteraciones.\n", iteraciones);

            List* acciones = actual->actions;
            State* temp = actual;
            for (int i = 0; i < list_size(acciones); i++) {
                int* accion = (int*)list_get(acciones, i);
                State* nuevo_temp = transicion(temp, *accion);
                if (nuevo_temp) {
                    imprimirEstado(nuevo_temp);
                    liberarState(temp);
                    temp = nuevo_temp;
                }
            }

            liberarState(actual);
            stack_clean(pila);
            list_clean(visitados);
            return;
        }

        list_append(visitados, actual);

        for (int accion = 1; accion <= 4; accion++) {
            State* nuevo_estado = transicion(actual, accion);
            if (nuevo_estado != NULL && !estado_visitado(visitados, nuevo_estado)) {
                stack_push(pila, nuevo_estado);
            } else {
                liberarState(nuevo_estado);
            }
        }
    }

    if (iteraciones >= max_iteraciones) {
        printf("Se alcanzó el límite máximo de iteraciones (%d).\n", max_iteraciones);
    } else {
        printf("No se encontró una solución.\n");
    }
    stack_clean(pila);
    list_clean(visitados);
}

void bfs(State* inicial, int max_iteraciones) {
    Queue* cola = queue_create(NULL);
    queue_insert(cola, inicial);

    List* visitados = list_create();

    int iteraciones = 0;

    while (!queue_is_empty(cola) && iteraciones < max_iteraciones) {
        State* actual = (State*)queue_remove(cola);
        iteraciones++;

        if (is_final_state(actual)) {
            printf("Solución encontrada en %d iteraciones.\n", iteraciones);

            List* acciones = actual->actions;
            State* temp = actual;
            for (int i = 0; i < list_size(acciones); i++) {
                int* accion = (int*)list_get(acciones, i);
                State* nuevo_temp = transicion(temp, *accion);
                if (nuevo_temp) {
                    imprimirEstado(nuevo_temp);
                    liberarState(temp);
                    temp = nuevo_temp;
                }
            }

            liberarState(actual);
            queue_clean(cola);
            list_clean(visitados);
            return;
        }

        list_append(visitados, actual);

        for (int accion = 1; accion <= 4; accion++) {
            State* nuevo_estado = transicion(actual, accion);
            if (nuevo_estado != NULL && !estado_visitado(visitados, nuevo_estado)) {
                queue_insert(cola, nuevo_estado);
            } else {
                liberarState(nuevo_estado);
            }
        }
    }

    if (iteraciones >= max_iteraciones) {
        printf("Se alcanzó el límite máximo de iteraciones (%d).\n", max_iteraciones);
    } else {
        printf("No se encontró una solución.\n");
    }
    queue_clean(cola);
    list_clean(visitados);
}

int main() {
    // Estado inicial del puzzle
    State* estado_inicial = (State*) malloc(sizeof(State));
    if (!estado_inicial) {
        printf("Error al asignar memoria para el estado inicial\n");
        return 1;
    }
    int init_square[3][3] = {
        {0, 2, 8}, // Primera fila (0 representa espacio vacío)
        {1, 3, 4}, // Segunda fila
        {6, 5, 7}  // Tercera fila
    };
    memcpy(estado_inicial->square, init_square, 3 * 3 * sizeof(int));
    estado_inicial->x = 0;
    estado_inicial->y = 0;
    estado_inicial->actions = list_create();

    // Imprime el estado inicial
    printf("Estado inicial del puzzle:\n");
    imprimirEstado(estado_inicial);

    printf("Distancia L1: %d\n", distancia_L1(estado_inicial));

    char opcion;
    do {
        printf("\n***** EJEMPLO MENU ******\n");
        puts("========================================");
        puts("     Escoge método de búsqueda");
        puts("========================================");

        puts("1) Búsqueda en Profundidad");
        puts("2) Búsqueda en Anchura");
        puts("3) Buscar Mejor Primero");
        puts("4) Salir");

        printf("Opcion: ");
        scanf(" %c", &opcion);

        switch (opcion) {
            case '1':
                dfs(estado_inicial, 3000);
                break;
            case '2':
                bfs(estado_inicial, 10000);
                break;
            case '3':
                //best_first(estado_inicial);
                break;
            }
            presioneTeclaParaContinuar();
            limpiarPantalla();
        } while (opcion != '4');
    
    liberarState(estado_inicial); // Liberar el estado inicial

    return 0;
}