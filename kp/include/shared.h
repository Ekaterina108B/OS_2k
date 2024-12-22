#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h>
#include <time.h>

#define MAX_WORD_LENGTH 20
#define MAX_PLAYERS 10
#define MAX_GAMES 5
#define MMAP_SIZE 4096
#define MAX_GAME_NAME 50
#define GAME_STATE_WAITING 0
#define GAME_STATE_ACTIVE 1
#define GAME_STATE_FINISHED 2

#define ACTION_NONE 0
#define ACTION_CREATE_GAME 1
#define ACTION_QUIT_GAME 2
#define ACTION_PING 3


typedef struct {
    char name[MAX_GAME_NAME];
    int required_players;
    int current_players;
    char secret_word[MAX_WORD_LENGTH];
    bool is_active;
    int winner_id;
    int game_state;
    bool has_new_guess[MAX_PLAYERS];
    int last_guess_client_id;
    int last_guess_bulls;
    int last_guess_cows;
    int last_guess_time;
    char last_guess_word[MAX_WORD_LENGTH];
    time_t last_ping[MAX_PLAYERS];
    int player_ids[MAX_PLAYERS];
} Game;

typedef struct {
    int action;
    int game_id;
    int client_id;
    char client_guess[MAX_WORD_LENGTH];
    bool server_ready;
    char new_game_name[MAX_GAME_NAME];
    int new_game_players;
    Game games[MAX_GAMES];
} SharedMemory;

#endif 