#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void round1votes(int rank, int st, int en, int num_candidate, int *votes, int (*preference)[num_candidate]);
void round2votes(int rank, int st, int en, int num_candidate, int *votes, int (*preference)[num_candidate], int highCandione, int maxtow);

int main(int argc, char *argv[]) {
    int rank, size, tag = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int num_candidate, num_votes;
    char filename[100] = ""; 

    if(rank == 0){
        printf("Enter 1 to generate file  \n");
        printf("Enter 2 to calculate the result \n");
        int option;
        scanf("%d", &option);
        if(option == 1){
            strcat(filename, "paper_election.txt");
            FILE *file = fopen("paper_election.txt", "w");
            printf("Enter number of candidates: ");
            scanf("%d", &num_candidate);
            printf("Enter number of voters: ");
            scanf("%d", &num_votes);
            fprintf(file, "%d\n%d\n", num_candidate, num_votes);
            for(int i = 0; i < num_votes; i++){
                printf("Enter voter %d preferences: ", i+1);
                for(int j = 0; j < num_candidate; j++){
                    int x;
                    scanf("%d", &x);
                    fprintf(file, "%d ", x);
                }
                fprintf(file, "\n");
            }
            fclose(file);
        } else {
            printf("Enter the filename: ");
            scanf("%s", filename);
            FILE *file = fopen(filename, "r");
            if(file == NULL){
                printf("File not found\n");
                MPI_Finalize();
                return 0;
            }
            fclose(file);
        }
    }
    MPI_Bcast(filename, 100, MPI_CHAR, 0, MPI_COMM_WORLD);

    FILE *file = fopen(filename, "r");
    fscanf(file, "%d\n%d\n", &num_candidate, &num_votes);
    int votes[num_candidate];
    int preference[num_votes][num_candidate];
    int (*Pp)[num_candidate] = preference;
    memset(votes, 0, sizeof(votes));
    memset(preference, 0, sizeof(preference));

    
    int p = num_votes / size;
    int st, en=0;
    if(p == 0)//This implies that the number of votes is less than the number of processes, so each process will handle a single vote.
    { 
        if(rank > num_votes - 1)//it means this process doesn't have any votes to handle, so
        {
            st = 0;
            en = 0;
        } else {
            st = rank + 2;
            en = rank + 3;
        }
    } 
    else 
    {
        st = rank * p + 2;
        if (rank == size - 1) 
        {
            en = num_votes + 2;
        } else {
            en = st + p;
        }
    }

    fseek(file, 0, SEEK_SET); // moves the file position indicator to the beginning of the file.
    for(int i = 0; i < num_votes + 2; i++){
        if(i < 2){//The first two lines are skipped because they contain the number of candidates and votes, not voter preferences.
            int temp;
            fscanf(file, "%d", &temp);
            continue;
        }
        if(i < st){
            for(int j = 0; j < num_candidate; j++){ // the preferences on that line are read and discarded because they are not in the range of the process
                int x;
                fscanf(file, "%d", &x);
            }
        } else if(i >= st && i < en){ // preferences on that line are read into the preference
            for(int j = 0; j < num_candidate; j++){
                fscanf(file, "%d", &preference[i-2][j]);
            }
        } else {
            break;
        }
    }
    fclose(file);

    printf("Process %d received filename: %s\n", rank, filename);
    printf("Process %d read num_candidates: %d, num_voters: %d\n", rank, num_candidate, num_votes);
    printf("Process %d: start: %d, end: %d\n", rank, st, en);

    round1votes(rank, st, en, num_candidate, votes, Pp);

    int totalNumVotes[num_candidate];
    MPI_Reduce(votes, totalNumVotes, num_candidate, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    int first_higher_vote = -1, highCandione = -1, second_higher_vote = -1, highCanditwo = -1;
    if(rank == 0){  
        for(int i = 0; i < num_candidate; i++){
            if(totalNumVotes[i] > first_higher_vote){
                second_higher_vote = first_higher_vote;
                highCanditwo = highCandione;
                first_higher_vote = totalNumVotes[i];
                highCandione = i;
            } else if(totalNumVotes[i] > second_higher_vote){
                second_higher_vote = totalNumVotes[i];
                highCanditwo = i;
            }
        }
        if(first_higher_vote > (double) num_votes/2){
            printf("Candidate %d wins in the first round with %d votes (%.2f%%)\n", highCandione+1, first_higher_vote, (first_higher_vote / (float)num_votes) * 100);
        } else {
            printf("Note: No one won in the first round and Candidate %d and Candidate %d will compete in the second round\n", highCandione+1, highCanditwo+1);
            printf("Percentage of votes per candidate in the first round:\n");
            for(int i = 0; i < num_candidate; i++)
            {
                printf("Candidate %d: %.2f%%\n", i+1, (totalNumVotes[i] / (float)num_votes) * 100);
            
            }
        }
    }
    MPI_Bcast(&highCandione, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&highCanditwo, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(!(first_higher_vote > (double) num_votes/2)){
        memset(votes, 0, sizeof(votes));
        round2votes(rank, st, en, num_candidate, votes, Pp, highCandione+1, highCanditwo+1);
        MPI_Reduce(votes, totalNumVotes, num_candidate, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(rank == 0){
            int second_higher_vote = 0, highCanditwo = 0;
            for(int i = 0; i < num_candidate; i++){
                if(totalNumVotes[i] > second_higher_vote){
                    second_higher_vote = totalNumVotes[i];
                    highCanditwo = i;
                }
            }
            printf("Percentage of votes per candidate in the second round:\n");
            for(int i = 0; i < num_candidate; i++){
                printf("Candidate %d: %.2f%%\n", i+1, (totalNumVotes[i] / (float)num_votes) * 100);
            }
            printf("Final Result : \nCandidate %d wins in the second round with %d votes (%.2f%%)\n", highCanditwo+1, second_higher_vote, (second_higher_vote / (float)num_votes) * 100 );
        }
    }
    MPI_Finalize();
    return 0;
}

void round1votes(int rank, int st, int en, int num_candidate, int *votes, int (*preference)[num_candidate]) {
    for(int i = st; i < en; i++){
        int index = preference[i-2][0];
        votes[index-1]++;
    }
}

void round2votes(int rank, int st, int en, int num_candidate, int *votes, int (*preference)[num_candidate], int highCandione, int maxtow) {
    for(int i = st; i < en; i++){
        for(int j = 0; j < num_candidate; j++){
            int index = preference[i-2][j];
            if(index == highCandione || index == maxtow){
                votes[index-1]++;
                break;
            }
        }
    }
}
