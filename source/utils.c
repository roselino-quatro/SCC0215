// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "../includes/utils.h"

int argument_quantity(char* arg_string) {
	// 1. Inicia contador de espaços
	int space_qty = 0;

	// 2. Cria ponteiro e anda com ele na string até parar no \0
	// A cada espaço achado, incrementa quantidade
	char* c = arg_string;
	while (*c != '\0') {
		if (*c == ' ') space_qty++;
		c++;
	}

	return space_qty+1; // Quantidade de argumentos é sempre qtd_espacos+1
}

char** arguments_get(char* arg_string) {
	if (arg_string == NULL) return NULL;

	// 1. Pega a quantidade de argumentos e aloca vetor de strings com esse tamanho
	int arg_qty = argument_quantity(arg_string);
	char** arguments = malloc((arg_qty+1) * sizeof(char*));

	// 2. Processa a arg_string, extraindo os argumentos
	char* arg = arg_string;
	for (int i = 0; i < arg_qty; i++) {
		int arg_len = strcspn(arg, " ");
		arguments[i] = strndup(arg, arg_len);

		arg += arg_len + 1; // Move o ponteiro para o proximo argumento
	}
	arguments[arg_qty] = NULL; // Vetor de argumentos é terminado em nulo

	return arguments;
}

void arguments_free(char** arguments) {
	int pos = 0;
	while (arguments[pos] != NULL) {
		free(arguments[pos]);
		pos++;
	}
	free(arguments);
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

void string_to_csv(char* string){
	if(string[0] == '\0') return;
	char* cur = string, *nxt = string;
	bool in_field = false;   // in_field state variable
	bool has_quotes = false;

	// Loop that extracts the fields in string, and separates them by ,
	// To do this, we use 2 pointers. One is current position, the other is next valid position.
	// Every time next is valid, copy it to current. Otherwhise, skip delimiters " and space (' ').
	while(*(nxt) != '\0'){
		if(*nxt == '\"'){   // " char triggers state switch
			in_field = !in_field; // flip "in field" state to "not in field", and vice-versa
			has_quotes = true;
			nxt++; // Jump from delimiter to next position
		}

		if (!in_field && *nxt == ' ') { // When NOT in a field and next pos is a space char
			*nxt = ','; // turn space char into ,
		}

		*cur = *nxt; // Next is valid: copy it's char to current position
		if (*nxt == '\0') {
			break;
		}
		cur++;
		nxt++;
	}

	if(has_quotes) {
		*(cur) = '\0';
	} else {
		*(++cur) = '\0';
	}
}

FILE* fopen_valid(char* name,char* mode) {
	// 0. Abrindo e testando validade do arquivo
	FILE* file = fopen(name, mode);
	if (file == NULL || fgetc(file) != '1') return NULL;

	// 1. Resetando arquivo para estado utilizado pelo programa
	rewind(file);
	return file;
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