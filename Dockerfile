FROM ubuntu:22.04

# Thiết lập môi trường cơ bản
ENV DEBIAN_FRONTEND=noninteractive

# Cài các dependencies cần thiết
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    g++ \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Tạo thư mục làm việc
WORKDIR /app

# Copy toàn bộ mã nguồn
COPY . .

# Biên dịch file C++
RUN g++ server.cpp Solver.cpp -o server -pthread -I.

# Mặc định port
ENV PORT=8080
EXPOSE $PORT

# Khởi động server
CMD ["./server"]
