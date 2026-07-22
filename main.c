#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRANSACOES 100
#define NUM_ITENS 7


const char* nomes_itens[] = {"Leite", "Cafe", "Cerveja", "Pao", "Manteiga", "Arroz", "Feijao"};


double calcular_suporte(int itemset, int bd[], int num_transacoes) {
    int contagem = 0;
    for (int i = 0; i < num_transacoes; i++) {

        if ((bd[i] & itemset) == itemset) {
            contagem++;
        }
    }
    return (double)contagem / num_transacoes;
}


double calcular_confianca(int antecedente, int consequente, int bd[], int num_transacoes) {
    int uniao = antecedente | consequente; // OR bit a bit para unir os conjuntos

    double suporte_antecedente = calcular_suporte(antecedente, bd, num_transacoes);
    double suporte_uniao = calcular_suporte(uniao, bd, num_transacoes);

    if (suporte_antecedente == 0.0) return 0.0;
    
    return suporte_uniao / suporte_antecedente;
}

int main() {
    int transacoes[MAX_TRANSACOES];
    int num_transacoes = 0;
    FILE *arquivo;
    char linha[100];


    arquivo = fopen("transacoes.txt", "r");
    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo transacoes.txt\n");
        printf("Certifique-se de que ele esta na mesma pasta do executavel.\n");
        return 1;
    }

    printf("--- Carregando Base de Dados ---\n");
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        int mascara = 0;
        int bit = 0;
        

        char *token = strtok(linha, ",\n");
        while (token != NULL && bit < NUM_ITENS) {
            if (atoi(token) == 1) {

                mascara |= (1 << bit);
            }
            token = strtok(NULL, ",\n");
            bit++;
        }
        transacoes[num_transacoes] = mascara;
        num_transacoes++;
    }
    fclose(arquivo);
    printf("Sucesso: %d transacoes lidas.\n\n", num_transacoes);


    double min_sup, min_conf;
    printf("Digite o Suporte minimo desejado (ex: 0.2 para 20%%): ");
    scanf("%lf", &min_sup);
    printf("Digite a Confianca minima desejada (ex: 0.5 para 50%%): ");
    scanf("%lf", &min_conf);
    printf("\n--- Gerando Regras de Associacao (Apriori 2-Itemsets) ---\n");

    int regras_encontradas = 0;


    for (int i = 0; i < NUM_ITENS; i++) {
        for (int j = 0; j < NUM_ITENS; j++) {
            if (i == j) continue; 

            int itemA = (1 << i);
            int itemB = (1 << j);
            int itemset = itemA | itemB;


            double sup_uniao = calcular_suporte(itemset, transacoes, num_transacoes);
            
            if (sup_uniao >= min_sup) {

                double conf = calcular_confianca(itemA, itemB, transacoes, num_transacoes);
                
                if (conf >= min_conf) {
                    printf("Regra: {%s} -> {%s}\n", nomes_itens[i], nomes_itens[j]);
                    printf("  - Suporte: %.1f%%\n", sup_uniao * 100);
                    printf("  - Confianca: %.1f%%\n\n", conf * 100);
                    regras_encontradas++;
                }
            }
        }
    }

    if (regras_encontradas == 0) {
        printf("Nenhuma regra atendeu aos criterios minimos informados.\n");
    } else {
        printf("Total de %d regras fortes encontradas.\n", regras_encontradas);
    }

    return 0;
}