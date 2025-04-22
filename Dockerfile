# Dùng image có GCC sẵn, không Fortran
FROM gcc:latest

# Thư mục làm việc
WORKDIR /app

# Cài Boost và build-essential (chỉ những cái cần thiết)
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy toàn bộ project
COPY . .

# Biên dịch server (Linux build)
RUN g++ server.cpp Solver.cpp -o server -pthread -I.

# Khai báo port
ENV PORT=8080
EXPOSE $PORT

# Lệnh khởi chạy server
CMD ./server
