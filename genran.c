#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int main()
{
    FILE *fp = fopen("input.txt","w");
    long long int size, length = 0;
    int counter = 0;
    srand(time(NULL));

    scanf("%lld",&size);
    size *= 1048576 / 1024;    //轉換成byte

    while(length <= size) {
        for(counter = 0; counter < 100; ++counter)
            fprintf(fp, "%d\n", rand());

        fseek(fp, 0 ,SEEK_END);    //計算檔案大小
        length = ftell(fp);
    }
    fclose(fp);
    printf("%d\n",RAND_MAX);
    printf("over\n");
    return 0;
}

