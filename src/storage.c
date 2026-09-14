#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/storage.h"

int storage_save_blockchain(const Blockchain *chain, const char *filepath)
{
if(!chain || !filepath) return 0;
FILE *f = fopen(filepath, "w");
    if(!f) return -1;

// output JSON format
fprintf(f,"[\n");
for(size_t i=0; i < chain->length;i++)
    {
    Block *b = chain->blocks[i];
    fprintf(f, "    {\n");

    fprintf(f,"      \"index\": %u,\n",b->index);
    fprintf(f,"      \"timestamp\": %ld,\n",(long)b->timestamp);
    fprintf(f,"      \"round\": %u,\n", b->round);
    fprintf(f,"      \"proposer\": \"%s\", \n", b->proposer);
    fprintf(f,"      \"prev_hash\": \"%s\",\n", b->prev_hash);
    fprintf(f,"      \"hash\": \"%s\",\n", b->hash);
    fprintf(f, "     \"signatures_count\": %u,\n", b->commit_signatures_count);
    fprintf(f,"      \"tx_count\": %u,\n", b->tx_count);
    
    for(uint32_t j = 0; j < b->tx_count; j++)
    {
    Transaction *tx = &b->transactions[j];

        fprintf(f,"  {\n");
        fprintf(f,"  \"sender\": \"%s\",\n", tx->sender);
        fprintf(f,"  \"receiver\": \"%s\",\n", tx->receiver);
        fprintf(f,"  \"amount\": \"%.6f\",\n", tx->amount);
        fprintf(f,"  \"fee\": \"%.6f\",\n", tx->fee);  
        fprintf(f,"  }%s\n", (j< b->tx_count -1) ? "," : "");
        fprintf(f,"  {\n");
    }
    fprintf(f,"     ]\n");
    fprintf(f,"     }%s\n",(i < chain->length - 1) ? "," : "");
    }

    fprintf(f,"     ]\n");
    fprintf(f,"     }\n");
fclose(f);
return 1;
}
