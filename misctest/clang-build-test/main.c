
void main(void) __attribute__((section(".main")));
void main() {
    int x = 0;
    for (int i = 0; i < 100; i++) {
        x += i;
    }
}
