#include<stdio.h>
#include<string.h>

int main(){
    char str[100];

    printf("enter a string:");
    scanf("%s",str);

 

    int len=strlen(str);

    if(len < 2){
        printf("string is not valid,plase enter a new string");
    }

    if(str[len-1]!='b'|| str[len-2]!='b'){
        printf("Invalid string");
        return 0;
    }


    for(int i=0;i<len-2;i++){
         if(str[i]!='a'){
            printf("Invalid string");
            return 0;
         }
    }
    printf("Valid string");
    return 0;
}
