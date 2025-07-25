#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HAND_SIZE 5

typedef struct {
    int rank; // 2–14
    char suit;
} Card;

typedef struct {
    int hand_rank;      // 0 = high card, 1 = pair, ..., 9 = royal flush
    int tiebreakers[5]; // sorted values for breaking ties
} HandValue;

// Card rank mapping
int rank_value(char r) {
    if (r >= '2' && r <= '9') return r - '0';
    switch (r) {
        case 'T': return 10;
        case 'J': return 11;
        case 'Q': return 12;
        case 'K': return 13;
        case 'A': return 14;
    }
    return -1;
}

int cmp_desc(const void *a, const void *b) {
    return (*(int*)b) - (*(int*)a);
}

int count_ranks(const Card hand[], int counts[15]) {
    for (int i = 0; i < 15; i++) counts[i] = 0;
    for (int i = 0; i < HAND_SIZE; i++)
        counts[hand[i].rank]++;
    return 0;
}

int is_flush(const Card hand[]) {
    char suit = hand[0].suit;
    for (int i = 1; i < HAND_SIZE; i++)
        if (hand[i].suit != suit) return 0;
    return 1;
}

int is_straight(const int ranks[HAND_SIZE]) {
    for (int i = 0; i < HAND_SIZE - 1; i++)
        if (ranks[i] - 1 != ranks[i + 1]) return 0;
    return 1;
}

void evaluate_hand(const Card hand[], HandValue *value) {
    int counts[15];
    int rank_list[HAND_SIZE];
    count_ranks(hand, counts);

    int idx = 0;
    for (int r = 14; r >= 2; r--) {
        for (int c = 0; c < counts[r]; c++) {
            rank_list[idx++] = r;
        }
    }

    int freq[5] = {0};
    int four = 0, three = 0, pairs = 0;
    int pair_ranks[2] = {0}, pair_idx = 0;
    int three_rank = 0, four_rank = 0;
    for (int r = 2; r <= 14; r++) {
        if (counts[r] == 4) { four = 1; four_rank = r; }
        else if (counts[r] == 3) { three = 1; three_rank = r; }
        else if (counts[r] == 2) { pairs++; pair_ranks[pair_idx++] = r; }
    }

    int sorted[HAND_SIZE];
    memcpy(sorted, rank_list, sizeof(sorted));
    qsort(sorted, HAND_SIZE, sizeof(int), cmp_desc);

    int flush = is_flush(hand);
    int straight = is_straight(sorted);

    // Handle special straight (A-2-3-4-5)
    int low_straight = (sorted[0] == 14 && sorted[1] == 5 &&
                        sorted[2] == 4 && sorted[3] == 3 &&
                        sorted[4] == 2);

    // Assign hand rank
    if (flush && (straight || low_straight)) {
        value->hand_rank = (sorted[0] == 14 && sorted[1] == 13) ? 9 : 8; // Royal or Straight Flush
        memcpy(value->tiebreakers, sorted, sizeof(sorted));
    } else if (four) {
        value->hand_rank = 7;
        value->tiebreakers[0] = four_rank;
        for (int i = 0, j = 1; i < HAND_SIZE; i++)
            if (rank_list[i] != four_rank)
                value->tiebreakers[j++] = rank_list[i];
    } else if (three && pairs == 1) {
        value->hand_rank = 6; // Full House
        value->tiebreakers[0] = three_rank;
        value->tiebreakers[1] = pair_ranks[0];
    } else if (flush) {
        value->hand_rank = 5;
        memcpy(value->tiebreakers, sorted, sizeof(sorted));
    } else if (straight || low_straight) {
        value->hand_rank = 4;
        if (low_straight) {
            value->tiebreakers[0] = 5;
        } else {
            value->tiebreakers[0] = sorted[0];
        }
    } else if (three) {
        value->hand_rank = 3;
        value->tiebreakers[0] = three_rank;
        for (int i = 0, j = 1; i < HAND_SIZE; i++)
            if (rank_list[i] != three_rank)
                value->tiebreakers[j++] = rank_list[i];
    } else if (pairs == 2) {
        value->hand_rank = 2;
        qsort(pair_ranks, 2, sizeof(int), cmp_desc);
        value->tiebreakers[0] = pair_ranks[0];
        value->tiebreakers[1] = pair_ranks[1];
        for (int i = 0; i < HAND_SIZE; i++)
            if (rank_list[i] != pair_ranks[0] && rank_list[i] != pair_ranks[1])
                value->tiebreakers[2] = rank_list[i];
    } else if (pairs == 1) {
        value->hand_rank = 1;
        value->tiebreakers[0] = pair_ranks[0];
        for (int i = 0, j = 1; i < HAND_SIZE; i++)
            if (rank_list[i] != pair_ranks[0])
                value->tiebreakers[j++] = rank_list[i];
    } else {
        value->hand_rank = 0;
        memcpy(value->tiebreakers, sorted, sizeof(sorted));
    }
}

int compare_hands(const Card h1[], const Card h2[]) {
    HandValue v1, v2;
    evaluate_hand(h1, &v1);
    evaluate_hand(h2, &v2);

    if (v1.hand_rank > v2.hand_rank) return 1;
    if (v2.hand_rank > v1.hand_rank) return -1;

    for (int i = 0; i < 5; i++) {
        if (v1.tiebreakers[i] > v2.tiebreakers[i]) return 1;
        if (v2.tiebreakers[i] > v1.tiebreakers[i]) return -1;
    }
    return 0;
}

void parse_input(const char *input, Card p1[], Card p2[]) {
    for (int i = 0; i < 5; i++) {
        p1[i].rank = rank_value(input[i * 3]);
        p1[i].suit = input[i * 3 + 1];
    }
    for (int i = 0; i < 5; i++) {
        p2[i].rank = rank_value(input[(i + 5) * 3]);
        p2[i].suit = input[(i + 5) * 3 + 1];
    }
}

int main() {
    int player_1_wins = 0;
    int player_2_wins = 0;

    FILE *fptr = fopen("poker_hands.txt", "r");
    if (!fptr) {
        printf("Failed to open file.\n");
        return 1;
    }

    char line[64];

    while (fgets(line, sizeof(line), fptr)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline if any
        printf("Processing: %s\n", line);

        Card p1[HAND_SIZE], p2[HAND_SIZE];
        parse_input(line, p1, p2);
        int result = compare_hands(p1, p2);

        if (result == 1) {
            player_1_wins++;
            printf("Player 1 wins (Total: %d)\n", player_1_wins);
        } else if (result == -1) {
            player_2_wins++;
            printf("Player 2 wins (Total: %d)\n", player_2_wins);
        } else {
            printf("Tie or error on input: %s\n", line);
        }
    }

    printf("\nFinal Results:\nPlayer 1 won %d times\nPlayer 2 won %d times\n", player_1_wins, player_2_wins);
    fclose(fptr);
    return 0;
}
