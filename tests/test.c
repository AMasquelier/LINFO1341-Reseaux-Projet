#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *receiver(void *arg)
{
    system("./receiver -o tests/file_to_check -n 1 :: 1300");

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    printf("Test 1 avec reseau 'parfait'\n");
    pthread_t t1;
    if (pthread_create(&t1, NULL, receiver, NULL) == -1){
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    sleep(2);

    system("./sender -f tests/file_to_send ::1 1300");

    sleep(2);

    if (pthread_join(t1, NULL)){
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    FILE * pp, *pp2;
    char in[256], out[256];

    if ((pp = popen("md5sum tests/file_to_send", "r")) == NULL)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fscanf(pp, "%s", in);

    sleep(2);

    if ((pp2 = popen("md5sum tests/file_to_check", "r")) == NULL)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fscanf(pp2, "%s", out);

    if (strcmp(in, out) == 0)
    {
        printf("md5 checksum are the same\n");
        system("rm tests/file_to_check");
        return 1;
    }
    printf("md5 checksum are not the same\n");
    system("rm tests/file_to_check");

    return 0;
}
