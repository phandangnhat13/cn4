# Connect4 C++ Server

A C++ implementation of a Connect4 game server that provides an AI opponent through a REST API.

## Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- nlohmann/json library (header-only)
- cpp-httplib (header-only)

## Building the Project

1. First, make sure you have all the required header files:
   - Copy `httplib.h` to the project directory
   - Download `json.hpp` from nlohmann/json and place it in the project directory

2. Create a build directory and build the project:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running the Server

After building, you can run the server:
```bash
./connect4_server
```

The server will start listening on port 8080.

## API Endpoints

### POST /api/connect4-move

**Method:** POST

**Request Body:**
```json
{
    "board": number[][],
    "current_player": number,
    "valid_moves": number[],
    "is_new_game": boolean
}
```

**Response:**
```json
{
    "move": number
}
```

**Example:**
```json

{
    "board": [
        [0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0],
        [0,0,1,0,0,0,0]
    ],
    "current_player": 2,
    "valid_moves": [0,1,2,3,4,5,6],
    "is_new_game": false
}


{
    "move": 3
}
```

- `board`: 2D array representing the game board (0 = empty, 1 = player 1, 2 = player 2)
- `current_player`: The current player (1 or 2)
- `valid_moves`: Array of valid column indices where a piece can be placed
- `move`: The column index where the AI chooses to place its piece

## Error Handling

The server will return a 400 status code with an error message if:
- The request body is invalid
- There are no valid moves available
- Any other error occurs during processing

## CORS Support

The server includes CORS middleware that allows requests from any origin. This can be modified in the server code if needed.
