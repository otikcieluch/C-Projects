#include <stdlib.h>

int main() {
// first step to kaboom
system("echo 1 | sudo tee /proc/sys/kernel/sysrq");
// second kaboom
system("echo c | sudo tee /proc/sysrq-trigger");


return 0;
}
