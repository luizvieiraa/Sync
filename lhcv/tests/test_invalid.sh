#!/bin/sh
set -eu
ulimit -c 0
# O contrato exige count >= 0; o assert deve rejeitar a violacao.
set +e
./tests/test_station negativo
resultado=$?
set -e
if [ "$resultado" -ne 134 ]; then
    echo "Falha: esperava SIGABRT (134), recebeu $resultado" >&2
    exit 1
fi
echo "OK: quantidade negativa rejeitada por assert (SIGABRT, retorno 134)."
