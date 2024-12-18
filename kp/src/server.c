#include "server.h"

SharedMemory* shared_mem = NULL;
int fd = -1;

const char* word_dictionary[] = {
    "cat", "dog", "sun", "map", "red",
    "box", "cup", "hat", "pen", "sky",
    "signature", "parkour", "leather",
    "voice", "rite", "zip", "country",
    "valley", "lover", "eve", "secretary",
    "balcony", "cacophony", "apple", "banana",
    "computer", "building", "ocean", "mountain",
    "river", "forest", "book", "pencil",
    "table", "chair", "lamp", "clock",
    "door", "window", "car", "tree", "flower"
};
#define DICT_SIZE (sizeof(word_dictionary) / sizeof(word_dictionary[0]))

GuessResult last_results[MAX_GAMES][MAX_PLAYERS];

void cleanup(void) {
    if(shared_mem != NULL){
        munmap(shared_mem, MMAP_SIZE);
    }
    if(fd != -1){
        close(fd);
        shm_unlink("/game_memory");
    }
}

void handle_signal(int signum) {
    printf("\nCompletion signal %d received. Clearing resources...\n", signum);
    cleanup();
    exit(0);
}

int find_free_game_slot(const char* name) {
    if(find_game_by_name(name) != -1){ return -2; }
    for(int i=0; i<MAX_GAMES; ++i){
        if (!shared_mem->games[i].is_active) {
            memset(&shared_mem->games[i], 0, sizeof(Game));
            return i;
        }
    }
    return -1;
}

int find_game_by_name(const char* name) {
    for(int i=0; i<MAX_GAMES; ++i){
        if(shared_mem->games[i].is_active && strcmp(shared_mem->games[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_game(int game_id, const char* name, int required_players, int client_id) {
    Game* game = &shared_mem->games[game_id];
    
    if(name && strlen(name) > 0) {
        strncpy(game->name, name, MAX_GAME_NAME - 1);
        game->name[MAX_GAME_NAME - 1] = '\0';
    } else {
        snprintf(game->name, MAX_GAME_NAME, "Game_%d", game_id);
    }
    
    game->required_players = required_players;
    game->current_players = 1;
    game->is_active = true;
    game->winner_id = -1;
    game->game_state = GAME_STATE_WAITING;
    
    int word_index = rand() % DICT_SIZE;
    strncpy(game->secret_word, word_dictionary[word_index], MAX_WORD_LENGTH - 1);
    game->secret_word[MAX_WORD_LENGTH - 1] = '\0';
    
    memset(game->player_ids, 0, sizeof(game->player_ids));
    memset(game->last_ping, 0, sizeof(game->last_ping));
    game->player_ids[0] = client_id;
    game->last_ping[0] = time(NULL);
    
    printf("A new game '%s' (ID: %d) with the word '%s' was created\n", game->name, game_id, game->secret_word);
}

void check_word(const char* guess, const char* secret, int* bulls, int* cows) {
    *bulls = 0;
    *cows = 0;
    
    size_t len = strlen(secret);
    if (strlen(guess) != len) {
        return;
    }
    
    int secret_count[256] = {0};
    int guess_count[256] = {0};
    
    for(size_t i=0; i<len; ++i){
        if(guess[i] == secret[i]){ ++(*bulls); }
        else {
            ++secret_count[(unsigned char)secret[i]];
            ++guess_count[(unsigned char)guess[i]];
        }
    }
    for(int i=0; i<256; ++i){
        if(secret_count[i] < guess_count[i]){ *cows += secret_count[i]; }
        else { *cows += guess_count[i]; }
    }
}

void process_guess(int game_id, int client_id, const char* guess) {
    Game* game = &shared_mem->games[game_id];
    int bulls, cows;
    
    check_word(guess, game->secret_word, &bulls, &cows);
    last_results[game_id][client_id % MAX_PLAYERS].bulls = bulls;
    last_results[game_id][client_id % MAX_PLAYERS].cows = cows;
    game->last_guess_client_id = client_id;
    game->last_guess_bulls = bulls;
    game->last_guess_cows = cows;
    game->last_guess_time = time(NULL);
    strncpy(game->last_guess_word, guess, MAX_WORD_LENGTH - 1);
    game->last_guess_word[MAX_WORD_LENGTH - 1] = '\0';
    
    for(int i=0; i<MAX_PLAYERS; ++i) {
        if(game->player_ids[i] != 0){ game->has_new_guess[i] = true; }
    }
    usleep(10000);
    printf("Player %d in game '%s': attempt '%s' - %d bulls, %d cows\n", client_id, game->name, guess, bulls, cows);
    
    if(bulls == (int)strlen(game->secret_word)){
        game->winner_id = client_id;
        game->game_state = GAME_STATE_FINISHED;
        game->is_active = false;
        game->current_players = 0;
        memset(game->player_ids, 0, sizeof(game->player_ids));
        printf("Player %d won in game '%s'!\n", client_id, game->name);
    }
}

void update_game_state(int game_id) {
    Game* game = &shared_mem->games[game_id];
    
    if(game->current_players < game->required_players){ game->game_state = GAME_STATE_WAITING; }
    else if(game->winner_id == -1){ game->game_state = GAME_STATE_ACTIVE; }
    if(game->current_players == 0 || game->game_state == GAME_STATE_FINISHED){
        game->is_active = false;
        printf("Game '%s' is over: all players have left the game\n", game->name);
    }
}

void remove_player_from_game(int game_id, int client_id) {
    Game* game = &shared_mem->games[game_id];
    if(!game->is_active){
        printf("Warning: The game is inactive!\n");
        return;
    }
    
    bool player_found = false;
    for(int i=0; i<MAX_PLAYERS; ++i){
        if(game->player_ids[i] == client_id){
            player_found = true;
            game->player_ids[i] = 0;
            game->last_ping[i] = 0;
            break;
        }
    }
    
    if(player_found && game->current_players > 0){
        --game->current_players;
        printf("Player %d has left the game '%s'. Remaining players: %d/%d\n", client_id, game->name, game->current_players, game->required_players);
        if(game->current_players == 0){
            game->is_active = false;
            memset(game->player_ids, 0, sizeof(game->player_ids));
            memset(game->last_ping, 0, sizeof(game->last_ping));
            printf("Game '%s' is over: all players have left the game\n", game->name);
        }
    } else { printf("Warning: No player found or 0 players (found=%d, current=%d)\n", player_found, game->current_players); }
    
    update_game_state(game_id);
}

void check_player_timeouts(void) {
    time_t current_time = time(NULL);
    
    for(int i=0; i<MAX_GAMES; ++i){
        Game* game = &shared_mem->games[i];
        if(game->is_active){
            for (int j=0; j<MAX_PLAYERS; ++j){
                if(game->player_ids[j] != 0){
                    if(current_time - game->last_ping[j] > PING_TIMEOUT){
                        printf("Timeout of player %d in game '%s'\n", game->player_ids[j], game->name);
                        remove_player_from_game(i, game->player_ids[j]);
                    }
                }
            }
        }
    }
}

int main(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    srand(time(NULL));
    
    fd = shm_open("/game_memory", O_CREAT | O_RDWR, 0666);
    if(fd == -1){
        perror("Initialisation error: shm_open");
        exit(1);
    }
    if(ftruncate(fd, MMAP_SIZE) == -1){
        perror("Initialisation error: ftruncate");
        close(fd);
        shm_unlink("/game_memory");
        exit(1);
    }
    shared_mem = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_mem == MAP_FAILED){
        perror("Initialisation error: mmap");
        close(fd);
        shm_unlink("/game_memory");
        exit(1);
    }
    memset(shared_mem, 0, sizeof(SharedMemory));
    shared_mem->server_ready = true;

    printf("Server is up and running\n");
    while(1){
        if(shared_mem->action != ACTION_NONE){
            
            switch(shared_mem->action){
                case ACTION_CREATE_GAME:
                {
                    int game_id = find_free_game_slot(shared_mem->new_game_name);
                    if(game_id == -2){
                        printf("Attempting to create a game with an existing name '%s'\n", shared_mem->new_game_name);
                        shared_mem->game_id = -1;
                    } else if(game_id == -1){
                        printf("There are no slots available for the new game\n");
                        shared_mem->game_id = -1;
                    } else {
                        create_game(game_id, shared_mem->new_game_name, shared_mem->new_game_players, shared_mem->client_id);
                        shared_mem->game_id = game_id;
                    }
                    break;
                }

                case ACTION_QUIT_GAME:
                {
                    if(shared_mem->game_id != -1){
                        remove_player_from_game(shared_mem->game_id, shared_mem->client_id);
                    }
                    break;
                }

                case ACTION_PING:
                {
                    int game_id = shared_mem->game_id;
                    int client_id = shared_mem->client_id;
                    if(game_id != -1){
                        Game* game = &shared_mem->games[game_id];
                        for(int i=0; i<MAX_PLAYERS; ++i){
                            if(game->player_ids[i] == client_id){
                                game->last_ping[i] = time(NULL);
                                break;
                            }
                        }
                    }
                    break;
                }
            }

            shared_mem->action = ACTION_NONE;
        }

        for(int i=0; i<MAX_GAMES; ++i){
            if(shared_mem->games[i].is_active){
                if(strlen(shared_mem->client_guess) > 0 && shared_mem->game_id == i){
                    process_guess(i, shared_mem->client_id, shared_mem->client_guess);
                    shared_mem->client_guess[0] = '\0';
                }
                update_game_state(i);
            }
        }

        check_player_timeouts();
        usleep(50000);
    }

    cleanup();
    return 0;
} 