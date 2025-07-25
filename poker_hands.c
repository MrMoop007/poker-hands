#include <stdio.h>
#include <string.h>

int hand_winner(char hand1[14], char hand2[14]){

}

int main(){

    FILE *fptr;

    fptr = fopen("poker_hands.txt", "r");

    char line[29];

    int player_1_wins = 0;
    int player_2_wins = 0;

    for(int i = 0; i < 2000; i++){
        fgets(line, 29, fptr);
        char hand1[14] = strncpy(line, 0, 14);
        char hand2[14] = strncpy(line, line+15, 14);
        if(hand_winner(hand1, hand2) == 1){player_1_wins++;}
        else{player_2_wins++;}
    }

    printf("Player 1 won %d hands, and player 2 won %d hands", player_1_wins, player_1_wins);

    fclose(fptr);
    return 0;
}