#include <string.h>
#include "../lib/merkle.h"
#include "../lib/utils.h"

void merkle_tree_calculate_root(const Transaction *txs, uint32_t count, uint8_t root[32])
{
	if (!txs || count == 0)
	{
        memset(root, 0, 32);
        return;
    }

    double_sha256((const uint8_t *)txs, sizeof(Transaction) * count, root);
}
