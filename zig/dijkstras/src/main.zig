const io = @import("io");
const uart = @import("uart");

const Vertex = struct { distance: i32, index: usize };

fn swap(heapLookup: []i32, x: *Vertex, y: *Vertex) void {
    // swap index in loopup table
    const tempIndex: i32 = heapLookup[(x.*).index];
    heapLookup[(x.*).index] = heapLookup[(y.*).index];
    heapLookup[(y.*).index] = tempIndex;

    // swap vertices in the heap
    const tempVertex: Vertex = *x;
    x.* = y.*;
    y.* = tempVertex;
}

fn heapifyDown(minHeap: []Vertex, heapLookup: []Vertex, size: i32, i: i32) void {
    const smallest: i32 = i;
    const l: i32 = i * 2 + 1;
    const r: i32 = i * 2 + 2;

    //check if l is smaller than smallest
    if (l < size and minHeap[l].distance < minHeap[smallest].distance) {
        smallest = l;
    }

    if (r < size and minHeap[r].distance < minHeap[smallest].distance) {
        smallest = r;
    }

    if (smallest != i) {
        swap(heapLookup, &minHeap[i], &minHeap[smallest]);

        heapifyDown(minHeap, heapLookup, size, smallest);
    }
}

fn heapifyUp(minHeap: []Vertex, heapLookup: []i32, i: i32) void {
    while (i > 0 and minHeap[(i - 1) / 2].distance > minHeap[i].distance) {
        const parent: i32 = (i - 1) / 2;

        swap(heapLookup, &minHeap[i], &minHeap[parent]);

        i = parent;
    }
}

fn deleteNode(minHeap: []Vertex, heapLookup: []i32, size: *i32, i: i32) void {
    swap(heapLookup, &minHeap[i], &minHeap[size.* - 1]);
    heapLookup[minHeap[size.* - 1].index] = -1;
    size.* -= 1;
    heapifyDown(minHeap, heapLookup, size.*, i);
}

fn dequeNode(minHeap: []Vertex, heapLookup: []i32, size: *i32) void {
    const value: Vertex = minHeap[0];
    deleteNode(minHeap, heapLookup, size, 0);
    return value;
}

fn insertNode(minHeap: []Vertex, heapLookup: []i32, size: *i32, newVertex: Vertex) void {
    minHeap[size.*] = newVertex;
    heapLookup[newVertex.index] = size.*;
    size.* += 1;
    heapifyUp(minHeap, heapLookup, size.* - 1);
}

fn updateNode(minHeap: []Vertex, heapLookup: []i32, vertex: i32, newDistance: i32) void {
    const pos: i32 = heapLookup[vertex];
    minHeap[pos].distance = newDistance;
    heapifyUp(minHeap, heapLookup, pos);
}

fn initEmptyArrayInt(array: []i32) void {
    for (array) |*element| {
        element.* = -1;
    }
}

fn initEmptyArrayInt0(array: []i32) void {
    for (array) |*element| {
        element.* = 0;
    }
}

fn dijkstras(graph: *i32, size: i32, source: i32, destination: i32, distances: []i32, previous: []i32, minHeap: []Vertex, heapLookup: []i32, visited: []i32) void {
    // initalize the previous and distance array
    initEmptyArrayInt(previous, size);
    initEmptyArrayInt(distances, size);
    initEmptyArrayInt(heapLookup, size);
    initEmptyArrayInt0(visited, size);
    distances[source] = 0;

    // add the source node to the priority queue
    var heapSize: i32 = 0;
    insertNode(minHeap, heapLookup, &heapSize, Vertex{ .distance = 0, .id = source});

    // always loop through the priority queue
    while (heapSize != 0) {
        const current: Vertex = dequeNode(minHeap, heapLookup, &heapSize);

        if (visited[current.index]) {
            continue;
        }

        visited[current.index] = 1;

        const currentIndex: i32 = current.index;

        // update distance for adjacent nodes
        for (0..size) |v| {
            if (graph[currentIndex * size + v] != -1) {
                // get the current weight between the nodes in the edge
                const edgeDistance: i32 = graph[currentIndex * size + v];
                // add on the distance to get the total distance
                const alt: i32 = distances[currentIndex] + edgeDistance;
                if (alt < distances[v] or distances[v] == -1) {
                    distances[v] = alt;
                    previous[v] = currentIndex;
                } else {
                    insertNode(minHeap, heapLookup, &heapSize, Vertex{ .distance = alt, .index = v});
                }
            }
        }
    }
}

//
//     // for debugging here show the path
//
//     /*uart0Init();*/
//     /**/
//     /*int final = destination;*/
//     /**/
//     /*uartSendU32(final + 1);*/
//     /*uartSendString(" -> ");*/
//     /**/
//     /*while (final != -1) {*/
//     /*    uartSendU32(previous[final] + 1);*/
//     /*    uartSendString(" -> ");*/
//     /*    final = previous[final];*/
//     /*}*/
//     /*uartSend('\n');*/
// }

export fn main() linksection(".main") void {
    uart.uart0Init();
    io.timerInit();
}
