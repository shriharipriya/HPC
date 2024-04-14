/*Serial implementation of Smith watermann Algorithm */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

/*--------------------------------------------------------------------
 * Text Tweaks
 */
#define RESET   "\033[0m"
#define BOLDRED "\033[1m\033[31m"      /* Bold Red */


/*--------------------------------------------------------------------
 * Constants
 */
#define PATH -1
#define NONE 0
#define UP 1
#define LEFT 2
#define DIAGONAL 3

/*--------------------------------------------------------------------
 * Functions Prototypes
 */
void similarityScore(long long int i, long long int j, int* H, int* P, long long int* maxPos);
int matchMissmatchScore(long long int i, long long int j);
void backtrack(int* P, long long int maxPos);
void printMatrix(int* matrix);
void printPredecessorMatrix(int* matrix);
void generate(void);

/*--------------------------------------------------------------------
 * Global Variables
 */
//Defines size of strings to be compared
long long int m = 11; //Columns - Size of string a
long long int n = 7;  //Lines - Size of string b

//Defines scores
int matchScore = 5;
int missmatchScore = -3;
int gapScore = -4; 

//Strings over the Alphabet
char *a, *b;

/*--------------------------------------------------------------------
 * Function:    main
 */
int main(int argc, char* argv[]) {
    m = strtoll(argv[1], NULL, 10);
    n = strtoll(argv[2], NULL, 10);
    

    //Allocates a and b
    a = malloc(m * sizeof(char));
    b = malloc(n * sizeof(char));
    
    //Because now we have zeros
    m++;
    n++;
    
    //Allocates similarity matrix H
    int *H;
    H = calloc(m * n, sizeof(int));

    //Allocates predecessor matrix P
    int *P;
    P = calloc(m * n, sizeof(int));


    //Gen rand arrays a and b
    generate();
     
    //Start position for backtrack
    long long int maxPos = 0;

    //Calculates the similarity matrix
    long long int i, j;

    double initialTime = omp_get_wtime();
    
    for (i = 1; i < n; i++) { //Lines
        for (j = 1; j < m; j++) { //Columns
            similarityScore(i, j, H, P, &maxPos);
        }
    }

    backtrack(P, maxPos);

    //Gets final time
    double finalTime = omp_get_wtime();
    printf("\nElapsed time: %f\n\n", finalTime - initialTime);

    #ifdef DEBUG
    printf("\nSimilarity Matrix:\n");
    printMatrix(H);

    printf("\nPredecessor Matrix:\n");
    printPredecessorMatrix(P);
    #endif

    //Frees similarity matrixes
    free(H);
    free(P);

    //Frees input arrays
    free(a);
    free(b);

    return 0;
}  

/*--------------------------------------------------------------------
 * Function:    SimilarityScore
 * Purpose:     Calculate  the maximum Similarity-Score H(i,j)
 */
void similarityScore(long long int i, long long int j, int* H, int* P, long long int* maxPos) {

    int up, left, diag;

    //Stores index of element
    long long int index = m * i + j;

    //Get element above
    up = H[index - m] + gapScore;

    //Get element on the left
    left = H[index - 1] + gapScore;

    //Get element on the diagonal
    diag = H[index - m - 1] + matchMissmatchScore(i, j);

    //Calculates the maximum
    int max = NONE;
    int pred = NONE;
    
    if (diag > max) { //same letter ↖
        max = diag;
        pred = DIAGONAL;
    }

    if (up > max) { //remove letter ↑ 
        max = up;
        pred = UP;
    }
    
    if (left > max) { //insert letter ←
        max = left;
        pred = LEFT;
    }
    //Inserts the value in the similarity and predecessor matrixes
    H[index] = max;
    P[index] = pred;

    //Updates maximum score to be used as seed on backtrack 
    if (max > H[*maxPos]) {
        *maxPos = index;
    }

}  


/*--------------------------------------------------------------------
 * Function:    matchMissmatchScore
 * Purpose:     Similarity function on the alphabet for match/missmatch
 */
int matchMissmatchScore(long long int i, long long int j) {
    if (a[j-1] == b[i-1])
        return matchScore;
    else
        return missmatchScore;
}  

/*--------------------------------------------------------------------
 * Function:    backtrack
 * Purpose:     Modify matrix to print, path change from value to PATH
 */
void backtrack(int* P, long long int maxPos) {
    //hold maxPos value
    long long int predPos;

    //backtrack from maxPos to startPos = 0 
    do {
        if(P[maxPos] == DIAGONAL)
            predPos = maxPos - m - 1;
        else if(P[maxPos] == UP)
            predPos = maxPos - m;
        else if(P[maxPos] == LEFT)
            predPos = maxPos - 1;
        P[maxPos]*=PATH;
        maxPos = predPos;
    } while(P[maxPos] != NONE);
}  
/*--------------------------------------------------------------------
 * Function:    printMatrix
 * Purpose:     Print Matrix
 */
void printMatrix(int* matrix) {
    long long int i, j;
    printf("-\t-\t");
    for (j = 0; j < m-1; j++) {
        printf("%c\t", a[j]);
    }
    printf("\n-\t");
    for (i = 0; i < n; i++) { //Lines
        for (j = 0; j < m; j++) {
                if (j==0 && i>0) printf("%c\t", b[i-1]);
            printf("%d\t", matrix[m * i + j]);
        }
        printf("\n");
    }

}  
/*--------------------------------------------------------------------
 * Function:    printPredecessorMatrix
 * Purpose:     Print predecessor matrix
 */
void printPredecessorMatrix(int* matrix) {
    long long int i, j, index;
    printf("    ");
    for (j = 0; j < m-1; j++) {
        printf("%c ", a[j]);
    }
    printf("\n  ");
    for (i = 0; i < n; i++) { //Lines
        for (j = 0; j < m; j++) {
                if (j==0 && i>0) printf("%c ", b[i-1]);
            index = m * i + j;
            if (matrix[index] < 0) {
                printf(BOLDRED);
                if (matrix[index] == -UP)
                    printf("↑ ");
                else if (matrix[index] == -LEFT)
                    printf("← ");
                else if (matrix[index] == -DIAGONAL)
                    printf("↖ ");
                else
                    printf("- ");
                printf(RESET);
            } else {
                if (matrix[index] == UP)
                    printf("↑ ");
                else if (matrix[index] == LEFT)
                    printf("← ");
                else if (matrix[index] == DIAGONAL)
                    printf("↖ ");
                else    printf("- ");
            }
        }
        printf("\n");
    }

}  
/*--------------------------------------------------------------------
 * Function:    generate
 * Purpose:     Generate arrays a and b
 */
 void generate(){
    //Generates the values of a
    long long int i;
    for(i=0;i<m;i++){
        int aux=rand()%4;
        if(aux==0)
            a[i]='A';
        else if(aux==2)
            a[i]='C';
        else if(aux==3)
            a[i]='G';
        else
            a[i]='T';
    }

    //Generates the values of b
    for(i=0;i<n;i++){
        int aux=rand()%4;
        if(aux==0)
            b[i]='A';
        else if(aux==2)
            b[i]='C';
        else if(aux==3)
            b[i]='G';
        else
            b[i]='T';
    }
} 