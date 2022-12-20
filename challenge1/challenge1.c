#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define length 70
#define size 3

/*CREATING 3 STRUCTS
---Client Infomation (client): 			firstName, lastName, isEmployed, creditScore
						 		   			|         |          |           |
						                  string    string      bool        int

---Order Information (order):  			numCosigned, carID, pricePerPerson, minCreds, needEmployed, buyNew, verifiedCosigners
                                    		|         |          |            |           |          |             |
	                               		   int       int       float         int        char        bool          int   

---Used Car Information (usedCar):      carID,   brand,   make,  year, mileage, price
										  |        |       |      |      |       |
	                                     int    string   string  int    int    float
	
---newCar Information (newCar):			carID,   brand,   make,  waitTime, price, minCred, needEmployment
										  |        |       |       |        |       |             |
										 int    string   string   float   float    int           bool
*/

struct client {
	char firstName[length];
	char lastName[length];
	bool isEmployed;
	int creditScore;
};

struct order {
	int numCosigned;
	int carID;
	float pricePerPerson;
	int minCreds;
	char needEmployed;
	bool buyNew;
	int verifiedCosigners;
};

struct usedCar {
	int carID;
	char brand[length];
	char make[length];
	int year;
	int mileage;
	float price;
};

struct newCar {
	int carID;
	char brand[length];
	char make[length];
	float waitTime;
	float price;
	int minCred;
	bool needEmployment;
};

/*DECLARE GLOBAL VARIABLES
--- usedCars
	-type: array of struct usedCar
	-size: 3 

--- newCars
	-type: array of struct newCar
	-size: 3 
*/

struct usedCar usedCars[size] = {
								{1, "Nissan", "Altima", 2010, 11335, 12891.23},
								{2,"Jeep","Renegade", 2003, 10089, 15902.99},
								{3,"Toyota","Lexus", 2017, 18290, 17920.32}
								};

struct newCar newCars[size] = {
							  {1, "Toyota", "Hino", 4.5, 25000.99, 500, .needEmployment = 1},
							  {2, "Jeep", "Gladiator", 5, 43043.21, 600, .needEmployment = 1},
							  {3, "Hyundai", "Venue", 1.5, 60032.45, 700, .needEmployment = 1}
							  };

/*DECLARE FUNCTIONS

printCars
-use:        print information of used or new cars availibles on storage
-type:		 void
-arguments:  userChoiceCar
-return:	 none

userChoice
-use:        storage user choice of new or used cars
-type:		 bool
-arguments:  userChoiceCar
-return:	 bool

checkValidChar
-use:        check valid character for used and new cars
-type:		 bool
-arguments:  char
-return:	 bool 

checkValidID
-use:        check valid ID 
-type:		 bool
-arguments:  int
-return:     bool 

getClientInfo
-use:        get information of clients
-type:       struct client
-arguments:  none 
-return:     struct client 

newOrder
-use:        start a new order
-type:  	 void
-arguments:  
-return 	 none

printClient
-use:        print client information
-type:  	 void
-arguments:  int, struct client
-return 	 none

*/

void printCars(bool userChoiceCar);
bool userChoice(char input);
bool checkValidChar(char c);
bool checkValidID(int n);
struct client getClientInfo();
void newOrder();
void printClient(int idx, struct client cosigner);

int main()
{
	bool new = 1;
	char input;
	while(new)
	{
		newOrder();
		printf("\n\n");
		printf("\nThank you for chosing our Dealership. Would you like to put in another purcharse? Enter here(Y/N): ");
		scanf(" %c", &input);
		new = (input == 'Y' || input == 'y') ? true : false;
	}
	return 0;
}

void newOrder()
{
	// Declare variables
	struct order actualOrder;
	char input;
	float price;

	printf("\nWelcome to ECE131 Dealership.\nWhat kind of car are you looking? (U)sed or (N)ew: ");
	scanf(" %c", &input);

	// save user choice 
	actualOrder.buyNew = userChoice(input);

	printCars(actualOrder.buyNew);
	
	printf("\nWhich car do you want to choose? Input the car ID or 0 to quit: ");
	scanf(" %d", &actualOrder.carID);

	// check valid input
	bool isValidID = checkValidID(actualOrder.carID);

	// ask until the user inputs a valid ID or 0
	while(!isValidID)
	{
		printf("Invalid ID, enter a value number: ");
		scanf(" %d", &actualOrder.carID);
		isValidID = checkValidID(actualOrder.carID);
	} 

	// quit the program
	if (actualOrder.carID == 0)
	{
		printf("See you again hopefully. Bye now.");
		exit(0);
	}

	// get the bumber of cosigners
	printf("Thank you. Could you tell me how many people are buying this car? Enter here: ");
	scanf(" %d", &actualOrder.numCosigned);
	printf("Great. So you chose carID %d.", actualOrder.carID);
	
	// print car cost
	if (!actualOrder.buyNew)
	{
		printf("\nThe cost of the car will be %.2f. Would you be OK with that? Enter here (Y/N): ", usedCars[actualOrder.carID - 1].price);
		price = usedCars[actualOrder.carID - 1].price;
	}
	else
	{
		printf("\nThe cost of the car will be %.2f. Would you be OK with that? Enter here (Y/N): ", newCars[actualOrder.carID - 1].price);	
		price = newCars[actualOrder.carID - 1].price;
	}
	scanf(" %c", &input);

	// exit the program
	if (input == 'N' || input == 'n') 
	{
		printf("\nIt's sad to see you go. Bye.");
		exit(0);
	}

	printf("Great! We'll start getting the cosigners information then!\n\n\nYou have %d people buying this car. I'll process them one by one.\n\n", actualOrder.numCosigned);
	
	// create an array of cosigners
	struct client cosigners[actualOrder.numCosigned];

	actualOrder.verifiedCosigners = actualOrder.numCosigned;

	// get cosigners information
	int startPoint = 0;
	for (int i = 0; i < actualOrder.numCosigned; i++)
	{
		printf("\nProcessing Cosigner #%d. ", i+1);
		struct client cli;
		cli = getClientInfo();
		if(actualOrder.buyNew) printf("Since you're buying new cars. We require employment and minimum credit score of %d.", newCars[actualOrder.carID - 1].minCred);
		if (actualOrder.buyNew == 1 && (cli.creditScore < newCars[actualOrder.carID - 1].minCred || cli.isEmployed == 0))
		{
			printf("\nUnfortunately you don't fulfill them.\nDo you guys want to skip them and re-adjust the price with the rest of the cosigners? Enter here (Y/N): ");
			scanf(" %c", &input);
			if (input == 'N' || input == 'n') 
			{
				printf("Thank you, bye.");
				exit(0);
			}
			actualOrder.verifiedCosigners --;	

			
		}
		else
		{
			cosigners[startPoint] = cli;
			startPoint ++;
			printf("\nThank you! You're set :)");
		}

		printf("\n\n");
		
	}

	if(actualOrder.verifiedCosigners <= 0)
	{
		printf("\nNot enough verified cosigners\n");
		exit(0);
	}
	
	actualOrder.pricePerPerson = price/actualOrder.verifiedCosigners;

	if (actualOrder.verifiedCosigners < actualOrder.numCosigned)
	{
		printf("Since the number of cosigner have changed. The price has changed from %.2f per person to %.2f.\n", price/actualOrder.numCosigned, actualOrder.pricePerPerson);	
	}
	else{
		printf("\nEverything looks good. The price per person remains at %.2f", actualOrder.pricePerPerson);
	}

	printf("\nDo you want to confirm and finish the purcharse (Y/N): ");
	scanf(" %c", &input);

	if (input == 'n' || input == 'N') 
	{
		printf("Come back next time when you change your mind. Bye");
		exit(0);
	}

	printf("\n\nPROCESSING...\nAwesome! You have purcharsed the car. Here's the information about the co-signers. You have a wonderful time.\n\n");

	for (int i = 0; i < actualOrder.verifiedCosigners; i++)
	{
		printClient(i+1, cosigners[i]);
	}

}

void printCars(bool userChoiceCar){

	if (!userChoiceCar)
	{
		// print used car
		printf("\n\nHere are the used cars we have.\n-----------------------------------\n\n");
		for (int i = 0; i < size; ++i)
		{
			printf("Car ID: %d\n%s %s %d Mileage: %d\nCost: $%.2f\n", usedCars[i].carID, usedCars[i].brand, usedCars[i].make, usedCars[i].year, usedCars[i].mileage, usedCars[i].price);		
			printf("-----------------------------------\n\n");
		}	
	}
	else{
		// print new car
		printf("\n\nHere are the new cars we have.\n-----------------------------------\n");
		for (int i = 0; i < size; ++i)
		{
			printf("Car ID: %d %s %s \nWait time: %.2f\nMinimun credit score: %d\nNeed employment (1) true and (0) false: %d\nCost: $%.2f\n", newCars[i].carID, newCars[i].brand, newCars[i].make, newCars[i].waitTime, newCars[i].minCred, newCars[i].needEmployment, newCars[i].price);
			printf("-----------------------------------\n\n");
		}	
	}	
}

bool checkValidChar(char c){
	if (c == 'u' || c == 'U' || c == 'n' || c == 'N') return 1; 
	return 0;
}

bool checkValidID(int n){
	if (n <= size && n >= 0) return 1;
	return 0;
}

bool userChoice(char input)
{
	// check valid input for user choice (u)sed or (n)ew
	bool isValidChar = checkValidChar(input);
	// Ask again until user inputs a valid character
	while(!isValidChar)
	{
		printf("Error insert again U for used and N for new: ");
		scanf(" %c", &input);
		isValidChar = checkValidChar(input);
	}
	if (input == 'n' || input == 'N') return 1;  
	return 0;
}

struct client getClientInfo()
{
	struct client cli;
	char input;

	printf("Please enter First Name followed by Last Name: ");
	scanf("%s %s", cli.firstName, cli.lastName);
	printf("Thank you. Do you currently have a job (Y)es or (N)o: ");
	
	scanf(" %c", &input);
	
	cli.isEmployed = (input == 'Y' || input == 'y') ? 1 : 0;
	
	printf("Enter your credit score: ");
	scanf("%d", &cli.creditScore);

	return cli;

}

void printClient(int idx, struct client cosigner)
{
	printf("Cosigner # %d %s %s. Credit score: %d. Employed(1)yes (0)no: %d\n", idx, cosigner.firstName, cosigner.lastName, cosigner.creditScore, cosigner.isEmployed);
}
