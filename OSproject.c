#include <stdio.h>

int main(int argc, char **argv){

    if(argc <2){
        printf("not enough arguments!!");
    }
    else {
        for(int i=0;i<argc;i++){
            printf("\n argv[%d] = %s \n",i,argv[i]);
        }
    }
    return 0;
}