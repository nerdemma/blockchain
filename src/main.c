#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../lib/blockchain.h"
#include "../lib/net.h"
#include "../lib/mempool.h"
#include "../lib/utils.h"
#include "../lib/block.h"
#include "../lib/discovery.h"
#include "../lib/peer_pool.h"
#include "../lib/p2p_sync.h"
PeerPool g_peer_pool;

void print_menu(void)
{
    printf("\n========= BLOCKCHAIN NODE =========\n");
    printf("1. View the blockchain status\n");
    printf("2. Mine block with pending transactions\n");
    printf("3. Create testing transaction\n");
    printf("4. Connect to peer (P2P)\n");
    printf("5. Exit\n");
    printf("Select Option: ");
}

// Función auxiliar para limpiar el buffer de stdin y evitar bucles infinitos
static void clear_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(int argc, char *argv[])
{
    uint32_t diff_bits = 16;
    uint16_t tcp_port = DEFAULT_PORT;

    if (argc > 1) { 
        tcp_port = (uint16_t)atoi(argv[1]);
    }

    // 1. Inicializar la estructura global de peers para evitar undefined behavior / segfault
    peer_pool_init(&g_peer_pool);

    // 2. Inicializar Blockchain y validar puntero NULL
    Blockchain *chain = blockchain_init(diff_bits, CHAIN_FILE);
    if (!chain) {
        fprintf(stderr, "[!] Error crítico: No se pudo inicializar la blockchain.\n");
        return 1;
    }

    Mempool mp;
    mempool_init(&mp);

    // 3. Inicializar Servidores
    int server_fd = start_server(tcp_port);
    discovery_start(tcp_port);

    if (p2p_sync_start(server_fd, chain, &mp, &g_peer_pool) == 0) 
    {
    printf("[+] Servidor p2p escuchando segundo plano del puerto: %d\n",tcp_port);
    }


    int option = 0;

    while (option != 5)
    {
        print_menu();
        if (scanf("%d", &option) != 1) {
            printf("[!] Entrada inválida.\n");
            clear_stdin();
            continue;
        }

        switch (option)
        {
        case 1:
        {
            if (!chain || !chain->blocks) {
                printf("[!] Error: Estructura de la cadena no válida.\n");
                break;
            }

            printf("[Local Chain] Total Blocks: %zu | Valid: %s\n",
                   chain->length, blockchain_is_valid(chain) ? "YES" : "NO");

            for (size_t i = 0; i < chain->length; i++) {
                if (chain->blocks[i]) {
                    printf("- Bloque [%zu] Hash cargado correctamente\n", i);
                }
            }
            break;     
        }
        
        case 2:
        {
            if (!chain || chain->length == 0 || !chain->blocks) {
                printf("[!] Error: No hay bloques en la cadena.\n");
                break;
            }

            Block *last = chain->blocks[chain->length - 1];
            if (!last) {
                printf("[!] Error: El último bloque es NULL.\n");
                break;
            }

            printf("\nMining block [%zu]...\n", chain->length);
            
            Block *new_block = block_create(last->hash, mp.transactions, (uint32_t)mp.count, diff_bits);
            if (!new_block) {
                printf("[!] Error al crear el bloque.\n");
                break;
            }

            mine_block(new_block, diff_bits);    
            
            if (blockchain_add_block(chain, new_block)) {
                blockchain_save_block(new_block, CHAIN_FILE);
                
                // Difundir el nuevo bloque a los peers conectados si el pool tiene nodos
                uint8_t buffer[1024];
                size_t bytes = block_serialize(new_block, buffer);
                if (bytes > 0) {
                    peer_pool_broadcast(&g_peer_pool, MSG_BLOCK, buffer, (uint32_t)bytes);
                }

                mempool_clear(&mp); 
                printf("[+] Bloque minado, guardado y transmitido con éxito.\n");   
            } else {
                block_free(new_block);
                printf("[!] El bloque minado fue rechazado por la cadena.\n");
            }
            break;
        }
       
  case 3:
    {
        printf("[Debug] Mempool address: %p, Transactions pointer: %p, Count: %zu\n", (void*)&mp, (void*)mp.transactions, mp.count);
        
        if (mp.transactions == NULL) {
            printf("[!] CRITICAL: Mempool transactions array is NULL. Did you call mempool_init?\n");
            break;
        }

        Transaction tx;
        memset(&tx, 0, sizeof(Transaction));
        transaction_create(&tx, "ALICE_ADDR", "BOB_ADDR", 25.5);

        // Verificación de bounds antes de añadir
        if (mp.count >= 100) { // Ajusta 100 al valor real de tu capacidad
            printf("[!] CRITICAL: Mempool capacity reached!\n");
            break;
        }

        printf("[Debug] Attempting to add transaction...\n");
        if (mempool_add_tx(&mp, &tx) == 0) {
            printf("[+] Transaction added successfully.\n");
        } else {
            printf("[!] Error in mempool_add_tx.\n");
        }
        break;   
    }

        case 4:
        {
            char ip[64] = {0};
            int p = 0;
            printf("PEER IP ADDRESS: ");
            if (scanf("%63s", ip) != 1) { clear_stdin(); break; }
            
            printf("PORT: ");    
            if (scanf("%d", &p) != 1) { clear_stdin(); break; }
            
            int peer_fd = connect_to_peer(ip, (uint16_t)p);    
            if (peer_fd >= 0) {
                // Agregar al pool activo
                peer_pool_add(&g_peer_pool, peer_fd, ip, (uint16_t)p);

                MsgVersion v = {1, (uint64_t)time(NULL), (uint32_t)chain->length};
                send_message(peer_fd, MSG_VERSION, &v, sizeof(MsgVersion));
            } else {
                printf("[!] No se pudo conectar al peer %s:%d\n", ip, p);
            }
            break;
        }

        case 5:
            printf("Cerrando nodo...\n");
            break;

        default:
            printf("Opción no válida.\n");
            break;
        }
    }

    // Liberación segura de recursos
    p2p_sync_stop();
    if(server_fd >=0) close(server_fd);
    mempool_clear(&mp);
    blockchain_free(chain);
    return 0;
}