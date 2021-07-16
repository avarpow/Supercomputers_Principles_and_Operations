#include<stdio.h>
int main(){
    for(int i=0;i<16;i++){
        printf("         if (a[j+%d] < a[i] || (a[j+%d] == a[i] && j+%d < i)) count++;\n",i,i,i);
    }
}