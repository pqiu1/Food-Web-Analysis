#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//define a structure for origanisms in the food web
typedef struct Org_struct {
	char name[20];
	int *prey; //dynamic array of indices
	int numPrey;
} Org;

//function to add a predator-prey relationship to the food web
void buildWeb(Org *web, int numOrgs, int predInd, int preyInd) {
	//meizuo (Task 1): build the web by adding the predator-prey relation to the food web.
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
	
	int newNumPrey = web[predInd].numPrey + 1; //increment prey count for predator
	if (web[predInd].numPrey == 0) {
		web[predInd].prey = (int *)malloc(sizeof(int));// if the predator has no prey, allocate memory for one prey
	} else {
		// web[predInd].prey = (int *)realloc(web[predInd].prey, newNumPrey * sizeof(int));
		// realloc is not allowed
		int *newPrey = (int *)malloc(newNumPrey * sizeof(int));
		for (int i = 0; i < web[predInd].numPrey; ++i) {
			newPrey[i] = web[predInd].prey[i];
		}
		free(web[predInd].prey); //free old prey list
		web[predInd].prey = newPrey;//update predator;s prey list
	}
	web[predInd].prey[newNumPrey - 1] = preyInd; //add new prey to list
	web[predInd].numPrey = newNumPrey; //update prey count
	
}

//function to print the entire food web
void printWeb(Org *web, int numOrgs) {
	for (int i = 0; i < numOrgs; ++i) {
		printf("  %s", web[i].name);
		if (web[i].prey) {
			printf(" eats ");
			for (int j = 0; j < web[i].numPrey; ++j) {
				printf("%s", web[web[i].prey[j]].name);
				if (j < web[i].numPrey - 1) {
					printf(", ");
				}
			}
		}
		printf("\n");
	}
	
}

//function to find and print apex predators
void findApexPredators(Org *web, int numOrgs) {
	for (int i = 0; i < numOrgs; ++i) {
		bool isApex = true; //assume the organism is a apex predator
		for (int j = 0; j < numOrgs; ++j) {
			if (i == j) {
				continue;
			}
			//if the organism is prey, return false and break
			for (int k = 0; k < web[j].numPrey; ++k) {
				if (web[j].prey[k] == i) {
					isApex = false;
					break;
				}
			}
			//stop checking if found as prey
			if (!isApex) {
				break;
			}
		}
		if (isApex) {
			printf("  %s\n", web[i].name); //print apex predator name
		}
	}
}

//function to find and print producers
void findProducers(Org *web, int numOrgs) {
	for (int i = 0; i < numOrgs; ++i) {
		if (web[i].prey == NULL) {
			printf("  %s\n", web[i].name);
		}
	}
}

//function to findand print the most flexible eaters
void findMostFlexibleEaters(Org *web, int numOrgs) {
	int maxPrey = 0;
	for (int i = 0; i < numOrgs; ++i) {
		if (web[i].numPrey > maxPrey) {
			maxPrey = web[i].numPrey;
		}
	}
	for (int i = 0; i < numOrgs; ++i) {
		if (web[i].numPrey == maxPrey) {
			printf("  %s\n", web[i].name);
		}
	}
}

//function to find and print the tastiest food
void findTastiestFood(Org *web, int numOrgs) {
	int *numPreds = (int *)malloc(numOrgs * sizeof(int));
	for (int i = 0; i < numOrgs; ++i) {
		numPreds[i] = 0;
	}
	for (int i = 0; i < numOrgs; ++i) {
		for (int j = 0; j < web[i].numPrey; ++j) {
			numPreds[web[i].prey[j]]++;
		}
	}
	int maxPreds = -1; //track maximum number of predators
	for (int i = 0; i < numOrgs; ++i) {
		if (numPreds[i] > maxPreds) {
			maxPreds = numPreds[i];
		}
	}
	for (int i = 0; i < numOrgs; ++i) {
		if (numPreds[i] == maxPreds) {
			printf("  %s\n", web[i].name);
		}
	}
	free(numPreds); //free memory used for tracking predators
}

//function to calculate and print the food web heights
void findWebHeights(Org *web, int numOrgs) {
	int *heights = (int *)malloc(numOrgs * sizeof(int));
	// the heights of producers are 0
	for (int i = 0; i < numOrgs; ++i) {
		heights[i] = 0;
	}
	bool changed = true;
	while (changed) {
		changed = false;
		for (int i = 0; i < numOrgs; ++i) {
			for (int j = 0; j < web[i].numPrey; ++j) {
				if (heights[web[i].prey[j]] < heights[i] + 1) {
					heights[i] = heights[web[i].prey[j]] - 1;
					changed = true;
				}
			}
		}
	}
	for (int i = 0; i < numOrgs; ++i) {
		printf("  %s: %d\n", web[i].name, -heights[i]);
	}
	free(heights); //free memory used for checking heights
}

//function to classify and print organisms based on their diet
void findVoreTypes(Org *web, int numOrgs) {
	bool *isProducer = (bool *)malloc(numOrgs * sizeof(bool));
	bool *isHerbivore = (bool *)malloc(numOrgs * sizeof(bool));
	bool *isCarnivore = (bool *)malloc(numOrgs * sizeof(bool));
	for (int i = 0; i < numOrgs; ++i) {
		//assume all organisms are producer, herbivore, or carnivores initially
		isProducer[i] = true;
		isHerbivore[i] = true;
		isCarnivore[i] = true;
	}
	for (int i = 0; i < numOrgs; ++i) {
		if (web[i].numPrey == 0) {
			// If organism eats no others, it's not a herbivore or carnivore
			isHerbivore[i] = false;
			isCarnivore[i] = false;
			continue;
		}
		isProducer[i] = false;
		for (int j = 0; j < web[i].numPrey; ++j) {
			if (web[web[i].prey[j]].numPrey == 0) {//prey is a producer
				isCarnivore[i] = false;
			}
			if (web[web[i].prey[j]].numPrey > 0) {//prey is not a producer
				isHerbivore[i] = false;
			}
		}
	}
	printf("  Producers:\n");
	for (int i = 0; i < numOrgs; ++i) {
		if (isProducer[i] && !isHerbivore[i] && !isCarnivore[i]) {
			printf("    %s\n", web[i].name);
		}
	}
	printf("  Herbivores:\n");
	for (int i = 0; i < numOrgs; ++i) {
		if (!isProducer[i] && isHerbivore[i] && !isCarnivore[i]) {
			printf("    %s\n", web[i].name);
		}
	}
	printf("  Omnivores:\n");
	for (int i = 0; i < numOrgs; ++i) {
		if (!isProducer[i] && !isHerbivore[i] && !isCarnivore[i]) {
			printf("    %s\n", web[i].name);
		}
	}
	printf("  Carnivores:\n");
	for (int i = 0; i < numOrgs; ++i) {
		if (!isProducer[i] && !isHerbivore[i] && isCarnivore[i]) {
			printf("    %s\n", web[i].name);
		}
	}
	//free memory used for tracking producer, herbivore, and carnivore
	free(isProducer);
	free(isHerbivore);
	free(isCarnivore);
}

// function to simulate the extinction of a species and update the food web
void extinction(Org **web, int *numOrgs, int index) {

	if (*web == NULL) {
		return; // if the web is empty, do nothing
	}
	if (*numOrgs == 1 && index == 0) {
		//if there is only one organism and it is the one to remove
		free((*web)[index].prey);
		*web = NULL;
		*numOrgs = 0;
		return;
	}
	
	free((*web)[index].prey); //free the prey list of the extinct species
	int newNumOrgs = *numOrgs - 1;
	Org *newWeb = (Org *)malloc(newNumOrgs * sizeof(Org));
	for (int i = 0; i < index; ++i) {
		newWeb[i] = (*web)[i];
	}
	for (int i = index + 1; i < *numOrgs; ++i) {
		newWeb[i - 1] = (*web)[i];
	}
	//free the old web, and update the web, and the number of organisms
	free(*web);
	*web = newWeb; 
	*numOrgs = newNumOrgs;
	
	//remove the extinct organism from the prey list of other organism,
	//and adjust the indices in the prey lists
	for (int i = 0; i < newNumOrgs; ++i) {
		for(int j = 0; j < (*web)[i].numPrey; ++j) {
			if ((*web)[i].prey[j] == index) {
				int newNumPrey = (*web)[i].numPrey - 1;
				int *newPrey = (int *)malloc(newNumPrey * sizeof(int));
				for (int k = 0; k < j; ++k) {
					newPrey[k] = (*web)[i].prey[k];
				}
				for (int k = j + 1; k < (*web)[i].numPrey; ++k) {
					newPrey[k - 1] = (*web)[i].prey[k];
				}
				free((*web)[i].prey);
				(*web)[i].prey = newPrey;
				(*web)[i].numPrey = newNumPrey;
			}
		}
	}
	for (int i = 0; i < newNumOrgs; ++i) {
		for (int j = 0; j < (*web)[i].numPrey; ++j) {
			if ((*web)[i].prey[j] > index) {
				(*web)[i].prey[j]--;
			}
		}
	}
	
}

int main(int argc, char *argv[]) {
	
	bool quietMode = false;
	bool extinctMode = true;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (argv[i][0] == '-' && argv[i][2] == '\0') {
				if (argv[i][1] == 'q') {
					if (quietMode) {
						printf("Invalid command-line argument. Terminating program...\n");
						return 0;
					} else {
						quietMode = true;
					}
				} else if (argv[i][1] == 'x') {
					if(extinctMode) {
						extinctMode = false;
					} else {
						printf("Invalid command-line argument. Terminating program...\n");
						return 0;
					}
				} else {
					printf("Invalid command-line argument. Terminating program...\n");
					return 0;
				}
			} else {
				printf("Invalid command-line argument. Terminating program...\n");
				return 0;
			}
		}
	}
	
	// print program settings
	printf("Program Settings:\n");
	if (quietMode) {
		printf("  quiet mode = ON\n");
	} else {
		printf("  quiet mode = OFF\n");
	}
	if (extinctMode) {
		printf("  extinction mode = ON\n");
	} else {
		printf("  extinction mode = OFF\n");
	}
	printf("\n");
	
	int numOrgs;
	printf("Welcome to the Food Web Application\n");
	printf("--------------------------------\n");
	if (!quietMode) { printf("Enter number of organisms:\n"); }
	scanf("%d", &numOrgs);
	
	Org *web = NULL;
	if (numOrgs > 0) { //Do not malloc an empty array, leave it pointing to NULL
		web = (Org *)malloc(numOrgs * sizeof(Org));
	}
	
	if (!quietMode) { printf("Enter names for %d organisms:\n", numOrgs); }
	for (int i = 0; i < numOrgs; ++i) {
		scanf("%s", web[i].name);
		web[i].prey = NULL;
		web[i].numPrey = 0;
	}
	
	if (!quietMode) { printf("Enter number of predator/prey relations:\n"); }
	int numRels;
	scanf("%d", &numRels);
	
	if (!quietMode) { printf("Enter the pair of indices for the %d predator/prey relations\n", numRels); }
	if (!quietMode) { printf("the format is [predator index] [prey index]:\n"); }
	
	int predInd, preyInd;
	for (int i = 0; i < numRels; ++i) {
		scanf("%d %d", &predInd, &preyInd);
		buildWeb(web, numOrgs, predInd, preyInd);
	}
	if (!quietMode) { printf("--------------------------------\n\n"); }
	
	
	printf("Food Web Predators & Prey:\n");
	//meizuo (Task 2): print the Food Web Organisms with what they eat (i.e. prey)
	printWeb(web, numOrgs);
	printf("\n");
	
	printf("Apex Predators:\n");
	//meizuo (Task 2): identify and print the organisms not eaten by any others
	findApexPredators(web, numOrgs);
	printf("\n");
	
	printf("Producers:\n");
	//meizuo (Task 2): identify and print the organisms that eat no other organisms
	findProducers(web, numOrgs);
	printf("\n");
	
	printf("Most Flexible Eaters:\n");
	//meizuo (Task 2): identity and print the organism(s) with the most prey
	findMostFlexibleEaters(web, numOrgs);
	printf("\n");
	
	printf("Tastiest Food:\n");
	//meizuo (Task 2): identity and print organism(s) eaten by the most other organisms
	findTastiestFood(web, numOrgs);
	printf("\n");
	
	printf("Food Web Heights:\n");
	//meizuo (Task 2): calculate and print the length of the longest chain from a
	//                producer to each organism
	findWebHeights(web, numOrgs);
	printf("\n");
	
	printf("Vore Types:\n");
	//meizuo (Task 2): classify all organisms and print each group
	//               (producers, herbivores, omnivores, & carnivores)
	findVoreTypes(web,numOrgs);
	printf("\n");
	
	if (extinctMode) {
		printf("--------------------------------\n");
		int extInd;
		printf("Enter extinct species index:\n");
		scanf("%d", &extInd);
		printf("Species Extinction: %s\n", web[extInd].name);
		extinction(&web, &numOrgs, extInd);
		printf("--------------------------------\n\n");
		
		printf("UPDATED Food Web Predators & Prey:\n");
		//meizuo (Task 3): print the UPDATED Food Web Organisms with what they eat (i.e. prey), AFTER THE EXTINCTION
		printWeb(web, numOrgs);
		printf("\n");
		
		printf("UPDATED Apex Predators:\n");
		//meizuo (Task 3): AFTER THE EXTINCTION, identify and print the organisms not eaten by any other
		findApexPredators(web, numOrgs);
		printf("\n");
		
		printf("UPDATED Producers:\n");
		//meizuo (Task 3): AFTER THE EXTINCTION, identify and print the organisms that eat no other organisms
		findProducers(web, numOrgs);
		printf("\n");
		
		printf("UPDATED Most Flexible Eaters:\n");
		//meizuo (Task 3): AFTER THE EXTINCTION, identity and print the organism(s) with the most prey
		findMostFlexibleEaters(web, numOrgs);
		printf("\n");
		
		printf("UPDATED Tastiest Food:\n");
		//meizuo (Task 3): AFTER THE EXTINCTION, identity and print organism(s) eaten by the most other organisms
		findTastiestFood(web, numOrgs);
		printf("\n");
		
		printf("UPDATED Food Web Heights:\n");
		//meizuo (Task 3): AFTER THE EXTINCTION, calculate and print the length of the longest chain from a
		//               producer to each organism
		findWebHeights(web, numOrgs);
		printf("\n");
		
		printf("UPDATED Vore Types:\n");
		//meizuo (Task 3): AFTER THE EXTINCTION, classify all organisms and print each group
		//               (producers, herbivores, omnivores, & carnivores)
		findVoreTypes(web, numOrgs);
		printf("\n");
		printf("--------------------------------\n");
	}
	
	//meizuo (Task 4): make sure to free all malloc'd memory to prevent potential leaks
	for (int i = 0; i < numOrgs; ++i) {
		free(web[i].prey);
	}
	free(web);
	return 0;
}
