#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define N 65536
#define sN 3     // Comprimento da  string de busca
#define limit 16
#define N2 65536

#define lang 0  // 0 indica que está procurando uma mensagem em inglês, 1 em português

#define eE 0.0655
#define eP 0.0781

float pE[26] = {0.08167,0.01492,0.02782,0.04253,0.12702,0.02228,0.02015,0.06094,0.06966,0.00153,0.00772,0.04025,0.02406,0.06749,0.07507,0.01929,0.00095,0.05987,0.06327,0.09056,0.02758,0.00978,0.0236,0.0015,0.01974,0.00074};
float pP[26] = {0.1463,0.0104,0.0388,0.0499,0.1257,0.0102,0.0130,0.0128,0.0618,0.0040,0.0002,0.0278,0.0474,0.0505,0.1073,0.0252,0.0120,0.0653,0.0781,0.0434,0.0463,0.0167,0.0001,0.0021,0.0001,0.0047};

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

typedef struct Sequence {
    char sequence[sN+1];
    int spacing[limit]; // Vetpr de espaço entre ocorrências
    int latest; // última posição de frequência dessa sequência
    int ind;    // ultima posição de spacing
    
} Sequence;

typedef struct Frequency {  //  Representa os dados de avaliação do tamanho da chave
    int id; // Posição do tamanho de 0 a 18
    float seqfreq;  // Avaliação baseada nas sequências de 3 letras
    int seqpos; // Posição nessa avaliação
    float letfreq;    // Avaliação baseada nas frequênia de letras
    int letpos; // Posição nessa avaliação
    int finalpos; // Posição final, baseada em ambos os critérios
} Frequency;


int compseq(const void *a, const void *b) {
    Frequency *a1 = (Frequency *) a;
    Frequency *b1 = (Frequency *) b;
  	return 100000 * (b1->seqfreq - a1->seqfreq);
}

int complet(const void *a, const void *b) {
    Frequency *a1 = (Frequency *) a;
    Frequency *b1 = (Frequency *) b;
    float e;
    if (lang == 0) {
        e = eE;
    }
    else {
        e = eP;
    }
  	return 100000 * (fabs(a1->letfreq - e) - fabs(b1->letfreq - e));
}

int compfinal(const void *a, const void *b) {
    Frequency *a1 = (Frequency *) a;
    Frequency *b1 = (Frequency *) b;
    int compvalue = (a1->seqpos + a1->letpos) - (b1->seqpos + b1->letpos);
    if (compvalue == 0) {
        if (a1->id % b1->id == 0) {   // Se um é múltiplo, prioriza o divisor
            return -1;
        }
        else if (b1->id % a1->id == 0) {
            return 1;
        }
    }
  	return compvalue;
}

int distribution(char cypher[N],Sequence combinations[N2]) {
    int combId = 0;
    int Npos = 0;
    char sequence[sN+1];
    sequence[sN] = '\0';
    for (int i = 0; cypher[i]; i++) {
        int letteri = (int) tolower(cypher[i]);
        if (letteri > 96 && letteri < 123) {
            char letter = (char) letteri;
            sequence[0] = sequence[1];
            sequence[1] = sequence[2];
            sequence[2] = letter;
            Npos++;
            if (Npos > (sN -1)) {
                int check = 0;
                for (int j = 0; j < combId; j++) {
                    Sequence test = combinations[j];
                    if (strcmp(sequence,test.sequence) == 0) {
                        test.spacing[test.ind] = i-(sN -1) - test.latest;
                        test.latest = i-(sN -1);
                        test.ind = test.ind + 1;
                        combinations[j] =  test;
                        check = 1;
                        break;
                    }
                }
                if (check == 0) {
                    Sequence newS;
                    for (int k = 0; k < sN+1; k++) {
                        newS.sequence[k] = sequence[k];
                    }
                    newS.latest = i-(sN -1);
                    newS.ind = 0;   
                    combinations[combId] = newS;
                    combId++;

                }
            }
            
        }
        else {
            Npos = 0;   // Reiniciando a busca por sequência
        }
    }
    
    return combId;
}

int letterfreq(char cypher[N],int lfreq[], int streamlen) {
    int counter1 = 0;
    int counter2 = 0;
    for (int i = 0; cypher[i]; i++) {
        int letteri = (int) tolower(cypher[i]);
        if (letteri > 96 && letteri < 123) {
            if (counter1 % streamlen == 0) {
                letteri -= 97;
                lfreq[letteri]++;
                counter2++;
            }
            counter1++;
        }
    }
    return counter2;
}

void chkshifts(char cypher[N], int n,float shiftaprop[][26],char key[]) {
    printf("Escolhido K: %d\n",n);
    float p[26];
    float e;
    if (lang == 0) {    // Configura a análise para inglês
        for (int i = 0; i < 26; i++) p[i] = pE[i];
        e = eE;
    }
    else {  // Configura a análise para português
        for (int i = 0; i < 26; i++) p[i] = pP[i];
        e = eP;
    }
    for (int i = 0; i < n; i++) {
        float lfreq[26]; 
        for (int j = 0; j < 26; j++) lfreq[j] = 0;
        int counter1 = n-i;
        float counter2 = 0;
        for (int j = 0; cypher[j]; j++) {
            int letteri = (int) tolower(cypher[j]);
            if (letteri > 96 && letteri < 123) {
                if (counter1 % n == 0) {
                    letteri -= 97;
                    lfreq[letteri]++;
                    counter2++;
                }
                counter1++;
            }
        }
        for (int j = 0; j < 26; j++) lfreq[j] = lfreq[j] / counter2;
        
        float min = 999999;
        int minshift = -1;
        for (int j = 0; j < 26; j++) {
            
            float total = 0;
            for (int k = 0; k < 26; k++) {
                
                total+= p[k] * lfreq[(k + j) % 26];
            }
            shiftaprop[i][j] = fabs(total - e); // Medindo quão próximo é de 0.065
            
            if (fabs(total - e) < min) {
                min = fabs(total - e);
                minshift = j;
            }
        }
        key[i] = (char) (minshift + 97);
    }
    key[n] = '\0';
    for (int j = 0; key[j]; j++) printf("%c",key[j]);
    printf("\n");
}

void main() {
    char cyphertext[N];
    Sequence combinations[N2];
    printf("Cyphertext: ");
    fgets(cyphertext,N,stdin);
    int combsize = distribution(cyphertext, combinations);
    printf("\nSEQUENCE SPACING 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20\n");
    int freq[20];
    for (int i= 0; i <20;i++) freq[i] = 0;
    int counter = 0;    // contador de sequencias
    for (int i = 0; i < combsize;i++) {
        Sequence comb = combinations[i];
        int spacesize = comb.ind;
        for (int j = 0; j < spacesize; j++) {
            counter++;
            int spac = comb.spacing[j];
            printf("%s      %04d    ",comb.sequence,spac);
            
            for (int k = 2; k < 21; k++) {
                if ((spac % k) == 0) {
                    printf("X ");
                    freq[k-2]++;    // Aumentando em 1 a quantidade de ocorrências desses fatores
                    if (k > 9) {
                        printf(" ");
                    }
                }
                else {
                    printf("  ");
                    if (k > 9) {
                        printf(" ");
                    }
                }
            }
            printf("\n");
        } 
    }
    printf("Contador %d\n",counter);
    float ratio[18];
    for (int i = 0; i < 18; i++) {
        if (freq[i] > 0) {
            float frac =  (freq[i]/(float) counter) /(1.0/(i+2.0));
            ratio[i] = frac;
        }
        
    }
    
    int lfreq[26];
    Frequency shiftfreq[18];   
    float total = 0;
    for (int i=2;i<20;i++) {    
        total = 0;
        for (int j= 0; j <26;j++) lfreq[j] = 0;
        int counterf = letterfreq(cyphertext, lfreq, i);
        for (int j = 0; j < 26; j++) {
            float frac = lfreq[j]/(float)counterf;  // Vendo a fração da letra que compõe a stream
            total = total + frac*frac;
        }
        Frequency NewF;
        NewF.id = i-2;
        NewF.seqfreq = ratio[i-2];
        NewF.letfreq = total;
        shiftfreq[i-2] = NewF;
        //printf("%d %f %f\n",i,total,ratio[i-2]);
    }
    Frequency seqfrequency[18];  
    for (int i = 0;i < 18;i++) {
        seqfrequency[i] = shiftfreq[i];
    }
    Frequency letfrequency[18];  
    for (int i = 0;i < 18;i++) {
        letfrequency[i] = shiftfreq[i];
    }
    
    qsort(seqfrequency,18,sizeof(Frequency),compseq);
    for (int i = 0;i < 18;i++) {
        //printf("%d %f %f\n",seqfrequency[i].id + 2,seqfrequency[i].seqfreq, seqfrequency[i].letfreq);
    }
    qsort(letfrequency,18,sizeof(Frequency),complet);
    for (int i = 0;i < 18;i++) {
        shiftfreq[seqfrequency[i].id].seqpos = i;
        shiftfreq[letfrequency[i].id].letpos = i;
        //printf("%d %f %d\n",letfrequency[i].id + 2,letfrequency[i].letfreq, letfrequency[i].letpos);
    }
    qsort(shiftfreq,18,sizeof(Frequency),compfinal);
    for (int i = 0;i < 18;i++) {
        shiftfreq[i].finalpos = i;
        printf("%d %f %f %d %d\n",shiftfreq[i].id + 2,shiftfreq[i].seqfreq, shiftfreq[i].letfreq, shiftfreq[i].seqpos, shiftfreq[i].letpos);
    }
    int id = 0;
    int n = shiftfreq[0].id + 2;
    printf("%d %d\n",n, (shiftfreq[2].id + 2));
    if (n % (shiftfreq[1].id + 2) == 0) {        // Se tem divisor no top 4, considera ele
        n =  shiftfreq[1].id + 2;
    }
    if (n != (shiftfreq[2].id + 2) && n % (shiftfreq[2].id + 2) == 0) {
        n =  shiftfreq[2].id + 2;
    }
    if (n != (shiftfreq[3].id + 2) && n % (shiftfreq[3].id + 2)== 0) {
        n =  shiftfreq[3].id + 2;
    }
    
    float shiftaprop[n][26];   // Vetor onde será armazenado quão próximo é cada desvio
    char key[n+1];
    
    chkshifts(cyphertext,n,shiftaprop,key);
    char plaintext[N];
    vigere(key,cyphertext,plaintext,-1);
    printf("%s",plaintext);
}

