# Etapa 1: compilação
FROM debian:bookworm AS builder

RUN apt update && apt install -y build-essential

WORKDIR /app
COPY . .

RUN make

# Etapa 2: execução
FROM debian:bookworm

WORKDIR /app
COPY --from=builder /app/webserv .

EXPOSE 8080
CMD ["./webserv"]
