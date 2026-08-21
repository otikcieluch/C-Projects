#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

void disable_oom_kill(void) {
    FILE *f = fopen("/proc/self/oom_score_adj", "w");
    if (f) {
        fprintf(f, "-1000");
        fclose(f);
    }
}
int main(void) {
    disable_oom_kill();
    FILE *check = fopen("/proc/self/oom_score_adj", "r");
    if (check) {
        char buf[16];
        fgets(buf, sizeof(buf), check);
        printf("oom_score_adj = %s", buf);
        fclose(check);
    }
    size_t bytes = 1000000ULL * 1024 * 1024 * 10;

    void *p = mmap(NULL, bytes, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    memset(p, 1, bytes);
    printf("touched all pages\n");

    pause();
    return 0;
}
