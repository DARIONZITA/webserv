#!/usr/bin/env bash
# test_hang.sh
# Simula 1 cliente pendurado + 4 clientes normais concorrentes
# Run: ./test_hang.sh
set -eu

HOST=localhost
PORT=8080
HANG_SLEEP=8   # tempo (s) que o cliente pendurado espera antes de completar o request
OUTDIR=/tmp/webserv_test
mkdir -p "$OUTDIR"

echo "=== Starting hanging client (will send partial request and sleep ${HANG_SLEEP}s) ==="

# Hanging client using /dev/tcp if available (bash). Runs in background.
if exec 3<>/dev/tcp/${HOST}/${PORT} 2>/dev/null; then
  # Use a subshell so fd 3 is closed when done
  (
    exec 3<>/dev/tcp/${HOST}/${PORT}
    printf 'GET / HTTP/1.1\r\nHost: %s\r\n' "$HOST" >&3
    echo "hanging client: partial request sent, sleeping ${HANG_SLEEP}s..."
    sleep "$HANG_SLEEP"
    echo "hanging client: finishing request now"
    printf '\r\n' >&3                  # send final blank line -> completes request
    # capture full response to a file
    cat <&3 > "$OUTDIR/hang_response.txt"
    exec 3>&-
  ) &
  HANG_PID=$!
  echo "hanging client pid: $HANG_PID"
else
  # Fallback to Python if /dev/tcp is not supported
  echo "/dev/tcp not available — using Python fallback for hanging client"
  python3 - <<PYCODE &
import socket, time, sys
s = socket.create_connection(("${HOST}", ${PORT}))
# send partial
s.sendall(b"GET / HTTP/1.1\r\nHost: ${HOST}\r\n")
print("python-hang: partial request sent, sleeping ${HANG_SLEEP}s", file=sys.stderr)
time.sleep(${HANG_SLEEP})
s.sendall(b"\r\n")
print("python-hang: finished request, reading response...", file=sys.stderr)
resp = b""
try:
    while True:
        data = s.recv(4096)
        if not data:
            break
        resp += data
except Exception as e:
    print("python-hang: recv ended:", e, file=sys.stderr)
open("${OUTDIR}/hang_response.txt","wb").write(resp)
s.close()
PYCODE
  HANG_PID=$!
  echo "python-hang pid: $HANG_PID"
fi

# small delay to ensure hanging client has connected and sent partial request
sleep 0.5

echo "=== Launching 4 normal curl clients in parallel ==="
for i in 1 2 3 4; do
  curl -s http://${HOST}:${PORT}/ > "${OUTDIR}/curl_${i}.txt" &
  echo "curl $i pid: $!"
done

echo "=== Waiting for all clients to finish ==="
wait

echo "=== DONE — results in $OUTDIR ==="
echo "Hanging client output (may be empty if server closed early):"
[ -f "${OUTDIR}/hang_response.txt" ] && sed -n '1,40p' "${OUTDIR}/hang_response.txt" || echo "(no hang_response.txt)"
echo
for i in 1 2 3 4; do
  echo "---- curl_${i}.txt ----"
  sed -n '1,20p' "${OUTDIR}/curl_${i}.txt" || echo "(no output)"
  echo
done
