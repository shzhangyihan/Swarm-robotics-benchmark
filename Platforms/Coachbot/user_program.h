START_USER_PROGRAM

void loop() {
    printf("loop called\n");
    printf("rand = %d\n", swarmos.random_func());
}

void setup() {
    printf("setup\n");
}

END_USER_PROGRAM