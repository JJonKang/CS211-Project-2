/// Project 2: Food Web Analysis with Dynamic Memory
/// Author: Jonathan Kang
/// Date: September 21, 2023
/// Course: CS 211, Fall 2023, UIC
/// System: Advanced Zylab
/// Description: Using a dynamically allocated struct array for the organisms, this program
/// creates a programmed food web. We can see the category an organism would be in (like a producer),
/// and can see their preys. There is also an extinction function which shows the change of the food web
/// as an animal disappears from the web.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Org_struct {
    char name[20];
    int* prey; //dynamic array of indices  
    int numPrey;
} Org;


void buildWeb(Org* web, int numOrg, int predInd, int preyInd) {
    //      Build the web by adding the predator-prey relation to the food web.
    //      Inputs: 
    //          web - a dynamically allocated array of Orgs 
    //          numOrgs - number of organisms = size of web[]
    //          predInd - predator index in web[]; an entry to its prey[] subitem will be added
    //          preyInd - prey index in web[]; will be added to predator's prey[] subitem
    //      Outputs:
    //          web - array is updated and implicitly returned (previously allocated on heap)
    //        
    //      For the predator-prey relation...
    //      (1) if the predator's prey[] array is empty, allocate memory for one index
    //          otherwise, reallocate predator's prey[] array to allow one more index
    //      (2) append the prey index as the last element of the predator's prey[] array 
    //      (3) update the numPrey subitem for the predator appropriately 
    if(web[predInd].numPrey == 0){
        web[predInd].prey = (int*)malloc(sizeof(int));
        web[predInd].prey[0] = preyInd;
        web[predInd].numPrey++;
    }
    else{
        int* tempArr = (int*)malloc((web[predInd].numPrey + 1) * sizeof(int));
        int i = 0;
        for (i; i < web[predInd].numPrey; i++){
            tempArr[i] = web[predInd].prey[i];
        }
        free(web[predInd].prey);
        web[predInd].prey = tempArr;
        web[predInd].prey[i] = preyInd;
        web[predInd].numPrey += 1;
    }




}

//From the extinction function
//Adds future.prey organism data from (*web)[i].prey[k] - 1 or (*web)[i].prey[k]
//depends on if the organism's prey is greater than the extinct animal or not
//
//web is the dynamically allocated array of Orgs, future is the dynamically allocated array of Orgs to change web in the end
//index is the extinct organism integer, i is for the for loop for organism selection
//track is for future organism tracking consistency, trackTwo is also for tracking consistency but for the for loop below
void futurePrey(Org** web, Org* future, int index, int i, int track, int trackTwo){
    for(int k = 0; k < (*web)[i].numPrey; k++){
        if((*web)[i].prey[k] != index){
            if((*web)[i].prey[k] > index){
                future[track].prey[trackTwo] = (*web)[i].prey[k] - 1;
            }
            else{
                future[track].prey[trackTwo] = (*web)[i].prey[k];
            }
            trackTwo++;
        }
    }
}

void extinction(Org** web, int* numOrgs, int index) {
    //      Remove the organism associated with [index] from web.
    //      Inputs: 
    //          web - a dynamically allocated array of Orgs 
    //          numOrgs - number of organisms = size of web[]
    //          index - organism index in web[] to remove
    //      Outputs:
    //          web - pointer passed-by-pointer; memory address of web array changes due to reallocation
    //          numOrgs - passed-by-pointer; must be decremented since web[] loses an organism
    //
    //      1. remove organism at index from web[] - DO NOT use realloc(), instead...
    //          (a) free any malloc'd memory associated with organism at index; i.e. its prey[] subitem
    //          (b) malloc new space for the array with the new number of Orgs 
    //          (c) copy all but one of the old array elements to the new array,  
    //              some require shifting forward to overwrite the organism at index
    //          (d) free the old array  
    //          (e) update the array pointer to the new array 
    //          (f) update numOrgs 
    //      2. remove index from all organsisms' prey[] array subitems - DO NOT use realloc(), instead...
    //          (a) search for index in all organisms' prey[] arrays; when index is found:
    //                [i] malloc new space for the array with the new number of ints
    //               [ii] copy all but one of the old array elements to the new array, 
    //                    keeping the same order some require shifting forward
    //              [iii] free the old array
    //               [iv] update the array pointer to the new array
    //                [v] update the numPrey subitem accordingly
    //          (b) update all organisms' prey[] elements that are greater than index, 
    //              which have been shifted forward in the web array
    //
    //          Edge case: check the size array being malloc'ed; 
    //                     for a web with only one organism and 
    //                     that orgranism goes extinct, 
    //                     instead of malloc'ing an empty array, 
    //                     explicitly set the pointer to NULL;
    //                     see the web[] allocation in main() as an example
    
    //the edge case
    if(*numOrgs == 1){
        free(*web);
        *web = NULL;
        *numOrgs = 0;
        return;
    }

    Org* future = (Org*)malloc((*numOrgs - 1) * sizeof(Org));
    int track = 0; //tracking future for everything within for loop below (besides the prey loop) when i is not the extinct animal (because of web).
    for(int i = 0; i < *numOrgs; i++){
        //checks if it's the extinct animal or not
        if(i != index){
            //used #include <studio.h> for strcpy into future[track].name
            strcpy(future[track].name, (*web)[i].name);
            future[track].numPrey = 0;
            //increases the non-extinct animal's number of prey by 1
            for(int k = 0; k < (*web)[i].numPrey; k++){
                if((*web)[i].prey[k] != index){
                    future[track].numPrey += 1;
                }
            }
            //if there's no prey at all, further calculations are no longer necessary on the organism
            //free the original *web organism's prey and call it null, continue for loop
            if(future[track].numPrey == 0){
                free((*web)[i].prey);
                future[track].prey = NULL;
                track++;
                continue;
            }
            //allocate future[track].prey
            future[track].prey = (int*)malloc(future[track].numPrey * sizeof(int));
            int trackTwo = 0; //tracker for future's prey count when web = future.
            futurePrey(web, future, index, i, track, trackTwo);
            track++;
        }
        free((*web)[i].prey);
    }

    //*web is freed then updated with future data
    //*numOrgs reduces by 1
    //carries out of function
    free(*web);
    *web = future;
    (*numOrgs)--;
}

//Sees and prints all the predators and prey that are in the web
//web is the dynamically allocated array of Orgs, numOrgs is size of web[]
void identification(Org* web, int numOrgs){
    for(int i = 0; i < numOrgs; i++){
        //the producer
        if(web[i].numPrey == 0){
            printf("  %s", web[i].name);
        }
        //the others
        else{
            printf("  %s eats %s", web[i].name, web[web[i].prey[0]].name);
            int check = 1;
            while(check < web[i].numPrey){
                printf(", %s", web[web[i].prey[check]].name);
                check++;
            }
        }
        printf("\n");
    }
}

//Sees and prints the organism that no others eat
//web is the dynamically allocated array of Orgs, numOrgs is size of web[]
void apex(Org* web, int numOrgs){
    int apexCheck[numOrgs];
    //gives all organisms 0 at the beginning
    for(int i = 0; i < numOrgs; i++){
        apexCheck[i] = 0;
    }
    //gets all organisms that eat the particular organism
    for(int i = 0; i < numOrgs; i++){
        for(int k = 0; k < web[i].numPrey; k++){
            apexCheck[web[i].prey[k]] += 1;
        }
    }
    //if 0, that means there's no predator for the organism
    for(int i = 0; i < numOrgs; i++){
        if(apexCheck[i] == 0){
            printf("  %s\n", web[i].name);
        }
    }
}

//Gets the organisms that don't eat other organisms
//web is the dynamically allocated array of Orgs, numOrgs is size of web[]
void producer(Org* web, int numOrgs){
    //uses number of prey for the organism to determine if producer
    for(int i = 0; i < numOrgs; i++){
        if(web[i].numPrey == 0){
            printf("  %s\n", web[i].name);
        }
    }
}

//Gets the organisms that eat the most variety
//web is the dynamically allocated array of Orgs, numOrgs is size of web[]
void flexible(Org* web, int numOrgs){
    int flexCheck[numOrgs];
    //gives each organism flexCheck the number of prey they eat
    for(int i = 0; i < numOrgs; i++){
        flexCheck[i] = web[i].numPrey;
    }

    //determines which organism eats the most with int most as a comparison
    int most = 0;
    for(int i = 0; i < numOrgs; i++){
        if(flexCheck[i] > most){
            most = flexCheck[i];
        }
    }

    for(int i = 0; i < numOrgs; i++){
        if(flexCheck[i] == most){
            printf("  %s\n", web[i].name);
        }
    }
}

//Gets the organisms that are eaten by the most amount of different organisms
//web is the dynamically allocated array of Orgs, numOrgs is size of web[]
void tasty(Org* web, int numOrgs){
    int tasteCheck[numOrgs];
    //gives every organism 0 for tasteCheck
    for(int i = 0; i < numOrgs; i++){
        tasteCheck[i] = 0;
    }

    //for loops are to raise tasteCheck up for an organism
    //and eventually to compare with int most
    int most = 0;
    for(int i = 0; i < numOrgs; i++){
        for(int k = 0; k < web[i].numPrey; k++){
            tasteCheck[web[i].prey[k]] += 1;
        }
    }
    for(int i = 0; i < numOrgs; i++){
        if(tasteCheck[i] > most){
            most = tasteCheck[i];
        }
    }
    for(int i = 0; i < numOrgs; i++){
        if(tasteCheck[i] == most){
            printf("  %s\n", web[i].name);
        }
    }
}

//Returns the highest value integer between two integers, a and b
//from height and heightVore function
int max(int a, int b){
    if(a >= b){
        return a;
    }
    else{
        return b;
    }
}

//Calculates and prints the length of the longest chain from a producer to each organism
//web is the dynamically allocated array of Orgs, numOrgs is size of web[]
void height(Org* web, int numOrgs){
    int heightCheck[numOrgs];
    bool change = true;
    int temp;
    //all heightCheck[] is 0
    for(int i = 0; i < numOrgs; i++){
        heightCheck[i] = 0;
    }

    //counts the chains repeatedly/continually until there is no more chain using bool change
    while(change == true){
        change = false;
        for(int i = 0; i < numOrgs; i++){
            temp = heightCheck[i];
            int k = 0;
            for(k; k < web[i].numPrey; k++){
                heightCheck[i] = max(1 + heightCheck[web[i].prey[k]], heightCheck[i]);
            }
            if(temp != heightCheck[i]){
                change = true;
            }
        }
    }
    for(int i = 0; i < numOrgs; i++){
        printf("  %s: %d\n", web[i].name, heightCheck[i]);
    }
}

//Classifies all organisms and prints each group (producers, herbivores, omnivores, & carnivores)
//web is the dynamically allocated array of Orgs, numOrgs is size of web[]
//an alteration of height
void heightVore(Org* web, int numOrgs){
    int heightCheck[numOrgs];
    bool change = true;
    int temp;
    //all heightCheck[] is 0
    for(int i = 0; i < numOrgs; i++){
        heightCheck[i] = 0;
    }

    //counts the chains repeatedly/continually until there is no more chain using bool change
    //for heightVore function this is used to categorize the organisms into their group based on their position on the chain
    while(change == true){
        change = false;
        for(int i = 0; i < numOrgs; i++){
            temp = heightCheck[i];
            int k = 0;
            for(k; k < web[i].numPrey; k++){
                heightCheck[i] = max(1 + heightCheck[web[i].prey[k]], heightCheck[i]);
            }
            if(temp != heightCheck[i]){
                change = true;
            }
        }
    }
    
    printf("  Producers:\n");
    for(int i = 0; i < numOrgs; i++){
        if(heightCheck[i] == 0){
            printf("    %s\n", web[i].name);
        }
    }

    printf("  Herbivores:\n");
    for(int i = 0; i < numOrgs; i++){
        if(heightCheck[i] == 1){
            printf("    %s\n", web[i].name);
        }
    }

    //both bools are to check if the organism eats BOTH producers and nonproducers
    printf("  Omnivores:\n");
    bool zero = false;
    bool nonzero = false;
    for(int i = 0; i < numOrgs; i++){
        for(int k = 0; k < web[i].numPrey; k++){
            if(heightCheck[web[i].prey[k]] == 0){
                zero = true;
            }
            else if(heightCheck[web[i].prey[k]] >= 1){
                nonzero = true;
            }
        }
        if(zero == true && nonzero == true){
            printf("    %s\n", web[i].name);
        }
        zero = false;
        nonzero = false;
    }

    //both bools are to check if the organism only eats nonproducers
    printf("  Carnivores:\n");
    for(int i = 0; i < numOrgs; i++){
        for(int k = 0; k < web[i].numPrey; k++){
            if(heightCheck[web[i].prey[k]] == 0){
                zero = true;
            }
            else if(heightCheck[web[i].prey[k]] >= 1){
                nonzero = true;
            }
        }
        if(zero != true && nonzero == true){
            printf("    %s\n", web[i].name);
        }
        zero = false;
        nonzero = false;
    }
}

int main(void) {

    int numOrgs;
    printf("Welcome to the Food Web Application\n");
    printf("--------------------------------\n");
    printf("Enter number of organisms:\n");
    scanf("%d",&numOrgs);

    Org* web = NULL;
    if(numOrgs > 0) { //Do not malloc an empty array, leave it pointing to NULL
        web = (Org*)malloc(numOrgs*sizeof(Org));
    }
    
    printf("Enter names for %d organisms:\n", numOrgs);
    for (int i = 0; i < numOrgs; ++i) {
        scanf("%s",web[i].name);
        web[i].prey = NULL;
        web[i].numPrey = 0;
    }

    printf("Enter number of predator/prey relations:\n");
    int numRels;
    scanf("%d",&numRels);

    printf("Enter the pair of indices for the %d predator/prey relations\n",numRels);
    printf("the format is [predator index] [prey index]:\n");
    
    int predInd, preyInd;
    for (int i = 0; i < numRels; ++i) {
        scanf("%d %d",&predInd, &preyInd);
        buildWeb(web,numOrgs,predInd,preyInd);
    }
    printf("--------------------------------\n\n");

    printf("Food Web Predators & Prey:\n");
    //Prints the Food Web Organisms with what they eat (i.e. prey)
    identification(web, numOrgs);
    printf("\n");

    printf("Apex Predators:\n");
    //Identifies and print the organisms not eaten by any others
    apex(web, numOrgs);
    printf("\n");

    printf("Producers:\n");
    //Identifies and print the organisms that eat no other organisms
    producer(web, numOrgs);
    printf("\n");

    printf("Most Flexible Eaters:\n");
    //Identifies and prints the organism(s) with the most prey
    flexible(web, numOrgs);
    printf("\n");

    printf("Tastiest Food:\n");
    //Identifies and prints organism(s) eaten by the most other organisms
    tasty(web, numOrgs);
    printf("\n");

    printf("Food Web Heights:\n");
    //Calculates and prints the length of the longest chain from a producer to each organism
    height(web, numOrgs);
    printf("\n");

    printf("Vore Types:\n");
    //Classifies all organisms and prints each group (producers, herbivores, omnivores, & carnivores)
    heightVore(web, numOrgs);
    printf("\n");

    printf("--------------------------------\n");
    int extInd;
    printf("Enter extinct species index:\n");
    scanf("%d",&extInd);
    printf("Species Extinction: %s\n", web[extInd].name);
    extinction(&web,&numOrgs,extInd);
    printf("--------------------------------\n\n");


    printf("UPDATED Food Web Predators & Prey:\n");
    //      Prints the UPDATED Food Web Organisms with what they eat (i.e. prey), AFTER THE EXTINCTION
    identification(web, numOrgs);
    printf("\n");

    printf("UPDATED Apex Predators:\n");
    //      AFTER THE EXTINCTION, identifies and prints the organisms not eaten by any other
    apex(web, numOrgs);
    printf("\n");

    printf("UPDATED Producers:\n");
    //      AFTER THE EXTINCTION, identifies and prints the organisms that eat no other organisms
    producer(web, numOrgs);
    printf("\n");

    printf("UPDATED Most Flexible Eaters:\n");
    //      AFTER THE EXTINCTION, identifies and prints the organism(s) with the most prey
    flexible(web, numOrgs);
    printf("\n");

    printf("UPDATED Tastiest Food:\n");
    //    AFTER THE EXTINCTION, identifies and prints organism(s) eaten by the most other organisms
    tasty(web, numOrgs);
    printf("\n");

    printf("UPDATED Food Web Heights:\n");
    //      AFTER THE EXTINCTION, calculates and prints the length of the longest chain from a 
    //      producer to each organism
    height(web, numOrgs);
    printf("\n");

    printf("UPDATED Vore Types:\n");
    //      AFTER THE EXTINCTION, classifies all organisms and print each group
    //      (producers, herbivores, omnivores, & carnivores)
    heightVore(web, numOrgs);
    printf("\n");
    printf("--------------------------------\n");

    //Frees all malloc'd memory to prevent potential leaks
    for(int i = 0; i < numOrgs; i++){
        free(web[i].prey);
    }
    free(web);
    return 0;
}
