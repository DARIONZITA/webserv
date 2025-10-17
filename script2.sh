#!/usr/bin/env bash
# test_hang.sh
# Simula 1 cliente pendurado + N clientes normais concorrentes em 2 portas simultâneas
# Uso: ./test_hang.sh [num_testes]
set -eu

NUM_TESTS=${1:-1}       # número de testes (default = 1)
HOST=localhost
PORTS=(8000 8080)       # as portas a testar
HANG_SLEEP=8            # tempo (s) que o cliente pendurado espera
OUTDIR=/tmp/webserv_test
mkdir -p "$OUTDIR"

run_test_for_port() {
  local PORT=$1
  local TESTDIR="${OUTDIR}/port_${PORT}"
  mkdir -p "$TESTDIR"

  echo "=== [Porta $PORT] Starting hanging client (sleep ${HANG_SLEEP}s) ==="

  # Hanging client usando /dev/tcp (bash)
  if exec 3<>/dev/tcp/${HOST}/${PORT} 2>/dev/null; then
    (
      exec 3<>/dev/tcp/${HOST}/${PORT}
      printf 'GET / HTTP/1.1\r\nHost: %s\r\n' "$HOST" >&3
      echo "[$PORT] hanging client: partial request sent, sleeping ${HANG_SLEEP}s..."
      sleep "$HANG_SLEEP"
      echo "[$PORT] hanging client: finishing request now"
      printf '\r\n' >&3
      cat <&3 > "${TESTDIR}/hang_response.txt"
      exec 3>&-
    ) &
    HANG_PID=$!
  else
    echo "[$PORT] /dev/tcp not available — using Python fallback"
    python3 - <<PYCODE &
import socket, time, sys
s = socket.create_connection(("${HOST}", ${PORT}))
s.sendall(b"GET / HTTP/1.1\r\nHost: ${HOST}\r\n")
print(f"[{PORT}] python-hang: partial request sent, sleeping ${HANG_SLEEP}s", file=sys.stderr)
time.sleep(${HANG_SLEEP})
s.sendall(b"\r\n")
print(f"[{PORT}] python-hang: finished request", file=sys.stderr)
resp = b""
try:
    while True:
        data = s.recv(4096)
        if not data:
            break
        resp += data
except Exception as e:
    print(f"[{PORT}] recv ended: {e}", file=sys.stderr)
open("${TESTDIR}/hang_response.txt","wb").write(resp)
s.close()
PYCODE
    HANG_PID=$!
  fi

  # Aguarda cliente pendurado iniciar
  sleep 0.5

  echo "=== [Porta $PORT] Launching ${NUM_TESTS} normal curl clients ==="
  for i in $(seq 1 $NUM_TESTS); do
    curl -s -i http://${HOST}:${PORT}/ > "${TESTDIR}/curl_${i}.txt" &
  done

  wait
  echo "=== [Porta $PORT] DONE — results in ${TESTDIR} ==="

  ok=true
  for i in $(seq 1 $NUM_TESTS); do
    if ! grep -q "200 OK" "${TESTDIR}/curl_${i}.txt"; then
      echo "⚠ Falha: curl_${i} na porta ${PORT} não retornou 200 OK"
      ok=false
    fi
  done

  if $ok; then
    echo "✅ Porta ${PORT} respondeu corretamente a todos os ${NUM_TESTS} testes."
  fi
}

echo "=== Iniciando testes paralelos nas portas 8000 e 8080 ==="
for PORT in "${PORTS[@]}"; do
  run_test_for_port "$PORT" &
done

wait
echo "=== Todos os testes concluídos ==="

if grep -q "✅" "$OUTDIR"/port_*/curl_*.txt 2>/dev/null; then
  echo "🎉 Todos os servidores responderam corretamente!"
else
  echo "⚠ Algumas portas falharam — verifique os logs em ${OUTDIR}"
fi
