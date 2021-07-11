// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "utils.h"

char** getFields(int qty,char* src){
	char** fields = malloc((qty+1) *sizeof(char*));
	char* field = src;
	for(int i = 0; i < qty+1; i++) {
		fields[i] = NULL;
	}

	int pos = 0;
	int len = strcspn(field,",");
	while(len != 0){
		fields[pos++] = strndup(field,len);
		field = field + len + 1;	// Advance field pointer and skip the delim
		if(*(field-1) == '\0') break;
		len = strcspn(field,",");
	}
	
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
	int hasQuotes = 0;

	while(*(nxt+1) != '\0'){
		if(*nxt == '\"'){	// " triggers inField state switch
			inField = !inField;
			hasQuotes = 1;
			nxt++;
		} 

		if(!inField && *nxt == ' ') *nxt = ','; // Every ' ' outside a field turns into ,
		*cur++ = *nxt++;
	}
	if(hasQuotes == 1) {
		*(cur) = '\0';
	} else {
		*(++cur) = '\0';
	}
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

// Abre um arquivo tratando erros
bool closeFile(FILE* file) {
	if(file == NULL) return false;

	fclose(file);

	return true;
}


/*  
    Converte o prefixo do veículo para int

    OBS1:   retorna -1 se o primeiro caracter é '*'

    OBS2:   retorna LIXO se a string contém algo diferente de números e letras 
            maiúsculas (com excessão do caso da OBS1)

    COMO FUNCIONA:

        Para converter os prefixos para int e garantir que prefixos diferentes 
        deem números diferentes interpretamos eles como números em base 36

        Um número N com 5 digitos dI em base 36 é escrito como:

            N = d0 * 36^0 + d1 * 36^1 + d2 * 36^2 + d3 * 36^3 + d4 * 36^4

        Nota-se que estamos escrevendo do digito menos significativo para o 
        mais significativo

        Como o nosso prefixo têm 5 bytes e eles podem assumir 36 valores
        cada, podemos interpretar cada char dele como um digito em base 36, 
        prefixos diferentes formaram números diferentes em base 36 e portanto 
        números diferentes quando convertemos para um int normal
*/
int convertePrefixo(char* str) {

    /* O registro que tem essa string como chave foi removido */
    if(str[0] == '*')
        return -1;

    /* Começamos com o primeiro digito na ordem de 36^0 = 1 */
    int power = 1;

    /* Faz a conversão char por char para chegar ao resultado */
    int result = 0;
    for(int i = 0; i < 5; i++) {

        /* 
            Interpreta o char atual como se fosse um digito
            em base 36. Os digitos da base 36 são:
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D,
            E, F, G, H, I, J, K, L, M, N, O, P, Q, R,
            S, T, U, V, W, X, Y, Z
        */
        int cur_digit;
        /* Checa pelos digitos normais e os converte para números */
        if(str[i] >= '0' && str[i] <= '9')
            cur_digit = str[i] - '0';
        /* Checa pelas letras e as converte para números */
        else if(str[i] >= 'A' && str[i] <= 'Z')
            cur_digit = 10 + str[i] - 'A';

        /*
            Multiplica o digito atual pelo ordem da posição atual
            e adiciona no resultado
            Primeira posição:   36^0 = 1
            Segunda posição:    36^1 = 36
            Terceira posição:   36^2 = 1.296
            Quarta posição:     36^3 = 46.656
            Quinta posição:     36^4 = 1.679.616
        */
        result += cur_digit * power;

        /* Aumenta a ordem atual */
        power *= 36;

    }

    return result;

}