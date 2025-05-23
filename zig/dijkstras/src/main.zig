const io = @import("io");
const uart = @import("uart");
const testData = @import("test_data");
const GRAPH_SIZE = 50;

const Vertex = struct { distance: i32, index: usize };

fn swap(heapLookup: []i32, x: *Vertex, y: *Vertex) void {
    // swap index in loopup table
    const tempIndex: i32 = heapLookup[(x.*).index];
    heapLookup[(x.*).index] = heapLookup[(y.*).index];
    heapLookup[(y.*).index] = tempIndex;

    // swap vertices in the heap
    const tempVertex: Vertex = x.*;
    x.* = y.*;
    y.* = tempVertex;
}

fn heapifyDown(minHeap: []Vertex, heapLookup: []i32, size: i32, i: i32) void {
    var smallest: usize = @intCast(i);
    const l: usize = @intCast(i * 2 + 1);
    const r: usize = @intCast(i * 2 + 2);

    //check if l is smaller than smallest
    if (l < size and minHeap[l].distance < minHeap[smallest].distance) {
        smallest = l;
    }

    if (r < size and minHeap[r].distance < minHeap[smallest].distance) {
        smallest = r;
    }

    if (smallest != i) {
        swap(heapLookup, &minHeap[@intCast(i)], &minHeap[smallest]);

        heapifyDown(minHeap, heapLookup, size, @intCast(smallest));
    }
}

fn heapifyUp(minHeap: []Vertex, heapLookup: []i32, i: i32) void {
    var iCopy: i32 = i;
    while (iCopy > 0 and minHeap[@intCast(@divTrunc(iCopy - 1, 2))].distance > minHeap[@intCast(iCopy)].distance) {
        const parent: usize = @intCast(@divTrunc(iCopy - 1, 2));

        swap(heapLookup, &minHeap[@intCast(iCopy)], &minHeap[parent]);

        iCopy = @intCast(parent);
    }
}

fn deleteNode(minHeap: []Vertex, heapLookup: []i32, size: *i32, i: i32) void {
    swap(heapLookup, &minHeap[@intCast(i)], &minHeap[@intCast(size.* - 1)]);
    heapLookup[minHeap[@intCast(size.* - 1)].index] = -1;
    size.* -= 1;
    heapifyDown(minHeap, heapLookup, size.*, i);
}

fn dequeNode(minHeap: []Vertex, heapLookup: []i32, size: *i32) Vertex {
    const value: Vertex = minHeap[0];
    deleteNode(minHeap, heapLookup, size, 0);
    return value;
}

fn insertNode(minHeap: []Vertex, heapLookup: []i32, size: *i32, newVertex: Vertex) void {
    minHeap[@intCast(size.*)] = newVertex;
    heapLookup[newVertex.index] = @intCast(size.*);
    size.* += 1;
    heapifyUp(minHeap, heapLookup, size.* - 1);
}

fn updateNode(minHeap: []Vertex, heapLookup: []i32, vertex: usize, newDistance: i32) void {
    const pos: i32 = heapLookup[vertex];
    minHeap[@intCast(pos)].distance = newDistance;
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

fn dijkstras(graph: *const [GRAPH_SIZE][GRAPH_SIZE]i32, size: usize, source: usize, distances: []i32, previous: []i32, minHeap: []Vertex, heapLookup: []i32, visited: []i32) void {
    // initalize the previous and distance array
    initEmptyArrayInt(distances);
    initEmptyArrayInt(previous);
    initEmptyArrayInt(heapLookup);
    initEmptyArrayInt0(visited);
    distances[source] = 0;

    // add the source node to the priority queue
    var heapSize: i32 = 0;
    insertNode(minHeap, heapLookup, &heapSize, Vertex{ .distance = 0, .index = source });

    // always loop through the priority queue
    while (heapSize != 0) {
        const current: Vertex = dequeNode(minHeap, heapLookup, &heapSize);

        if (visited[current.index] == 1) {
            continue;
        }

        visited[current.index] = 1;

        const currentIndex: usize = current.index;

        // update distance for adjacent nodes
        for (0..size) |v| {
            if (graph[currentIndex][v] != -1) {
                // get the current weight between the nodes in the edge
                const edgeDistance: i32 = graph[currentIndex][v];
                // add on the distance to get the total distance
                const alt: i32 = distances[currentIndex] + edgeDistance;
                if (alt < distances[v] or distances[v] == -1) {
                    distances[v] = alt;
                    previous[v] = @intCast(currentIndex);
                    if (heapLookup[v] != -1) {
                        updateNode(minHeap, heapLookup, v, alt);
                    } else {
                        insertNode(minHeap, heapLookup, &heapSize, Vertex{ .distance = alt, .index = v });
                    }
                }
            }
        }
    }
}

fn printPath(previousArray: []i32, destination: usize) void {
    // for debugging here show the path

    var final: i32 = @intCast(destination);
    uart.uartSendString("\r\n");
    uart.uartSendU32(@intCast(final + 1));
    uart.uartSendString(" -> ");

    while (final != -1) {
        uart.uartSendU32(@intCast(previousArray[@intCast(final)] + 1));
        uart.uartSendString(" -> ");
        final = previousArray[@intCast(final)];
    }

    uart.uartSend('\n');
}

export fn main() linksection(".main") void {
    uart.resetIOBank0();
    io.gpioPin9Fsel();

    io.timerInit();
    const initTime: u64 = io.readTime();

    var dummyArray: [50]i32 = undefined;
    const dummySink: *volatile [50]i32 = &dummyArray;

    var distances: [GRAPH_SIZE]i32 = undefined;
    var previous: [GRAPH_SIZE]i32 = undefined;
    var minHeap: [GRAPH_SIZE]Vertex = undefined;
    var heapLookup: [GRAPH_SIZE]i32 = undefined;
    var visited: [GRAPH_SIZE]i32 = undefined;

    const ammountTest: u32 = 25;

    io.gpioPin9High();

    for (0..ammountTest) |_| {
        for (&testData.dijkstrasTestDataArray) |*testGraph| {
            dijkstras(&testGraph.graph, testGraph.size, testGraph.source, &distances, &previous, &minHeap, &heapLookup, &visited);

            for (0..50) |x| {
                dummySink.*[x] = previous[x];
            }
        }
    }

    io.gpioPin9Low();

    const finishTime: u64 = io.readTime() - initTime;

    uart.uart0Init();
    uart.uartSendString("\r\nZig dijkstras : ");
    uart.uartSendU32(ammountTest);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(finishTime));
    uart.uartSendString(" microseconds");
}
