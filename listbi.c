#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "listbi.h"
#include "player.h"

ListBi LISTBI_create(){
    ListBi list;

    list.first = (NodeBi *) malloc(sizeof(NodeBi));
    list.last = (NodeBi *) malloc(sizeof(NodeBi));

    if (list.first == NULL || list.last == NULL){
        printf("Error al crear la lista!\n");

        if(list.first != NULL){
            free(list.first);
        }
        if(list.last != NULL){
            free(list.last);
        }

    } else{
        list.pdi = list.first;
        list.first->next = list.last; //Apuntem un al altre
        list.first->prev = NULL; //Els 2 fantasmes apunten a NULL (1r.Prev y 2n.next)

        list.last->next = NULL; //Els 2 fantasmes apunten a NULL (1r.Prev y 2n.next)
        list.last->prev = list.first;//Apuntem un al altre
    }

    return list;
}

int LISTBI_insert(ListBi *list, Player *player){
    NodeBi *n = (NodeBi *) malloc(sizeof(NodeBi));

    if (n == NULL) {
        printf("Error al insertar una canción!\n");
    } else {
        n->player = *player; //Asignar song a N
        n->next = list->pdi->next; // El Next de N es el antiguo Next del PDI
        n->prev = list->pdi; // El Prev de N es el antiguo PDI
        list->pdi->next->prev = n; // El que iba despues del antiguo PDI, su Prev pasa a ser N
        list->pdi->next = n; // El next del antiguo PDI es N
        list->pdi = n; // El nuevo PDI es N
    }

    return 1;
}

void LISTBI_go_first(ListBi *list){
    list->pdi = list->first->next;
}

//Util para pasar los turnos en direccion Horaria
int LISTBI_next(ListBi *list){
    if (list->pdi != list->first && list->pdi != list->last) {
        if(list->pdi->next == list->last){
            list->pdi = list->first->next;
        }else{
            list->pdi = list->pdi->next;
        }
    }
    return 1;
}

//TODO: Create LISTBI_previous(&game->player_list);
int LISTBI_previous(ListBi *list){
    if (list->pdi != list->first && list->pdi != list->last) {
        if(list->pdi->prev == list->first){
            list->pdi = list->last->prev;
        }else{
            list->pdi = list->pdi->prev;
        }
    }
    return 1;
}

Player LISTBI_get(ListBi *list){
    if (list->pdi == list->first || list->pdi == list->last) {
        printf("No existe ningun jugador!\n\n");
    }
    return list->pdi->player;
}

void LISTBI_show_list(ListBi *player_list) {
    Player player;
    NodeBi *n = player_list->first->next;
    while (n->next != NULL) {
        player = n->player;
        printf("\t%s - %d cards \n\n", player.name, player.num_cards); // player.wins, player.loses
        n = n->next;
    }
}