// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "utils.h"

char** getFields(int qty,char* src){
	char** fields = malloc((qty+1) *sizeof(char*));
	char* field = src;

	int pos = 0;
	int len = strcspn(field,",");
	while(len != 0){
		fields[pos++] = strndup(field,len);
		field = field + len + 1;	// Advance field pointer and skip the delim
		if(*(field-1) == '\0') break;
		len = strcspn(field,",");
	}
	
	fields[pos] = NULL;
	return fields;
}

int memcpyField(void* dest,const void* src,size_t n){
	memcpy(dest,src,n);
	return n;
}

#define READLINE_RATIO_ 1.5
char* readline(FILE* fstream){
	if(!fstream)	return NULL;
	char* line = NULL;
	int pos = 0, csize = 2;

	do{
		if (pos+2 == csize)	{csize *= READLINE_RATIO_; line = realloc(line, csize);};

		line[pos] = fgetc(fstream);
		if(line[pos] == '\r') pos--;	// Tratando \r - volta e sobrescreve no prox. loop
	} while(line[pos++] != '\n' && !feof(fstream));
	if(pos == 1) {free(line); return NULL;}; // Caso a stream só tenha '\0', retornar NULL

	line[pos - 1] = '\0';
	line = realloc(line, pos);
	return line;
}

void cleanString(char* str){
	if(str[0] == '\0') return;
	char* cur = str, *nxt = str;
	char inField = 0;	// inField state: true -> copy chars , false -> skip " and ' ' turns into ,

	while(*(nxt+1) != '\0'){
		if(*nxt == '\"'){	// " triggers inField state switch
			inField = !inField;
			nxt++;
		} 

		if(!inField && *nxt == ' ') *nxt = ','; // Every ' ' outside a field turns into ,
		*cur++ = *nxt++;
	}
	*cur = '\0';
}

// Função fornecida binarioNaTela
void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

// Abre um arquivo tratando erros
FILE* openFile(const char* name, const char* mode) {
	if(name == NULL || mode == NULL) perror("Null values in OpenFIle");

	FILE* arq = fopen(name, mode);

	return arq;
}