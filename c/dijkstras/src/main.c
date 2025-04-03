#define GRAPH_SIZE 50

#include "test_data.h"
#include "uart.h"

typedef struct {
    int distance;
    int index;
} Vertex;

void swap(int heapLookup[], Vertex *x, Vertex *y) {
    // Swap lookup table entries for vertices
    int tempIndex = heapLookup[x->index];
    heapLookup[x->index] = heapLookup[y->index];
    heapLookup[y->index] = tempIndex;

    // swap vertices in the heap
    Vertex temp = *x;
    *x = *y;
    *y = temp;
}

void heapifyDown(Vertex minHeap[], int heapLookup[], int size, int i) {
    int smallest = i;
    int l = i * 2 + 1;
    int r = i * 2 + 2;

    // check if l is smaller than smallest
    if (l < size && minHeap[l].distance < minHeap[smallest].distance) {
        smallest = l;
    }

    if (r < size && minHeap[r].distance < minHeap[smallest].distance) {
        smallest = r;
    }

    if (smallest != i) {
        swap(heapLookup, &minHeap[i], &minHeap[smallest]);

        heapifyDown(minHeap, heapLookup, size, smallest);
    }
}

void heapifyUp(Vertex minHeap[], int heapLookup[], int i) {
    while (i > 0 && minHeap[(i - 1) / 2].distance > minHeap[i].distance) {
        int parent = (i - 1) / 2;

        swap(heapLookup, &minHeap[i], &minHeap[parent]);

        i = parent;
    }
}

void deleteNode(Vertex minHeap[], int heapLookup[], int *size, int i) {
    swap(heapLookup, &minHeap[i], &minHeap[*size - 1]);
    heapLookup[minHeap[*size - 1].index] = -1;
    (*size)--;
    heapifyDown(minHeap, heapLookup, *size, i);
}

Vertex dequeNode(Vertex minHeap[], int heapLookup[], int *size) {
    Vertex value = minHeap[0];
    deleteNode(minHeap, heapLookup, size, 0);
    return value;
}

void insertNode(Vertex minHeap[], int heapLookup[], int *size, Vertex newVertex) {
    minHeap[*size] = newVertex;
    heapLookup[newVertex.index] = *size;
    (*size)++;
    heapifyUp(minHeap, heapLookup, *size - 1);
}

void updateNode(Vertex minHeap[], int heapLookup[], int vertex, int newDistance) {
    int pos = heapLookup[vertex];

    minHeap[pos].distance = newDistance;

    heapifyUp(minHeap, heapLookup, pos);
}

void initEmptyArrayInt(int array[], int size) {
    for (int i = 0; i < size; i++) {
        array[i] = -1;
    }
}

void initEmptyArrayInt0(int array[], int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 0;
    }
}

void dijkstras(int *graph, int size, int source, int distances[], int previous[], Vertex minHeap[], int heapLookup[], int visited[]) {
    // initialize the previous and distance array
    initEmptyArrayInt(previous, size);
    initEmptyArrayInt(distances, size);
    initEmptyArrayInt(heapLookup, size);
    initEmptyArrayInt0(visited, size);
    distances[source] = 0;

    // add the source node to the priority queue
    int heap_size = 0;
    insertNode(minHeap, heapLookup, &heap_size, (Vertex){0, source});

    // always loop through the priority queue
    while (heap_size != 0) {
        Vertex current = dequeNode(minHeap, heapLookup, &heap_size);

        if (visited[current.index]) {
            continue;
        }

        visited[current.index] = 1;

        int current_index = current.index;

        // update distance for adjacent nodes
        for (int v = 0; v < size; v++) {
            if (graph[current_index * size + v] != -1) {
                // get the current weight between the nodes in the edge
                int edge_distance = graph[current_index * size + v];
                // add on the distance to get the total distance
                int alt = distances[current_index] + edge_distance;
                if (alt < distances[v] || distances[v] == -1) {
                    distances[v] = alt;
                    previous[v] = current_index;

                    if (heapLookup[v] != -1) {
                        updateNode(minHeap, heapLookup, v, alt);
                    } else {
                        insertNode(minHeap, heapLookup, &heap_size, (Vertex){alt, v});
                    }
                }
            }
        }
    }
}

void printPath(int previous[], int destination) {
    // for debugging here show the path

    int final = destination;

    uartSendU32(final + 1);
    uartSendString(" -> ");

    while (final != -1) {
        uartSendU32(previous[final] + 1);
        uartSendString(" -> ");
        final = previous[final];
    }
    uartSend('\n');
}

void main(void) __attribute__((section(".main")));
void main() {
    timerInit();
    uart0Init();

    int distances[GRAPH_SIZE];
    int previous[GRAPH_SIZE];
    Vertex minHeap[GRAPH_SIZE];
    int heapLookup[GRAPH_SIZE];
    int visited[GRAPH_SIZE];

    int amountTests = 500;

    for (int i = 0; i < amountTests; i++) {
        for (int i = 0; i < dijkstrasTestDataArraySize; i++) {
            dijkstras((int *)dijkstrasTestDataArray[i].graph,
                      50,
                      dijkstrasTestDataArray[i].source,
                      distances,
                      previous,
                      minHeap,
                      heapLookup,
                      visited);
            printPath(previous, dijkstrasTestDataArray[i].destination);
        }
    }

    uart0Init();
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(readTime());
    uartSendString(" microseconds");
}
