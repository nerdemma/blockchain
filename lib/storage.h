#ifndef STORAGE_H
#define STORAGE_H
#include "blockchain.h"


int storage_save_blockchain(const Blockchain *chain, const char *filepath);
int storage_load_blockchain(Blockchain *chain, const char *filepath);


#endif // STORAGE_H
