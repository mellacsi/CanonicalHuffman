#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Simone Mellace
 * Gestione richiesta da linea di comando e Huffman canonico
 */

typedef struct canonicalNode {
	int numBitCodifica;
	int caracter;
	int bits[256];
} CanonicalNode;

typedef struct node {
	int freq;
	int caracter;
	struct node *right;
	struct node *left;
} Node;

void printbinario(int bits[]) {
	int i = 0;
	for (i = 0; i < 8; i++) {
		printf("%d ", bits[i]);
	}
	printf("\n");
}

void printCodLength(CanonicalNode** codifica) {
	int i;
	printf("\n");
	for (i = 0; i < 256; i++) {
		printf("%c: %d\n", codifica[i]->caracter, codifica[i]->numBitCodifica);
	}
}
void printBits(CanonicalNode** bitUsati) {
	int j;
	int i = 255;
	for (j = 0; j < 256; j++) {
		printf("%c: ", bitUsati[j]->caracter);
		for (i = 0; i < 256; i++) {
			if (bitUsati[j]->bits[i] != 2) {
				printf("%d ", bitUsati[j]->bits[i]);
			}
		}
		printf("\n");
	}
}
void stampaAlbero(Node* head) {
	if (head == NULL) {
		return;
	}
	if (head->freq == 1) {
		printf("%c\n", head->caracter);
	}
	stampaAlbero(head->left);
	fprintf(stderr, "0");
	stampaAlbero(head->right);
	fprintf(stderr, "1");
}

void initializeBits(int bits[]) {
	int i = 0;
	for (i = 0; i < 8; i++) {
		bits[i] = 0;
	}
}

void initialize(Node tree[]) {
	int i;
	for (i = 0; i < 256; i++) {
		//inizializzo frequenze a 0
		//inizializzo caratteri a byte da 0 a 255 (256 bytes)
		tree[i].freq = 0;
		tree[i].caracter = i;
		tree[i].right = NULL;
		tree[i].left = NULL;
	}
}
void initializePtr(CanonicalNode** codifiche) {
	int i;
	int j;
	for (i = 0; i < 256; i++) {
		//inizializzo lunghezze a 0
		//inizializzo caratteri a byte da 0 a 255 (256 bytes)
		codifiche[i] = (CanonicalNode*) malloc(sizeof(CanonicalNode));
		codifiche[i]->numBitCodifica = 0;
		codifiche[i]->caracter = i;

		for (j = 0; j <= 255; j++) {
			codifiche[i]->bits[j] = 2;
		}
	}
}

int initializeFirst(CanonicalNode** bitUsati) {
// trova il primo valore con codifica, lo inizializza con gli zeri torna la sua posizione
	int j = 0;
	while (j < 256 && bitUsati[j]->numBitCodifica == 0) {
		j++;
	}

	if (j == 256) {
		//nessun elemento nel file
		return -1;
	} else {
		int i;
		for (i = 0; i < bitUsati[j]->numBitCodifica; i++) {
			bitUsati[j]->bits[255 - i] = 0;
		}
		return j;
	}
}

int compare(const void * a, const void * b) {
	//due metodi per accedere al campo puntato freq
	return ((*(Node*) b).freq - ((Node*) a)->freq);
}

int reorderCod(const void * a, const void * b) {
	return ((*(CanonicalNode**) a)->caracter - (*(CanonicalNode**) b)->caracter);
}

int comparePtr(const void * a, const void * b) {
	return ((*(Node**) b)->freq - (*(Node**) a)->freq);
}

int compareCodifica(const void* a, const void* b) {
	CanonicalNode* nodeA = *(CanonicalNode**) a;
	CanonicalNode* nodeB = *(CanonicalNode**) b;

	if (nodeA->numBitCodifica == nodeB->numBitCodifica) {
		return (int) (nodeA->caracter) - (int) (nodeB->caracter);
	} else {
		return nodeA->numBitCodifica - nodeB->numBitCodifica;
	}
}

void incrementa(int bits[]) {
	int i = 255;
//faccio incrementare di uno rispetto alla codifica precedente
	int resto = 1;
	while (i >= 0) {
		if (resto == 1) {
			if (bits[i] == 0 || bits[i] == 2) {
				bits[i] = 1;
				resto = 0;
			} else {
				bits[i] = 0;
				resto = 1;
			}
		}
		i--;
	}
}

void shifta(int bits[], int numZeri) {
	//shifto vecchia codifica
	int tmp[256];
	int i = 255;

	if (numZeri > 0) {
		//copio la codifica attuale in tmp
		while (i >= 0) {
			if (bits[i] != 2) {
				tmp[i] = bits[i];
			} else {
				tmp[i] = 2;
			}
			i--;
		}
		//shifto valori
		i = 255;
		while (i >= 0) {
			if ((255 - i) < numZeri) {
				bits[i] = 0;
			} else {
				bits[i] = tmp[i + numZeri];
			}
			i--;
		}
	}
}

void complete(CanonicalNode** bitUsati, int indice, int numZeri) {
	//trovo la codifica attuale
	int i = 255;
	int tmp[256];
//bit più significativi sono a dx
//copia vecchio
	while (i >= 0) {
		if (bitUsati[indice - 1]->bits[i] != 2) {
			tmp[i] = bitUsati[indice - 1]->bits[i];
		} else {
			tmp[i] = 2;
		}
		i--;
	}
	incrementa(tmp);
	shifta(tmp, numZeri);
	i = 255;
	while (i >= 0 && tmp[i] != 2) {
		bitUsati[indice]->bits[i] = tmp[i];
		i--;
	}
}

void trovaCodifiche(CanonicalNode** bitRicevuti) {
	// trova le varie codifiche grazie alle lunghezze e le salva
	int i;
	int indiceFirst = initializeFirst(bitRicevuti);
//bitRicevuti contiene le lunghezze delle codifiche in ordine crescente!
	if (indiceFirst != -1) {
		//il primo lo abbiamo già inizializzato
		for (i = (indiceFirst + 1); i < 256; i++) {
			complete(bitRicevuti, i,
					(bitRicevuti[i]->numBitCodifica
							- bitRicevuti[i - 1]->numBitCodifica));
		}
	}
	//printBits(bitRicevuti);
}

void canonico(Node* head, CanonicalNode** codifica, int n) {
	//foglia
	if (head->left == NULL) {
		codifica[head->caracter]->caracter = head->caracter;
		codifica[head->caracter]->numBitCodifica = n;
	} else {
		//non foglia
		canonico(head->left, codifica, n + 1);
		canonico(head->right, codifica, n + 1);
	}
}

int riempitoFinoA(int buffer[8]) {
	//torna -1 se buffer pieno altrimenti l'indice di completamento
	int i = 7;
	for (i = 7; i >= 0; i--) {
		if (buffer[i] == 2) {
			return i;
		}
	}
	return -1;
}

int powOf(int pow) {
	//legge una data potenza in base 2
	int i;
	int res = 1;

	if (pow == 0) {
		return 1;
	} else {
		for (i = 0; i < pow; i++) {
			res = res * 2;
		}
		return res;
	}
}

int leggiNumeri8(int buffer[8]) {
	//legge in decimale il contenuto del buffer
	int j;
	int somma = 0;

	for (j = 0; j < 8; j++) {
		if (buffer[7 - j] == 1) {
			somma += powOf(j);
		} else if (buffer[7 - j] == 2) {
			return -1;
		}
	}
	return somma;
}

void cercaLunghezze(char* argv[], CanonicalNode** bitRicevuti) {
	//leggo file e trovo varie lunghezze
	FILE* file = fopen(argv[2], "r");
	if (file == NULL) {
		perror("File non trovato");
		exit(1);
	}
	int t = fgetc(file);
	int i;
	for (i = 0; i < 256; i++) {
		bitRicevuti[i]->numBitCodifica = t;
		t = fgetc(file);
	}
	fclose(file);
}

void convABinario(int n, int bits[]) {
	//trasformo da decimale a formato binario
	//metto il risultato nel buffer
	int curry;
	int i;
	for (i = 0; i < 8; i++) {
		bits[i] = 0;
	}
	i = 7;
	while (n != 0) {
		curry = n % 2;
		n = n / 2;
		bits[i] = curry;
		i--;
	}
}

void saveTree(CanonicalNode** bitUsati, char* argv[]) {
	FILE* fpCompresso = fopen(argv[3], "w+");
	FILE* fpLetto = fopen(argv[2], "r");
//header
//printBits(bitUsati);
	qsort(bitUsati, 256, sizeof(CanonicalNode*), reorderCod);
//printBits(bitUsati);

	int i;
	for (i = 0; i < 256; i++) {
		fputc(bitUsati[i]->numBitCodifica, fpCompresso);
	}
	//inserisco bit per scrivere lunghezza da leggere dell'ultimo bit
	fputc(0, fpCompresso);

	int t = fgetc(fpLetto);
	int buffer[8] = { 0 };
	int indice;
	int counter = 0;
	int numBitUltimoByte = 0;
	//comprimo file
	while (t != EOF) {
		indice = 256 - bitUsati[t]->numBitCodifica;
		//leggo i bits del carattere attuale
		for (i = indice; i < 256; i++) {
			//ho il buffer pieno, scrivo il suo valore su file
			if (counter == 8) {
				fputc(leggiNumeri8(buffer), fpCompresso);
				counter = 0;
				numBitUltimoByte = 0;
			}
			//leggo un bits e lo inserisco nel buffer
			if (bitUsati[t]->bits[i] != 2) {
				buffer[counter] = bitUsati[t]->bits[i];
				counter++;
				numBitUltimoByte++;
			} else {
				break;
			}
		}
		t = fgetc(fpLetto);
	}
	//scrivo l'ultimo buffer
	//non è per forza tutto pieno quindi potrei non averlo scritto
	if (numBitUltimoByte != 0) {
		fputc(leggiNumeri8(buffer), fpCompresso);
	}
	//inserisco quanti bit leggere dell'ultimo byte subito dopo la header
	fseek(fpCompresso, 256, SEEK_SET);
	fwrite(&numBitUltimoByte, 1, 1, fpCompresso);

	fclose(fpCompresso);
	fclose(fpLetto);
}

void generateTree(Node* ptrElements[], Node elements[], Node* head) {
//creo albero con codifiche
	int i;
	Node* child;
	for (i = 255; i > 0; i--) {
		//non analizzo le frequenze a 0
		if (elements[i - 1].freq > 0) {
			//creo nuovo nodo e lo associo nel modo corretto
			child = (Node*) malloc(sizeof(Node));
			child->freq = ptrElements[i]->freq + ptrElements[i - 1]->freq;
			// non mi serve il carattere unito
			child->caracter = 0;
			child->right = ptrElements[i];
			child->left = ptrElements[i - 1];
			//memorizzo insieme alle frequenze
			ptrElements[i - 1] = child;
			//riordino array
			qsort(ptrElements, 256, sizeof(Node*), comparePtr);
		}
	}
}

void contaFreq(char* argv[], Node elements[]) {
	// leggo byte e conto frequenze
	FILE* file = fopen(argv[2], "r");
	if (file == NULL) {
		perror("File non trovato");
		exit(1);
	}
	int t = fgetc(file);
	while (t != EOF) {
		elements[t].freq++;
		t = fgetc(file);
	}
	fclose(file);
}

void cercaCodice(char* argv[], CanonicalNode** bitRicevuti, Node* head) {
	//cerca nel file compresso il valore originale
	FILE* fpDecompresso = fopen(argv[3], "w+");
	FILE* fpLetto = fopen(argv[2], "r");

	int i;
	int buffer[8];
	Node* tmp = head;

	//leggo valori utili da file e mi posiziono nel punto giusto
	fseek(fpLetto, 256, SEEK_SET);
	int bitUltimoByte = fgetc(fpLetto);
	fseek(fpLetto, 0, SEEK_END);
	int lunghezzaFile = ftell(fpLetto);
	fseek(fpLetto, 257, SEEK_SET);
	int t = fgetc(fpLetto);
	int counterFile = 258;

	while (t != EOF) {
		convABinario(t, buffer);
		//leggo ultimo byte
		if (counterFile == lunghezzaFile) {
			//tratto il caso della lettura dell'ultimo byte
			for (i = 0; i < bitUltimoByte; i++) {
				if (buffer[i] == 1) {
					tmp = tmp->right;
				} else {
					tmp = tmp->left;
				}
				if (tmp->freq != -1) { //foglia
					fputc(tmp->caracter, fpDecompresso);
					tmp = head;
				}
			}
		} else {
			//tratto il caso di un byte normale
			for (i = 0; i < 8; i++) {
				if (buffer[i] == 1) {
					tmp = tmp->right;
				} else {
					tmp = tmp->left;
				}

				if (tmp->freq != -1) { //foglia
					fputc(tmp->caracter, fpDecompresso);
					tmp = head;
				}
			}
		}
		counterFile++;
		t = fgetc(fpLetto);
	}
	fclose(fpDecompresso);
	fclose(fpLetto);
}

void generaCanTree(CanonicalNode** bitRicevuti, Node* head) {
	int i;
	int j;
	Node* child;
	Node* tmp = head;
	int indice;
	//la testa rimane vuota le codifiche partono da li
	for (i = 0; i < 256; i++) {
		if (bitRicevuti[i]->numBitCodifica > 0) {
			indice = 256 - bitRicevuti[i]->numBitCodifica;
			//metto in codifica la sequenza di bit che identifica il carattere attuale
			for (j = indice; j < 256; j++) {
				// 0<-- -->1
				if (bitRicevuti[i]->bits[j] == 1) {
					if (tmp->right == NULL) {
						child = (Node*) malloc(sizeof(Node));
						child->left = NULL;
						child->right = NULL;
						//inizializzo la freq a -1 per poi poterla individuare facilmente
						child->freq = -1;
						tmp->right = child;
						tmp = tmp->right;
					} else {
						tmp = tmp->right;
					}
				} else {
					if (tmp->left == NULL) {
						child = (Node*) malloc(sizeof(Node));
						child->left = NULL;
						child->right = NULL;
						child->freq = -1;
						tmp->left = child;
						tmp = tmp->left;
					} else {
						tmp = tmp->left;
					}
				}
			}
			tmp->caracter = bitRicevuti[i]->caracter;
			//freq=1 --> è una foglia
			tmp->freq = 1;
			tmp = head;
		}
	}
}

void huffmanDec(char* argv[]) {
	CanonicalNode** bitRicevuti = (CanonicalNode**) malloc(
			256 * sizeof(CanonicalNode*));
	initializePtr(bitRicevuti);
	cercaLunghezze(argv, bitRicevuti);
	//printCodLength(bitRicevuti);
	qsort(bitRicevuti, 256, sizeof(CanonicalNode*), compareCodifica);
	//trovo i bit di codifica
	trovaCodifiche(bitRicevuti);
	//creo la testa dell'albero e lo genero
	Node* head = (Node*) malloc(sizeof(Node));
	head->left = NULL;
	head->right = NULL;
	head->freq = -1;
	generaCanTree(bitRicevuti, head);

	//stampaAlbero(head);
	cercaCodice(argv, bitRicevuti, head);
	free(bitRicevuti);
}

void huffmanCod(char* argv[]) {
	Node elements[256];
//array di ptr a nodi
	Node* ptrElements[256];
	initialize(elements);
	contaFreq(argv, elements);

	qsort(elements, 256, sizeof(Node), compare);
//faccio puntare i puntatori agli elementi giusti
	int i;
	for (i = 0; i < 256; i++) {
		ptrElements[i] = &elements[i];
	}

	Node* head = NULL;

	generateTree(ptrElements, elements, head);
	//la testa è l'ultimo elemento dell'array
	head = ptrElements[0];
	//memoria con info generali
	CanonicalNode** bitUsati = (CanonicalNode**) malloc(
			256 * sizeof(CanonicalNode*));
	initializePtr(bitUsati);

	//trovo lunghezze codifiche
	canonico(head, bitUsati, 0);
	//printCodLength(bitUsati);
	qsort(bitUsati, 256, sizeof(CanonicalNode*), compareCodifica);
	//printCodLength(bitUsati);
	int indiceFirst = initializeFirst(bitUsati);
	//trovo le codifiche canoniche
	//bitUsati contiene le lunghezze delle codifiche in ordine crescente!
	if (indiceFirst != -1) {
		//il primo lo abbiamo già inizializzato
		for (i = (indiceFirst + 1); i < 256; i++) {
			complete(bitUsati, i,
					(bitUsati[i]->numBitCodifica
							- bitUsati[i - 1]->numBitCodifica));
		}
	}
	//printBits(bitUsati);
	saveTree(bitUsati, argv);
	free(bitUsati);
}

int main(int argc, char * argv[]) {
	if (argc == 4) {
		if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0) {
			printf("Comprimo %s in %s\n", argv[2], argv[3]);
			huffmanCod(argv);
		} else if (strcmp(argv[1], "-D") == 0 || strcmp(argv[1], "-d") == 0) {
			printf("Decomprimo %s in %s\n", argv[2], argv[3]);
			huffmanDec(argv);
		} else {
			printf("Inserire -C per comprimere o -D per decomprimere\n");
		}
	} else {
		printf(
				"Fromulazione comando:\nProgramma -Modalità(-C o -D) inputFile outputFile\n");
	}
	return 0;
}
