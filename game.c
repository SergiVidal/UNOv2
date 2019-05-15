#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "game.h"
#include "player.h"
#include "cli.h"

// TODO: Error al guardar nombre y tipo, se traga un \n
Player *GAME_create_bots(char *filename, Game *game) {
    FILE *f = NULL;
    Player *players = NULL;
    char skip;
    int total = 0;

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error intentando abrir el fichero %s\n", filename);
    } else {
        fscanf(f, "%d", &total);
        fscanf(f, "%c", &skip);
        players = (Player *) realloc(players, sizeof(Player) * total);
        for (int i = 0; i < total; i++) {
            fscanf(f, "%c", &skip);
            fgets(players[i].name, MAXC, f);
            fgets(players[i].type, MAXC, f);
            fscanf(f, "%d", &players[i].num_cards);
            fscanf(f, "%c", &skip);
            (game->total_players)++;
        }

        fclose(f);
    }
    return players;
}

// TODO: Error al guardar nombre y tipo, se traga un \n
Player GAME_create_player(char *filename, Game *game) {
    FILE *f = NULL;
    Player player;

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error intentando abrir el fichero %s\n", filename);
    } else {
        fgets(player.name, MAXC, f);
        fscanf(f, "%d", &player.wins);
        fscanf(f, "%d", &player.loses);
        strcpy(player.type, "Player");
        player.num_cards = 7;
        (game->total_players)++;
        fclose(f);
    }
    return player;
}

Player *GAME_create_player_list(Player *bots, Player player, Game *game) {
    Player *players = NULL;
    players = (Player *) realloc(players, sizeof(Player) * game->total_players);

    for (int i = 0; i < game->total_players - 1; i++) {
        players[i] = bots[i];

    }
    players[game->total_players - 1] = player;

    return players;
}

void GAME_swap(Player *p1, Player *p2) {
    Player tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

void GAME_sort_by_name(Player *players, int length) {
    int min_pos;

    for (int i = 0; i < length - 1; i++) {
        min_pos = i;
        for (int j = i + 1; j < length; j++) {
            if (strcmp(players[min_pos].name, players[j].name) > 0) {
                min_pos = j;
            }
        }
        GAME_swap(&players[min_pos], &players[i]);
    }
}

void GAME_init_hands(Stack *stack, Player *player_list, Game *game) {
    Card *card;
    PDIList *handList = NULL;

    handList = (PDIList *) realloc(handList, sizeof(PDIList) * game->total_players);
    for (int i = 0; i < game->total_players; i++) { //i < totalbots
        handList[i] = LIST_create();
        for (int j = 0; j < player_list[i].num_cards; j++) {
            card = STACK_pop(stack); // Saca una carta del Deck
            LIST_insert(&handList[i], card); // Inserta una carta en la PDIList
        }
        //Falta añadir la handList a cada uno de los players
        player_list[i].handList = &handList[i];
    }
}

void GAME_init_game(Game *game, Player *players) {
    // Init
    Stack deck;
    Stack discardDeck;
    ListBi player_list;

    // Genera y Mezcla BARAJA
    deck = STACK_fill_deck();
    deck = STACK_randomize(&deck);
    game->deck = deck;

    // Genera baraja DESCARTE
    discardDeck = STACK_create();
    game->discard_deck = discardDeck;

    // Reparte cartas a los jugadores
    GAME_init_hands(&game->deck, players, game);

    // Players
    GAME_sort_by_name(players, game->total_players);
    player_list = LISTBI_create();
    for (int i = 0; i < game->total_players; ++i) {
        LISTBI_insert(&player_list, &players[i]);
    }
    game->player_list = player_list;

    // Quita una carta del deck (la que se va a jugar) y la pone en la pila de descarte
    STACK_push(&game->discard_deck, STACK_pop(&game->deck));
}

//Muestra los players junto al num total de cartas
void GAME_show_players(ListBi *player_list) {
    LISTBI_go_first(player_list);
    Player *player;
    while (player_list->pdi->next != NULL) {
        player = LISTBI_get(player_list);
        printf("\t%s \t%d cards \n\n", player->name, player->num_cards); // player.wins, player.loses
        player_list->pdi = player_list->pdi->next;
    }
}

//Show Player Cards Available
void GAME_show_cards(Player *player) {
    LIST_go_first(player->handList);
    int cont = 0;
    char value[MAXC];

    printf("%s", player->name);
    while (player->handList->last->next != NULL) {
        cont++;
        if (player->handList->last->next->card->type == 1) {
            strcpy(value, "Block Turn");
            printf("\t%d. %s - %s\n", cont, value, player->handList->last->next->card->color);
        } else if (player->handList->last->next->card->type == 2) {
            strcpy(value, "Change Direction");
            printf("\t%d. %s - %s\n", cont, value, player->handList->last->next->card->color);
        } else if (player->handList->last->next->card->type == 3) {
            strcpy(value, "Get +2");
            printf("\t%d. %s - %s\n", cont, value, player->handList->last->next->card->color);
        } else if (player->handList->last->next->card->type == 4) {
            strcpy(value, "Get +4");
            printf("\t%d. %s\n", cont, value);
        } else if (player->handList->last->next->card->type == 2) {
            strcpy(value, "Change Color");
            printf("\t%d. %s\n", cont, value);
        } else {
            printf("\t%d. %d - %s\n", cont, player->handList->last->next->card->value,
                   player->handList->last->next->card->color);
        }
        player->handList->last = player->handList->last->next;
    }
}

// Muestra los players y la cantidad de cartas
// + La carta actual
void GAME_display_game_status(Game *game) {
    Card *card = game->discard_deck.last->card;
    GAME_show_players(&game->player_list);
    printf("\t### %d %s ###\n\n", card->value, card->color); // Printa la carta actual
}

// TODO: No funciona fuera del main (No obtiene el player con player = LISTBI_get())
void GAME_display_actions(Game *game) {
    //Show actual Player cards
    Player *player;
    player = LISTBI_get(&game->player_list);

    printf("Enter an option -> %s\n", player->name);
    int option;
    do {
        option = CLI_get_action();
        switch (option) {
            case CLI_SHOW_HAND:
                GAME_show_cards(player);
                break;
            case CLI_GET_CARD:
                GAME_get_card(game);
                break;
            default:
                printf("Opcion incorrecta\n\n");
                break;
        }
    } while (option < 1 || option > 2);
}

void GAME_get_card(Game *game) {
    Player *player;
    Card *card;

    player = LISTBI_get(&game->player_list);

    card = STACK_pop(&game->deck); // Saca una carta del Deck
    LIST_insert(player->handList, card); // Inserta una carta en la PDIList

    (player->num_cards)++;

    char value[MAXC];

    if (card->type == 1) {
        strcpy(value, "Block Turn");
        printf("Se ha robado un %s - %s. ¿Deseas jugarlo? [S/N]\n", value, card->color);
    } else if (card->type == 2) {
        strcpy(value, "Change Direction");
        printf("Se ha robado un %s - %s. ¿Deseas jugarlo? [S/N]\n", value, card->color);
    } else if (card->type == 3) {
        strcpy(value, "Get +2");
        printf("Se ha robado un %s - %s. ¿Deseas jugarlo? [S/N]\n", value, card->color);
    } else if (card->type == 4) {
        strcpy(value, "Get +4");
        printf("Se ha robado un %s. ¿Deseas jugarlo? [S/N]\n", value);
    } else if (card->type == 2) {
        strcpy(value, "Change Color");
        printf("Se ha robado un %s. ¿Deseas jugarlo? [S/N]\n", value);
    } else {
        printf("Se ha robado un %d - %s. ¿Deseas jugarlo? [S/N]\n", card->value, card->color);
    }

//    GAME_show_cards(player);
}

// TODO: Bucle infinto al usar GAME_is_end()
int GAME_is_end(Game *game){
//    LISTBI_go_first(&game->player_list);
    while(game->player_list.pdi->next != NULL){
        if (game->player_list.pdi->player->num_cards == 0) {
            return 1; // Empty
        }
        game->player_list.pdi = game->player_list.pdi->next;

    }
    return 0; // Not Empty
}

// TODO: Terminar, comprobar GAME_display_actions() + no entra bien en los ifs por el \n
void GAME_play(Game *game){
    Player *player;
    LISTBI_go_first(&game->player_list);

    //Test - Obligar turno Player
//    LISTBI_next(&game->player_list);
//    LISTBI_next(&game->player_list);
    //Test - Obligar turno Player

//    while(!GAME_is_end(game)){
        player = LISTBI_get(&game->player_list);
        if(strcmp(player->type, "Player") == 0) {
            printf("1.actual: %s - %s\n", player->name, player->type);
//            GAME_display_game_status(game);
//            GAME_display_actions(game);


        } else if (strcmp(player->type, "Calmado") == 0){
            printf("2.actual: %s -%s-\n", player->name, player->type);

        } else { // Agresivo
            printf("3.actual: %s - %s-\n", player->name, player->type);
            GAME_display_game_status(game); // Debe estar en if Player
            GAME_display_actions(game); // Debe estar en if Player
        }
//        Segun la direccion de la partida - Horaria 0 / Antihoraria 1
//        if(game->direction == 0) {
//            LISTBI_next(&game->player_list);
//        }else{
//            LISTBI_previous(&game->player_list);
//        }
//    }
}