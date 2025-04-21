# Sử dụng base image có GCC
FROM gcc:latest

# Thư mục làm việc
WORKDIR /app

# Cài đặt dependencies cần thiết
RUN apt-get update && apt-get install -y \
    build-essential \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
COPY . .

# Build server
# Note: Trên Windows local dùng: g++ server.cpp Solver.cpp -o server.exe -lws2_32 -lwsock32 -I.
# Trên Linux (Docker) dùng pthread thay cho WinSock
RUN g++ server.cpp Solver.cpp -o server -pthread -I.

# Port mặc định (sẽ override bởi biến môi trường)
ENV PORT=8080

# Expose port
EXPOSE $PORT

# Chạy server với port từ biến môi trường
CMD ./server 