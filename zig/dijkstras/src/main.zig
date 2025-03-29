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

export fn main() linksection(".main") void {
    uart.uart0Init();
    io.timerInit();
}
