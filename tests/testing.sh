#!/bin/bash
CLI="../node_cli"
PEER_TARGET="192.168.100.189:8333"

echo -e "\n[1] Verificando el estado inicial de la blockchain..."
$CLI --status

echo -e "\n[2] Creando transacciones de prueba..."
$CLI --tx "ALICE:BOB:50.0"
$CLI --tx "BOB:CARLIE:12.5"

echo -e "\n[3] Ejecutando proceso de minado..."
$CLI --mine

echo -e "\n[4] Verificando el estado tras minar"
$CLI --status

echo -e "\n[5] Probando conectividad p2p"
$CLI --connect "$PEER_TARGET"




