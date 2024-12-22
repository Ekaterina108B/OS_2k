#include "client.h"

SharedMemory* shared_mem = NULL;
int fd = -1;
int current_game_id = -1;
int my_client_id = -1;
volatile int game_running = 0;
volatile int should_exit = 0;

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

void cleanup(void) {
    if(shared_mem != NULL){ munmap(shared_mem, MMAP_SIZE); }
    if(fd != -1){ close(fd); }
    pthread_mutex_destroy(&print_mutex);
}

void handle_signal(int signum) {
    printf("\nCompletion signal %d received. Exit the game...\n", signum);
    if(current_game_id != -1){
        shared_mem->action = ACTION_QUIT_GAME;
        shared_mem->game_id = current_game_id;
        shared_mem->client_id = my_client_id;
    }
    should_exit = 1;
    game_running = 0;
    cleanup();
    exit(0);
}

void menu(void) {
    printf("\n=== Bulls and cows ===\n");
    printf("1. Create a new game\n");
    printf("2. Join an existing game\n");
    printf("3. Show list of active games\n");
    printf("4. Exit\n");
    printf("Select action: ");
}

void print_active_games(void) {
    printf("\nList of active games:\n");
    bool found = false;
    for (int i=0; i<MAX_GAMES; ++i) {
        if(shared_mem->games[i].is_active){
            printf("- %s (Players: %d/%d)\n", shared_mem->games[i].name, shared_mem->games[i].current_players, shared_mem->games[i].required_players);
            found = true;
        }
    }
    if(!found){ printf("No active games\n"); }
}

int join_game(const char* game_name) {
    for(int i=0; i<MAX_GAMES; ++i){
        if(shared_mem->games[i].is_active && strcmp(shared_mem->games[i].name, game_name) == 0){
            Game* game = &shared_mem->games[i];
            if(game->current_players < game->required_players){
                for(int j=0; j<MAX_PLAYERS; ++j){
                    if(game->player_ids[j] == 0){
                        game->player_ids[j] = my_client_id;
                        game->last_ping[j] = time(NULL);
                        game->current_players++;
                        return i;
                    }
                }
            } else {
                printf("The game is already full!\n");
                return -1;
            }
        }
    }
    printf("404: Game not found!\n");
    return -1;
}

void* message_receiver(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    Game* current_game = data->current_game;
    int last_winner = -1;
    int last_players = current_game->current_players;
    int last_guess_client_id = -1;
    int last_guess_time = 0;
    int my_player_index = -1;
    for(int i=0; i<MAX_PLAYERS; ++i){
        if(current_game->player_ids[i] == my_client_id){
            my_player_index = i;
            break;
        }
    }

    while(game_running && my_player_index != -1){
        if(current_game->has_new_guess[my_player_index] && 
            (current_game->last_guess_client_id != last_guess_client_id || 
             current_game->last_guess_time != last_guess_time)) {
            
            pthread_mutex_lock(&print_mutex);
            if(current_game->last_guess_client_id == my_client_id){
                printf("Your attempt '%s': %d bulls, %d cows\n", 
                    current_game->last_guess_word,
                    current_game->last_guess_bulls,
                    current_game->last_guess_cows);
            } else {
                printf("\nPlayer %d: attempt '%s' - %d bulls, %d cows\n",
                    current_game->last_guess_client_id,
                    current_game->last_guess_word,
                    current_game->last_guess_bulls,
                    current_game->last_guess_cows);
            }
            printf("> ");
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex);
            
            last_guess_client_id = current_game->last_guess_client_id;
            last_guess_time = current_game->last_guess_time;
            current_game->has_new_guess[my_player_index] = false;
        }
        
        if(current_game->winner_id != -1 && current_game->winner_id != last_winner) {
            pthread_mutex_lock(&print_mutex);
            if(current_game->winner_id == my_client_id){
                printf("\nCongratulations! You've won! Press Enter to return to the menu...\n");
            } else {
                printf("\nGame over! The winner is player %d. Press Enter to return to the menu...\n", current_game->winner_id);
            }
            pthread_mutex_unlock(&print_mutex);
            
            game_running = 0;
            return NULL;
        }

        if (current_game->current_players != last_players) {
            pthread_mutex_lock(&print_mutex);
            printf("\nThe number of players has changed: %d/%d\n", current_game->current_players, current_game->required_players);
            printf("> ");
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex);
            last_players = current_game->current_players;
        }

        usleep(100000);
    }
    return NULL;
}

void* ping_sender(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    while(game_running){
        shared_mem->action = ACTION_PING;
        shared_mem->game_id = data->game_id;
        shared_mem->client_id = data->client_id;
        sleep(1);
    }
    return NULL;
}

void play_game(void) {
    char guess[MAX_WORD_LENGTH];
    Game* current_game = &shared_mem->games[current_game_id];
    char game_name[MAX_GAME_NAME];
    strncpy(game_name, current_game->name, MAX_GAME_NAME - 1);
    game_name[MAX_GAME_NAME - 1] = '\0';
    
    printf("\nYou've joined the game '%s'\n", game_name);
    printf("Number of players: %d/%d\n", current_game->current_players, current_game->required_players);

    if(strlen(current_game->last_guess_word) > 0 && current_game->last_guess_client_id != 0) {
        printf("Last guess: player %d attempted '%s' - %d bulls, %d cows\n",
               current_game->last_guess_client_id,
               current_game->last_guess_word,
               current_game->last_guess_bulls,
               current_game->last_guess_cows);
    }
    printf("Guess the word! (enter 'quit' to exit)\n");
    fflush(stdout);
    
    ThreadData thread_data = {
        .current_game = current_game,
        .game_id = current_game_id,
        .client_id = my_client_id
    };
    pthread_t receiver_thread, ping_thread;
    game_running = 1;
    if(pthread_create(&receiver_thread, NULL, message_receiver, &thread_data) != 0 ||
        pthread_create(&ping_thread, NULL, ping_sender, &thread_data) != 0){
        perror("Failed to create threads");
        return;
    }

    while(game_running && !should_exit){
        if(fgets(guess, MAX_WORD_LENGTH, stdin) == NULL){ break; }

        guess[strcspn(guess, "\n")] = 0;
        if(strcmp(guess, "quit") == 0){
            printf("\nQuitting the game...\n");
            shared_mem->action = ACTION_QUIT_GAME;
            shared_mem->game_id = current_game_id;
            shared_mem->client_id = my_client_id;
            game_running = 0;
            break;
        }
        if(strlen(guess) == 0 && game_running){
            printf("> ");
            fflush(stdout);
            continue;
        }

        shared_mem->client_id = my_client_id;
        shared_mem->game_id = current_game_id;
        strncpy(shared_mem->client_guess, guess, MAX_WORD_LENGTH - 1);
        shared_mem->client_guess[MAX_WORD_LENGTH - 1] = '\0';
    }

    game_running = 0;
    pthread_join(receiver_thread, NULL);
    pthread_join(ping_thread, NULL);
    current_game_id = -1;
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    fd = shm_open("/game_memory", O_RDWR, 0666);
    if(fd == -1){
        printf("Error: server is not started\n");
        exit(1);
    }
    shared_mem = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_mem == MAP_FAILED){
        perror("Initialisation error: mmap");
        close(fd);
        exit(1);
    }

    while(!shared_mem->server_ready) {
        printf("Waiting for server...\n");
        sleep(1);
    }

    my_client_id = getpid() % 10000;
    int choice;
    while(!should_exit){
        menu();
        while(scanf("%d", &choice) != 1){
            while(getchar() != '\n');
            printf("Invalid entry. Try again: ");
        }

        int c;
        while((c = getchar()) != '\n' && c != EOF);

        switch(choice){
            case 1:
            {
                char game_name[MAX_GAME_NAME];
                printf("Enter the name of the game: ");
                if (scanf("%s", game_name) != 1) {
                    printf("Error reading game name\n");
                    break;
                }
                printf("Enter the number of players (2 to 10): ");
                int required_players;
                while(scanf("%d", &required_players) != 1 || required_players < 2 || required_players > 10){
                    while(getchar() != '\n');
                    printf("Incorrect number of players. Enter a number between 2 and 10: ");
                }
                
                shared_mem->action = ACTION_CREATE_GAME;
                shared_mem->client_id = my_client_id;
                strncpy(shared_mem->new_game_name, game_name, MAX_GAME_NAME - 1);
                shared_mem->new_game_name[MAX_GAME_NAME - 1] = '\0';
                shared_mem->new_game_players = required_players;
                usleep(100000);
                
                current_game_id = shared_mem->game_id;
                if(current_game_id != -1){
                    printf("The game has been successfully created!\n");
                    play_game();
                } else {
                    printf("Game creation error: a game with this name already exists\n");
                }
                break;
            }

            case 2: {
                char game_name[MAX_GAME_NAME];
                printf("Enter the name of the game: ");
                if(scanf("%s", game_name) != 1){
                    printf("Error reading game name\n");
                    break;
                }
                
                current_game_id = join_game(game_name);
                if(current_game_id != -1){ play_game(); }
                break;
            }

            case 3:
            {
                print_active_games();
                break;
            }

            case 4:
            {
                should_exit = 1;
                break;
            }

            default:
                printf("Wrong choice\n");
        }
    }

    cleanup();
    return 0;
} 