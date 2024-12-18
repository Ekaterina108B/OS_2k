#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "shared.h"

typedef struct {
    Game* current_game;
    int game_id;
    int client_id;
} ThreadData;

void cleanup(void);
void handle_signal(int signum);
void menu(void);
void print_active_games(void);
int join_game(const char* game_name);
void* message_receiver(void* arg);
void* ping_sender(void* arg);
void play_game(void);

#endif