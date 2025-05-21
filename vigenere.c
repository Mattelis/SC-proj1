#include <stdio.h>
#include <ctype.h>

#define N 65536

void vigere(char key[], char plaintext[],char cyphertext[], int S) {
    int size = 0;
    for (int i = 0; (int) key[i] != 10 && key[i] ; i++) {
        key[i]  = tolower(key[i]);
        size++;
    }
    key[size] = 0;  // Marcando como fim da chave
    
    int vigcounter = 0;
    int count = 0;
    for (int i = 0; (int) plaintext[i] != 10 && plaintext[i]; i++) {
        count++;
        int keyvalue = ((int) key[vigcounter % size]) - 97;
        
        int plainvalue  = (int) tolower(plaintext[i]);
        if (plainvalue > 96 && plainvalue < 123) {
            vigcounter++;
            plainvalue -= 97;
            plainvalue = (plainvalue + keyvalue*S) % 26;
            if (plainvalue < 0) {
                plainvalue = 26 - (-1 * plainvalue);
            }
            cyphertext[i] = (char) (plainvalue + 97);
            
        }
        else {
            cyphertext[i] = plaintext[i];
        }
    }
    cyphertext[count] = 0;  // Marcando como fim do cyphertext
    return;
}


void main() {
    char plaintext[N];
    char key[256];
    
    char name[50];
  
    // Reading a string
    
    printf("Your key is: ");
    fgets(key,256,stdin);
    printf("Your text is: ");
    fgets(plaintext,1024,stdin);
    int size = 0;
    for (int i = 0; (int) key[i] != 10; i++) {
        size++;
    }
    char cyphertext[N];
    vigere(key,plaintext,cyphertext,1); // S = 1 indica que é para somar o valor do elemento da chave
    printf("Key: %s\nCyphertext: %s \n",key,cyphertext);
    char plaintext2[N];
    vigere(key,cyphertext,plaintext2,-1); // S = -1 indica que é para subtrair o valor do elemento da chave
    printf("Plaintext: %s",plaintext2);
}