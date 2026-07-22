#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>   

#define MAX_TRANSACOES 100 
#define NUM_ITENS 7        

// traduz a posição do bit para o nome legível na hora de imprimir (em ordem do arquivo txt)
const char* nomes_itens[] = {"Leite", "Cafe", "Cerveja", "Pao", "Manteiga", "Arroz", "Feijao"};

// a frequência (Suporte) em que um conjunto aparece nas compras
double calcular_suporte(int conjunto_itens, int bd[], int num_transacoes) {
    int contagem = 0; //  contador de aparições
    
    for (int i = 0; i < num_transacoes; i++) { // percorre as transacoes
        //se os bits do baterem exatamente com os bits acesos da transação, então o conjunto todo está lá dentro
        if ((bd[i] & conjunto_itens) == conjunto_itens) {
            contagem++; // contabilizado
        }
    }
    return (double)contagem / num_transacoes; // retorna a frequência relativa: proporção
}

//  a probabilidade condicional (Confiança) 
double calcular_confianca(int antecedente, int consequente, int bd[], int num_transacoes) {
    int uniao = antecedente | consequente; //  item A com o item B
    
    // obtem o suporte de A e B juntos
    double suporte_antecedente = calcular_suporte(antecedente, bd, num_transacoes);
    double suporte_uniao = calcular_suporte(uniao, bd, num_transacoes);

    if (suporte_antecedente == 0.0) return 0.0; // caso seja passado porcentagem inválida
    
    return suporte_uniao / suporte_antecedente; // P(A e B) / P(A)
}

int main() {
    int transacoes[MAX_TRANSACOES]; // Vetor onde cada espaço guardará uma transação convertida em bits
    int num_transacoes = 0;         // controla quantas transacoes por vez
    FILE *arquivo;                  // ponteiro de arquivo
    char linha[100];                // array pra armazenar as linhas de arquivo

    // abre o arquivo em modo de leitura
    arquivo = fopen("transacoes.txt", "r");
    if (arquivo == NULL) { // se o arquivo nao existir, e cancelado
        printf("Erro ao abrir o arquivo.\n");
        return 1; 
    }

    // lê linha por linha até o final
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        int mascara = 0; 
        int bit = 0;     // rastreador de qual coluna é lida no momento
        
        // fatiador de texto para lidar com virgula
        char *token = strtok(linha, ",\n");
        while (token != NULL && bit < NUM_ITENS) { 
            if (atoi(token) == 1) { // atoi para converter para int. Se for positivo, entao
                mascara |= (1 << bit); // Desloca o bit para a posição correta 
            }
            token = strtok(NULL, ",\n"); // faz outra fatiada para o próximo pedaco de linha
            bit++; // avanca para o proximo item no txt
        }
        transacoes[num_transacoes] = mascara; // salve a transacao no banco de memoria
        num_transacoes++; // Confirma que adicionou uma transação e vai para a próxima
    }
    fclose(arquivo); //fecha o txt

    double min_sup, min_conf; // variáveis para scanf
    printf("Digite o Suporte minimo (ex: 0.2): "); 
    scanf("%lf", &min_sup); 
    printf("Digite a Confianca minima (ex: 0.5): "); 
    scanf("%lf", &min_conf); 

    int regras_encontradas = 0; // regras que corresponderam

    // testa todas as combinações possíveis entre dois itens
    for (int i = 0; i < NUM_ITENS; i++) {
        for (int j = 0; j < NUM_ITENS; j++) {
            if (i == j) continue; // se os itens forem iguais, nao interagem

            int itemA = (1 << i); // Transforma o índice do item A no seu bit correspondente
            int itemB = (1 << j); // Transforma o índice do item B no seu bit correspondente
            int itemset = itemA | itemB; // Cria a união lógica dos dois itens para a regra

            // Calcula qual é a presença dos dois itens juntos na base de dados inteira
            double sup_uniao = calcular_suporte(itemset, transacoes, num_transacoes);
            
            // só avança caso a dupla atenda ao mínimo exigido
            if (sup_uniao >= min_sup) {
                // Suporte compatível. Calculado se a relação Causa -> Efeito (Confiança) é forte
                double conf = calcular_confianca(itemA, itemB, transacoes, num_transacoes);
                
                // Se atende a confiança mínima, é uma regra de associação válida
                if (conf >= min_conf) {
                    // Resgata os nomes e converte as frações para %
                    printf("Regra: {%s} -> {%s} | Sup: %.1f%% | Conf: %.1f%%\n", nomes_itens[i], nomes_itens[j], sup_uniao * 100, conf * 100);
                    regras_encontradas++; // mais uma regra correspondente
                }
            }
        }
    }

    if (regras_encontradas == 0) printf("Nenhuma regra atendeu aos criterios.\n");

    return 0; 
}