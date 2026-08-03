#include "../lib/mempool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void mempool_init(Mempool *mp) {
    if (!mp) return;
    mp->count = 0;
    // ¡Asegúrate de que esto no sea NULL!
    mp->transactions = (Transaction*)malloc(sizeof(Transaction) * 100); 
    if (!mp->transactions) {
        printf("Error: Malloc failed in mempool_init\n");
        exit(1);
    }
}


int mempool_add_tx(Mempool *mp, const Transaction *tx)
{
    if (!mp || !tx || !mp->transactions) {
        return -1; // Error por puntero nulo
    }

    if (mp->count >= MAX_MEMPOOL_SIZE) {
        return -2; // Error: Mempool llena
    }

    // Copiar la transacción al arreglo
    mp->transactions[mp->count] = *tx;
    mp->count++;

    printf("[Mempool] Transaction added, total pending: %zu\n", mp->count);

    return 0; // <--- ✅ Devuelve 0 indicando ÉXITO
}



void mempool_clear(Mempool *mp)
{
    mp->count =0;
}