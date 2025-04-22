#include "httplib.h"
#include "json.hpp"
#include "Position.hpp"
#include "Solver.hpp"
#include "OpeningBook.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <random>

using json = nlohmann::json;
using namespace GameSolver::Connect4;

// Global state như Python
Solver solver;
Position position;
std::string move_sequence = "";
std::vector<std::vector<int>> previous_board;

// Khởi tạo previous_board
void init_previous_board() {
    previous_board = std::vector<std::vector<int>>(6, std::vector<int>(7, 0));
}

// Reset state
void reset_state() {
    position = Position();
    move_sequence = "";
    init_previous_board();
    std::cout << "State reset" << std::endl;
}

// Kiểm tra game over
bool is_game_over(const std::vector<std::vector<int>>& board) {
    // Check full board (draw)
    bool is_full = true;
    for(const auto& row : board) {
        for(int cell : row) {
            if(cell == 0) {
                is_full = false;
                break;
            }
        }
        if(!is_full) break;
    }
    if(is_full) return true;

    // Check for 4 in a row
    int height = board.size();
    int width = board[0].size();

    for(int r = 0; r < height; r++) {
        for(int c = 0; c < width; c++) {
            if(board[r][c] == 0) continue;
            int player = board[r][c];

            // Check right
            if(c <= width - 4) {
                bool win = true;
                for(int i = 0; i < 4; i++) {
                    if(board[r][c+i] != player) {
                        win = false;
                        break;
                    }
                }
                if(win) return true;
            }

            // Check down
            if(r <= height - 4) {
                bool win = true;
                for(int i = 0; i < 4; i++) {
                    if(board[r+i][c] != player) {
                        win = false;
                        break;
                    }
                }
                if(win) return true;
            }

            // Diagonal down-right
            if(r <= height - 4 && c <= width - 4) {
                bool win = true;
                for(int i = 0; i < 4; i++) {
                    if(board[r+i][c+i] != player) {
                        win = false;
                        break;
                    }
                }
                if(win) return true;
            }

            // Diagonal up-right
            if(r >= 3 && c <= width - 4) {
                bool win = true;
                for(int i = 0; i < 4; i++) {
                    if(board[r-i][c+i] != player) {
                        win = false;
                        break;
                    }
                }
                if(win) return true;
            }
        }
    }
    return false;
}

// Debug: In ra trạng thái bàn cờ
void printBoard(const std::vector<std::vector<int>>& board) {
    std::cout << "\nBoard state:" << std::endl;
    for(int row = 0; row < 6; row++) {
        std::cout << "|";
        for(int col = 0; col < 7; col++) {
            if(board[row][col] == 0) std::cout << " ";
            else if(board[row][col] == 1) std::cout << "X";
            else std::cout << "O";
            std::cout << "|";
        }
        std::cout << std::endl;
    }
    std::cout << " 1 2 3 4 5 6 7" << std::endl;
}

// Đăng ký nước đi của đối thủ
void register_opponent_move(const std::vector<std::vector<int>>& current_board) {
    // Game over? Reset everything.
    if(is_game_over(current_board)) {
        std::cout << "Opponent wins. Game over detected. Resetting state." << std::endl;
        reset_state();
        return;
    }

    int height = current_board.size();
    int width = current_board[0].size();

    // Find the column where a new piece was added
    for(int col = 0; col < width; col++) {
        for(int row = 0; row < height; row++) {
            if(previous_board[row][col] != current_board[row][col]) {
                // There is a difference — new disc dropped
                if(current_board[row][col] != 0 && previous_board[row][col] == 0) {
                    position.playCol(col);
                    move_sequence += std::to_string(col + 1);
                    // Deep copy current_board to previous_board
                    previous_board = std::vector<std::vector<int>>(current_board);
                    return;
                }
            }
        }
    }

    // If no move detected (possibly a duplicate request), just update snapshot
    previous_board = std::vector<std::vector<int>>(current_board);
}

// Tìm nước đi tối ưu
int getBestMove(int current_player, const std::vector<int>& valid_moves) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::cout << "\nAnalyzing position..." << std::endl;
    std::cout << "Current sequence: " << move_sequence << std::endl;
    
    // Phân tích tất cả các nước đi
    std::vector<int> scores = solver.analyze(position);

    // In ra điểm số của từng nước đi
    std::cout << "\nScores: ";
    for (int i = 0; i < Position::WIDTH; i++) {
        std::cout << scores[i] << " ";
    }
    std::cout << std::endl;

    // Tìm điểm số cao nhất trong các nước đi hợp lệ
    int best_score = scores[valid_moves[0]];
    for(int move : valid_moves) {
        if(scores[move] > best_score) {
            best_score = scores[move];
        }
    }

    // Tìm tất cả các cột có điểm bằng điểm cao nhất
    std::vector<int> best_moves;
    for(int move : valid_moves) {
        if(scores[move] == best_score) {
            best_moves.push_back(move);
        }
    }

    // Random chọn một trong các cột tốt nhất
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, best_moves.size() - 1);
    int best_col = best_moves[dis(gen)];

    // Cập nhật trạng thái
    position.playCol(best_col);
    move_sequence += std::to_string(best_col + 1);

    // Cập nhật previous_board với nước đi của AI
    for(int row = previous_board.size() - 1; row >= 0; row--) {
        if(previous_board[row][best_col] == 0) {
            previous_board[row][best_col] = current_player;
            break;
        }
    }

    // Game over? Reset everything.
    if(is_game_over(previous_board)) {
        std::cout << "I win. Game over detected. Resetting state." << std::endl;
        reset_state();
        return -1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Analysis took " << duration.count() << "ms" << std::endl;
    std::cout << "Selected move: " << best_col << " with score: " << scores[best_col] << std::endl;
    std::cout << "Number of equally good moves: " << best_moves.size() << std::endl;

    return best_col;
}

int main() {
    std::cout << "Initializing solver..." << std::endl;
    
    std::cout << "Loading opening book..." << std::endl;
    solver.loadBook("7x6.book");

    // Khởi tạo previous_board
    init_previous_board();

    httplib::Server svr;

    // Đọc port từ biến môi trường hoặc dùng default
    const char* port_str = std::getenv("PORT");
    int port = port_str ? std::stoi(port_str) : 8080;

    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    // API chính để lấy nước đi
    svr.Post("/api/connect4-move", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        
        try {
            auto start = std::chrono::high_resolution_clock::now();

            json data = json::parse(req.body);
            std::vector<std::vector<int>> board = data["board"];
            int current_player = data["current_player"];
            std::vector<int> valid_moves = data["valid_moves"];

            if (valid_moves.empty()) throw std::runtime_error("no valid moves");

            std::cout << "\nReceived request with:" << std::endl;
            std::cout << "Current player: " << current_player << std::endl;
            std::cout << "Valid moves: ";
            for(int move : valid_moves) std::cout << move << " ";
            std::cout << std::endl;
            printBoard(board);

            // Đăng ký nước đi của đối thủ
            register_opponent_move(board);

            // Lấy nước đi tốt nhất
            int selected_move = getBestMove(current_player, valid_moves);

            // Nếu game over, trả về nước đi đầu tiên
            if(selected_move == -1) {
                selected_move = valid_moves[0];
            }

            json response = {{"move", selected_move}};
            res.set_content(response.dump(), "application/json");

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "\nTotal request processed in " << duration.count() << "ms" << std::endl;
            std::cout << "--------------------\n" << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            json error = {{"error", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // Health check endpoint
    svr.Get("/api/test", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        json response = {
            {"status", "ok"},
            {"message", "Server is running"}
        };
        res.set_content(response.dump(), "application/json");
    });

    // Reset ván
    svr.Post("/api/reset", [](const httplib::Request& req, httplib::Response& res) {
        reset_state();
        res.set_content("{\"status\": \"reset done\"}", "application/json");
    });

    std::cout << "Server running on port " << port << "..." << std::endl;
    svr.listen("0.0.0.0", port);
    return 0;
}
