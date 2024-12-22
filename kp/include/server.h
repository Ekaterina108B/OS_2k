#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "shared.h"

#define PING_TIMEOUT 5

typedef struct {
    int bulls;
    int cows;
} GuessResult;

void cleanup(void);
void handle_signal(int signum);
int find_free_game_slot(const char* name);
int find_game_by_name(const char* name);
void create_game(int game_id, const char* name, int required_players, int client_id);
void check_word(const char* guess, const char* secret, int* bulls, int* cows);
void process_guess(int game_id, int client_id, const char* guess);
void update_game_state(int game_id);
void remove_player_from_game(int game_id, int client_id);
void check_player_timeouts(void);

#endif 