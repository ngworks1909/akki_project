#include<stdio.h> // this is a comment

#define PI 3.14
#define MAX 100
#define MSG "Hello from macro"

/* this is a 
multi-line comment */

int main() {
    int radius = 5;
    float area = PI * radius * radius; // area of a circle
    int values[MAX]; /* define array */

    printf("Hello"); /* inline comment */
    printf("%s", MSG); // print macro message

    return 0;
}
