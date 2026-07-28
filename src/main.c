#include <stdio.h>
#include <string.h>
#include "../lib/utils.h"
#include "../lib/block.h"
#include "../lib/blockchain.h"

void test_stage1_hashing()
{
printf("------------- ETAPA 1: Prueba de Hashing SHA 256 ------------------");
uint8_t out[32];
char hex[65];
sha256((const uint8_t*)"", 0, out);
hex_encode(out, 32, hex);
printf("SHA-256(""): %s", hex);
}

int main()
{
test_stage1_hashing();
uint32_t diff_bits = 16;
printf("Creacion de Blockchain y Mineria. Dificultad: %u bits",diff_bits);
Blockchain *chain = blockchain_create(diff_bits);
char hex_hash[65];

hex_encode(chain->blocks[0]->hash,32, hex_hash);
printf("[Genesis] Hash: %s | nonce %u"
, hex_hash, chain->blocks[0]->header.nonce);

// minar bloque 1
Block *b1 = block_create(chain->blocks[0]->hash, "Tx 1: Alice -> Bob (5 BTC)", diff_bits);
mine_block(b1, diff_bits);
blockchain_add_block(chain, b1);
hex_encode(b1->hash, 32, hex_hash);

printf("[Bloque 1] Hash: %s | nonce %u"
, hex_hash, b1->header.nonce);

//validar cadena
printf("¿Cadena Valida?: %s",
    blockchain_is_valid(chain) ? "SI (OK)" : "NO (CORRUPTA)");

    //simulacion de ataque (manipulacion de datos)
    printf("Simulando alteracion maliciosa en el bloque 1");
    b1->data[0]='X';
    printf("¿cadena valida tras alteracion?: %s"
    , blockchain_is_valid(chain) ? "SI (OK)" : "NO (DETECTADA)");
    blockchain_free(chain);
    return 0;
}