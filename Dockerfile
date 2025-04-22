FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN g++ server.cpp Solver.cpp -o server -pthread -I.

ENV PORT=8080
EXPOSE $PORT

CMD ./server
