#!/usr/bin/env bash
# test_hang.sh
# Simula 1 cliente pendurado + 4 clientes normais concorrentes
# Agora testa as portas 8000 e 8080, e aceita número de repetições como argumento.
# Uso: ./test_hang.sh [num_testes]
set -eu

NUM_TESTES="${1:-1}"    # número de testes (default: 1)
HOST=localhost
PORTS=(8000 8080)
HANG_SLEEP=8
OUTDIR=/tmp/webserv_test
mkdir -p "$OUTDIR"

echo "=== Executando $NUM_TESTES teste(s) nas portas: ${PORTS[*]} ==="

for ((t=1; t<=NUM_TESTES; t++)); do
  echo
  echo "==================== TESTE $t ===================="

  for PORT in "${PORTS[@]}"; do
    echo "--- Porta $PORT ---"
    TESTDIR="$OUTDIR/test_${t}_${PORT}"
    mkdir -p "$TESTDIR"

    echo "=== Iniciando cliente pendurado (porta $PORT) ==="

    if exec 3<>/dev/tcp/${HOST}/${PORT} 2>/dev/null; then
      (
        exec 3<>/dev/tcp/${HOST}/${PORT}
        printf 'GET / HTTP/1.1\r\nHost: %s\r\n' "$HOST" >&3
        echo "hanging client ($PORT): parcial enviada, dormindo ${HANG_SLEEP}s..."
        sleep "$HANG_SLEEP"
        echo "hanging client ($PORT): finalizando requisição"
        printf '\r\n' >&3
        cat <&3 > "$TESTDIR/hang_response.txt"
        exec 3>&-
      ) &
      HANG_PID=$!
    else
      echo "/dev/tcp não disponível — usando Python fallback"
      python3 - <<PYCODE &
import socket, time, sys
s = socket.create_connection(("${HOST}", ${PORT}))
s.sendall(b"GET / HTTP/1.1\r\nHost: ${HOST}\r\n")
print("python-hang ($PORT): parcial enviada, dormindo ${HANG_SLEEP}s", file=sys.stderr)
time.sleep(${HANG_SLEEP})
s.sendall(b"\r\n")
resp = b""
try:
    while True:
        data = s.recv(4096)
        if not data:
            break
        resp += data
except Exception as e:
    print("python-hang ($PORT): recv terminou:", e, file=sys.stderr)
open("${TESTDIR}/hang_response.txt","wb").write(resp)
s.close()
PYCODE
      HANG_PID=$!
    fi

    sleep 0.5

    echo "=== Lançando 4 clientes curl em paralelo (porta $PORT) ==="
    for i in 1 2 3 4; do
      curl -s http://${HOST}:${PORT}/ > "${TESTDIR}/curl_${i}.txt" &
    done

    echo "Aguardando término de todos os clientes..."
    wait

    echo "--- Resultados porta $PORT ---"
    for i in 1 2 3 4; do
      if grep -q "Servidor funcionando" "${TESTDIR}/curl_${i}.txt"; then
        echo "✓ curl_$i respondeu corretamente (porta $PORT)"
      else
        echo "⚠ curl_$i não teve resposta válida (porta $PORT)"
      fi
    done

    if [ -s "${TESTDIR}/hang_response.txt" ]; then
      echo "✓ Cliente pendurado recebeu resposta (porta $PORT)"
    else
      echo "⚠ Cliente pendurado não recebeu resposta (porta $PORT)"
    fi
  done
done

# --- Verificação final ---
echo
echo "==============================================="
if grep -qr "200 OK" "$OUTDIR" && ! grep -qr "⚠" "$OUTDIR"; then
  echo "✅ TODOS OS TESTES FORAM BEM-SUCEDIDOS!"
else
  echo "❌ ALGUNS TESTES FALHARAM — verifique saídas em $OUTDIR"
fi
echo "Resultados salvos em: $OUTDIR"
