#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef enum {false, true} bool;        // Allows boolean types in C

/* Defines a job struct */
struct Process {
    uint32_t A;                         // A: Arrival time of the process
    uint32_t B;                         // B: Upper Bound of CPU burst times of the given random integer list
    uint32_t C;                         // C: Total CPU time required
    uint32_t M;                         // M: Multiplier of CPU burst time
    uint32_t processID;                 // The process ID given upon input read

    uint8_t condition;                     // 0 is unstarted, 1 is ready, 2 is running, 3 is blocked, 4 is terminated

    int32_t finishingTime;              // The cycle when the the process finishes (initially -1)
    uint32_t CPUTime;         // The amount of time the process has already run (time in running state)
    uint32_t Btime;      // The amount of time the process has been IO blocked (time in blocked state)
    uint32_t currentWaitingTime;        // The amount of time spent waiting to be run (time in ready state)

    uint32_t BurstTime;                   // The amount of time until the process finishes being blocked
    uint32_t CPUBurst;                  // The CPU availability of the process (has to be > 1 to move to running)

    int32_t quantum;                    // Used for schedulers that utilise pre-emption

    bool operating;            // Used to check when to calculate the CPU burst when it hits running mode

    struct Process* pushBlock;  // A pointer to the next process available in the blocked list
    struct Process* freshPrime;   // A pointer to the next process available in the ready queue
    struct Process* pushPrimeForward; // A pointer to the next process available in the ready suspended queue
};

/* Global values */
// Flags to be set
bool sumDat = false;           // Flags whether the output should be detailed or not
bool ranomizeN = false;            // Flags whether the output should include the random digit or not
bool firstPrime = true;
struct Process* namelessOp = NULL;

uint32_t present = 0;             // The current cycle that each process is on
uint32_t countPrime = 0;   // The total number of processes constructed
uint32_t totalPrime = 0;   // The total number of processes that have started being simulated
uint32_t completion = 0;  // The total number of processes that have finished running
uint32_t totalTimeNumber = 0;

const char* pickAr= "random-numbers";
const uint32_t SEED_VALUE = 200;  // Seed value for reading from file
/* Queue & List pointers */
// readyQueue head & tail pointers

//******************* Additional variables as needed******************//

struct Process* selection = NULL;
struct Process* Afront = NULL;
struct Process* Aback = NULL;
struct Process* Pausefront = NULL;
struct Process* Pauseback = NULL;
struct Process* Blockfront = NULL;
struct Process* Blockback = NULL;
uint32_t placement = 0;
uint32_t PauseQ = 0;
uint32_t BlockQ = 0;
/**
 * Reads a random non-negative integer X from a file named random-numbers (in the current directory)
 */
uint32_t ranomize(uint32_t line, FILE* infinity)
{
    char str[512];
    uint32_t MysteryNumb = (uint32_t) atoi(fgets(str, 512, infinity));
    uint32_t returnValue = 1 + (MysteryNumb % line);
    return returnValue;
} // End of the ranomize function

//  **********************************insert Q(enqueue)**********************************//

void Qsetup(struct Process* FreshN)
{
    if (placement == 0)
    {
        Afront = FreshN;
        Aback = FreshN;
    }
    else
    {
        FreshN->freshPrime = NULL;

        Aback->freshPrime = FreshN;
        Aback = Aback->freshPrime; 
    }
    ++placement;
} 

//  ********************************** Dequeue**********************************//

struct Process* dsetup()
{
    if (placement == 0)
    {
        printf("ERROR\n");
        return NULL;
    }
    else
    {
        struct Process* rot = Afront;
        Afront = Afront->freshPrime;
        --placement;
        if (placement == 0)
            Aback = NULL;
        rot->freshPrime = NULL;
        return rot;
    }
} 
//  ********************************** pause Q **********************************//

void pauseQue(struct Process* FreshN)
{
    if (PauseQ == 0)
    {
        Pausefront = FreshN;
        Pauseback = FreshN;
        Afront = Aback;
    }
    else
    {
        FreshN->pushPrimeForward = NULL;
        Pauseback->pushPrimeForward = FreshN;
        Pauseback = Pauseback->pushPrimeForward;
    }
    ++PauseQ;
} 


//  ********************************** replaceNode **********************************//
struct Process* replaceN()
{
    if (PauseQ == 0)
    {
        return NULL;
    }
    else
    {
        struct Process* rot = Pausefront;
        Pausefront = Pausefront->pushPrimeForward;
        --PauseQ;
        if (PauseQ == 0)
            Pauseback = NULL;
        rot->pushPrimeForward = NULL;
        return rot;
    }
}

//  ********************************** insert block **********************************//
void insertB(struct Process* FreshN)
{
    if ((Blockfront == NULL) || (Blockback == NULL))
    {
        Blockfront = FreshN;
        Blockback = FreshN;
    }
    else
    {
        Blockback->pushBlock = FreshN;
        Blockback = FreshN;
    }
    ++BlockQ;
}

//  ********************************** replaceBlockNode **********************************//

struct Process* removeB()
{
    if (Blockfront == NULL)
    {
        return NULL;
    }
    else
    {
        struct Process* rot = Blockfront;
        Blockfront = Blockfront->pushBlock;
        if (Blockfront == NULL)
            Blockback = NULL;
        --BlockQ;
        rot->pushBlock = NULL;
        return rot;
    }
} 

//  ********************************** executeBlock **********************************//

void execxuteB()
{
    if (BlockQ != 0)
    {
        struct Process* presentN = Blockfront;
        while (presentN != NULL)
        {
            if ( (int32_t) presentN->BurstTime <= 0)
            {
                presentN->condition = 1;
                if (presentN->processID == Blockfront->processID)
                {
                    struct Process* removeBlockOp = removeB();
                    Qsetup(removeBlockOp);
                }
                else if (presentN->processID == Blockback->processID)
                {
                    struct Process* blockPrime = Blockfront;
                    while (blockPrime->pushBlock->pushBlock != NULL)
                    {blockPrime = blockPrime->pushBlock;}
                    Blockback = blockPrime;
                    struct Process* extraN = blockPrime->pushBlock;
                    blockPrime->pushBlock = NULL;
                    extraN->pushBlock = NULL;
                    --BlockQ;
                    Qsetup(extraN);
                }
                else
                {
                    struct Process* blockPrime = Blockfront;
                    while (blockPrime->pushBlock->processID != presentN->processID)
                    {blockPrime = blockPrime->pushBlock;}
                    struct Process* extraN = blockPrime->pushBlock;
                    blockPrime->pushBlock =
                            blockPrime->pushBlock->pushBlock;
                    extraN->pushBlock = NULL;
                    --BlockQ;
                    Qsetup(extraN);
                }
            } 
            presentN = presentN->pushBlock;
        } 
    } 
} 

/**
 * @param terminateAlg 
 * @param selectAlg 
 */
//************************************* current job ****************************//
void proceedForward(struct Process terminateAlg[], uint8_t selectAlg)
{
    if (selection != NULL)
    {
        if (selection->operating == true)
        {
            selection->operating = false;
            selection->BurstTime = 1 + (selection->M * selection->CPUBurst);
        }
        if (selection->C == selection->CPUTime)
        {
            selection->condition = 4;
            selection->finishingTime = present;
            terminateAlg[completion] = *selection;
            ++completion;
            if (selectAlg == 2)
                namelessOp = NULL;
            selection = NULL;
        }
        else if (selection->CPUBurst <= 0)
        {
            selection->condition = 3;
            insertB(selection);
            selection = NULL;
        } 
        else if ((selectAlg == 1) && (selection->quantum <= 0))
        {
            selection->condition = 1;
            Qsetup(selection);
            selection = NULL;
        } 
        else
        {

            selection->condition = 2;
        }
    } 
} 

/**
 * @param Prime 
 */
//*********************************************** input process **************************//
void creation(struct Process Prime[])
{
    uint32_t i = 0;
    for (; i < countPrime; ++i)
    {
        if (Prime[i].A == present)
        {
            ++totalPrime;
            Prime[i].condition = 1; 
            if ((namelessOp == NULL) && (firstPrime == true))
            {
                firstPrime = false;
                namelessOp = &Prime[i];
            }
            Qsetup(&Prime[i]);
        }
    }
} 
/**
 * @param selectAlg 
 * @param pushthroughCurrent 
 * @param randomFile 
 */
//*********************************prepare job while in queue ************************************//

void executeR(uint8_t selectAlg, uint8_t pushthroughCurrent, FILE* randomFile)
{

    if ((namelessOp != NULL) && (placement != 0)
        && (Afront != namelessOp) && (selectAlg == 2))
    {
        uint32_t i = 0;
        for (; i < placement; ++i)
        {
            struct Process* pauseNo = dsetup();
            pauseNo->condition = 1;
            pauseQue(pauseNo);
        }
    }
    if ((PauseQ != 0) && (selectAlg == 2))
    {
        if (namelessOp == NULL) {
            struct Process *continueRunning = replaceN();
            continueRunning->condition = 1;
            namelessOp = continueRunning;
            Qsetup(continueRunning);
        }
    }

    //************************* handles next job ready in queue**********//
    if (placement != 0)
    {
        if (selection == NULL)
        {
            if (selectAlg == 3)
            {
                uint32_t i = 0;
                struct Process* presentPrime = Afront;
                struct Process* quickPrime = Afront;
                for (; i < placement; ++i)
                {
                    if ((quickPrime->C - quickPrime->CPUTime) >
                            (presentPrime->C - presentPrime->CPUTime))
                    {
                        quickPrime = presentPrime;
                    }
                    presentPrime = presentPrime->freshPrime;
                }
                struct Process* loadedPrime;

                if (Afront->processID == quickPrime->processID)
                {
                    loadedPrime = dsetup();
                }
                else if (Aback->processID == quickPrime->processID)
                {
                    struct Process* presentNode = Afront;
                    while (presentNode->freshPrime->freshPrime != NULL)
                    {presentNode = presentNode->freshPrime;}
                    Aback = presentNode;
                    loadedPrime = presentNode->freshPrime;
                    presentNode->freshPrime = NULL;
                    loadedPrime->freshPrime = NULL;
                    --placement;
                }
                else  //************************begin to dequeue list****************//
                {
                    struct Process* presentNode = Afront;
                    while (presentNode->freshPrime->processID != quickPrime->processID)
                    {presentNode = presentNode->freshPrime;}
                    loadedPrime = presentNode->freshPrime;
                    presentNode->freshPrime =
                            presentNode->freshPrime->freshPrime;
                    loadedPrime->freshPrime = NULL;
                    --placement;
                }
                loadedPrime->condition = 2;
                loadedPrime->operating = true;
                char str[20];
                uint32_t MysteryNumb = (uint32_t) atoi(fgets(str, 20, randomFile));
                if ((ranomizeN) && (pushthroughCurrent % 2 == 0))
                    printf("select %i\n", MysteryNumb);
                uint32_t newCPUBurst = 1 + (MysteryNumb % quickPrime->B);
                if (newCPUBurst > (loadedPrime->C - loadedPrime->CPUTime))
                    newCPUBurst = loadedPrime->C - loadedPrime->CPUTime;
                loadedPrime->CPUBurst = newCPUBurst;
                selection = loadedPrime;
            } 
            else  //*************************begin to process the CPU burst******//
            {    
                struct Process* preparePrime = dsetup();
                char str[20];
                uint32_t MysteryNumb = (uint32_t) atoi(fgets(str, 20, randomFile));
                if ((ranomizeN) && (pushthroughCurrent % 2 == 0))
                    printf("select %i\n", MysteryNumb);

                uint32_t newCPUBurst = 1 + (MysteryNumb % preparePrime->B);
                if (newCPUBurst > (preparePrime->C - preparePrime->CPUTime))
                    newCPUBurst = preparePrime->C - preparePrime->CPUTime;
                preparePrime->CPUBurst = newCPUBurst;
                if (preparePrime->CPUBurst > 0)
                {
                    preparePrime->condition = 2;
                    preparePrime->operating = true;

                    if (selectAlg == 1)
                    {
                        preparePrime->quantum = 2;
                    }
                    selection = preparePrime;
                }
            } 
        } 
    }
    //**************************************Ready Process*********************************//
    if ((selectAlg == 2) && (placement != 0))
    {
        uint32_t i = 0;
        for (; i < placement; ++i)
        {
            if (selection != NULL)
            {
                struct Process* pauseNo = dsetup();
                pauseNo->condition = 1;
                pauseQue(pauseNo);
            }
        }
    } 
} 

/**
 * @param Prime 
 * @param selectAlg 
 */
//*********************************** functions that handles the timers **************//
void countdown(struct Process Prime[], uint8_t selectAlg)
{
    uint32_t i = 0;
    for (; i < countPrime; ++i)
    {
        switch (Prime[i].condition)
        {
            case 0:
                break;
            case 3:
                ++Prime[i].Btime;
                --Prime[i].BurstTime;
                break;
            case 2:
                ++Prime[i].CPUTime;
                --Prime[i].CPUBurst;
                if (selectAlg == 1)
                {
                    --Prime[i].quantum;
                }
                break;
            case 1:
                ++Prime[i].currentWaitingTime;
                break;
            case 4:
                break;
            default:
                fprintf(stderr, "Error\n");
                exit(1);
        } 
    }

//*************************************calcualote total time************//
    for (i = 0; i < countPrime; ++i)
    {
        if (Prime[i].condition == 3)
        {
            ++totalTimeNumber;
            break;
        }
    }
}
//************************************* variables for main function**********//
/**
 * @param Prime
 */
void Extra(struct Process Prime[])
{
    present = 0;
    totalPrime = 0;
    completion = 0;
    totalTimeNumber = 0;
    firstPrime = true;
    namelessOp = NULL;
    Afront = NULL;
    Aback = NULL;
    placement = 0;
    Pausefront = NULL;
    Pauseback = NULL;
    PauseQ = 0;
    Blockfront = NULL;
    Blockback = NULL;
    BlockQ = 0;
    selection = NULL;
    uint32_t i = 0;
    FILE* infinity = fopen(pickAr, "r");
    for (; i < countPrime; ++i)
    {
        Prime[i].condition = 0;
        Prime[i].freshPrime = NULL;
        Prime[i].pushPrimeForward = NULL;
        Prime[i].pushBlock = NULL;
        Prime[i].finishingTime = -1;
        Prime[i].CPUTime = 0;
        Prime[i].Btime = 0;
        Prime[i].currentWaitingTime = 0;
        Prime[i].operating = false;
        Prime[i].CPUBurst = ranomize(Prime[i].B, infinity);
        Prime[i].BurstTime = Prime[i].M * Prime[i].CPUBurst;
    }
    fclose(infinity);
} 

/**
 * @param argc 
 * @param argv 
 */
//*******************************************sets number of arguments*********************//
uint8_t arguments(int32_t argc, char *argv[])
{
    if (argc == 2)
        return 1;
    else
    {
        if ((strcmp(argv[1], "yes") == 0) || (strcmp(argv[2], "yes") == 0))
            sumDat = true;
        if ((strcmp(argv[1], "no") == 0) || (strcmp(argv[2], "no") == 0))
            ranomizeN = true;
        if ((strcmp(argv[1], "no") != 0) && (strcmp(argv[1], "yes") != 0))
            return 1;
        else if ((strcmp(argv[2], "no") != 0) && (strcmp(argv[2], "yes") != 0))
            return 2;
        else
            return 3;
    }
} 
//**************************from here on we set our output functions*******************//
/**
 * Prints to standard output the original input
 * @param Prime The original processes inputted, in array form
 */
void printStart(struct Process Prime[])
{
    printf("insert %i", countPrime);

    uint32_t i = 0;
    for (; i < countPrime; ++i)
    {
        printf(" ( %i %i %i %i)", Prime[i].A, Prime[i].B,
               Prime[i].C, Prime[i].M);
    }
    printf("\n");
} 

/**
 
 * @param terminateAlg 
 */
void printFinal(struct Process terminateAlg[])
{
    printf("The (sorted) input is: %i", countPrime);

    uint32_t i = 0;
    for (; i < completion; ++i)
    {
        printf(" ( %i %i %i %i)", terminateAlg[i].A, terminateAlg[i].B,
               terminateAlg[i].C, terminateAlg[i].M);
    }
    printf("\n");
} // End of the print final function

/**
 * Prints out specifics for each process.
 * @param Prime The original processes inputted, in array form
 */
void printProcessSpecifics(struct Process Prime[])
{
    uint32_t i = 0;
    printf("\n");
    for (; i < countPrime; ++i)
    {
        printf("Process %i:\n", Prime[i].processID);
        printf("\t(A,B,C,M) = (%i,%i,%i,%i)\n", Prime[i].A, Prime[i].B,
               Prime[i].C, Prime[i].M);
        printf("\tFinishing time: %i\n", Prime[i].finishingTime);
        printf("\tTurnaround time: %i\n", Prime[i].finishingTime - Prime[i].A);
        printf("\tI/O time: %i\n", Prime[i].Btime);
        printf("\tWaiting time: %i\n", Prime[i].currentWaitingTime);
        printf("\n");
    }
} // End of the print process specifics function

/**
 * Prints out the summary data
 * @param Prime The original processes inputted, in array form
 */
void printSummaryData(struct Process Prime[])
{
    uint32_t i = 0;
    double totalAmountOfTimeUtilisingCPU = 0.0;
    double totalAmountOfTimeIOBlocked = 0.0;
    double totalAmountOfTimeSpentWaiting = 0.0;
    double totalTurnaroundTime = 0.0;
    uint32_t finalFinishingTime = present - 1;
    for (; i < countPrime; ++i)
    {
        totalAmountOfTimeUtilisingCPU += Prime[i].CPUTime;
        totalAmountOfTimeIOBlocked += Prime[i].Btime;
        totalAmountOfTimeSpentWaiting += Prime[i].currentWaitingTime;
        totalTurnaroundTime += (Prime[i].finishingTime - Prime[i].A);
    }

    // Calculates the CPU utilisation
    double CPUUtilisation = totalAmountOfTimeUtilisingCPU / finalFinishingTime;

    // Calculates the IO utilisation
    double IOUtilisation = (double) totalTimeNumber / finalFinishingTime;

    // Calculates the throughput (Number of processes over the final finishing time times 100)
    double throughput =  100 * ((double) countPrime/ finalFinishingTime);

    // Calculates the average turnaround time
    double averageTurnaroundTime = totalTurnaroundTime / countPrime;

    // Calculates the average waiting time
    double averageWaitingTime = totalAmountOfTimeSpentWaiting / countPrime;

    printf("Summary Data:\n");
    printf("\tFinishing time: %i\n", present - 1);
    printf("\tCPU Utilisation: %6f\n", CPUUtilisation);
    printf("\tI/O Utilisation: %6f\n", IOUtilisation);
    printf("\tThroughput: %6f processes per hundred cycles\n", throughput);
    printf("\tAverage turnaround time: %6f\n", averageTurnaroundTime);
    printf("\tAverage waiting time: %6f\n", averageWaitingTime);
} // End of the print summary data function



/**
 * @param pushthroughCurrent 
 * @param Prime 
 * @param terminateAlg 
 * @param randomFile 
 * @param datastream 
 */
void datapoint(uint8_t pushthroughCurrent, struct Process Prime[],
                       struct Process terminateAlg[], FILE* randomFile, uint8_t datastream)
{
    if ((sumDat) && ((pushthroughCurrent) % 2 == 0))
    {
        printf("previous run\t%i:\t", present);
        int i = 0;
        for (; i < countPrime; ++i)
        {
            switch (Prime[i].condition)
            {
                case 0:
                    printf("unstarted \t0\t");
                    break;
                case 1:
                    printf("ready   \t0\t");
                    break;
                case 2:
                    printf("running \t%i\t", Prime[i].CPUBurst + 1);
                    break;
                case 3:
                    printf("blocked \t%i\t", Prime[i].BurstTime + 1);
                    break;
                case 4:
                    printf("terminated \t0\t");
                    break;
                default:
                    fprintf(stderr, "Error\n");
                    exit(1);
            } 
        } 
        printf("\n");
    }
    proceedForward(terminateAlg, datastream);
    execxuteB();
    if (totalPrime != countPrime)
    {
        creation(Prime);
    }
    executeR(datastream, pushthroughCurrent, randomFile);
    countdown(Prime, datastream);
    ++present;
} 
//********************Print FCFS, RR, BLOCK, SJF***************************//
void listoutputs (struct Process Prime[], uint8_t datastream)
{
    switch (datastream)
    {
       case 0:
            printf("---- FCFS ----\n");
            break;
        case 1:
            printf("---- RR ----\n");
            break;
        case 2:
            printf("---- block ----\n");
            break;
        case 3:
            printf("---- SJF ----\n");
            break;
        default:
            printf("Error\n");
            datastream = 0;
            break;
    }

    printStart(Prime);
    uint8_t pushthroughCurrent = 1;
    struct Process terminateAlg[countPrime];
    FILE* infinity = fopen(pickAr, "r");
    while (completion != countPrime)
        datapoint(pushthroughCurrent, Prime, terminateAlg, infinity, datastream);
    fclose(infinity);
    printFinal(terminateAlg);
    Extra(Prime);
    printf("\n");
    ++pushthroughCurrent;
    if (sumDat)
        printf("burst for processes\n");
    infinity = fopen(pickAr, "r");
    while (completion != countPrime)
        datapoint(pushthroughCurrent, Prime, terminateAlg, infinity, datastream);
    fclose(infinity);
    switch (datastream)
    {
        case 0:
            printf("************First Come First Serve************\n");
            break;
        case 1:
            printf("************Round Robin************\n");
            break;
        case 2:
            printf("************ Block ************\n");
            break;
        case 3:
            printf("************Shortest Job First************\n");
            break;
        default:
            break;
    }
    printProcessSpecifics(Prime);
    printSummaryData(Prime);
    Extra(Prime);        
    switch (datastream)
    {
        case 0:
            printf("---- FCFS ----\n");
            break;
        case 1:
            printf("---- RR ----\n");
            break;
        case 2:
            printf("----Block ----\n");
            break;
        case 3:
            printf("---- SJF ----\n");
            break;
        default:
            break;
    }
} 

int main(int argc, char *argv[])
{
    FILE* inputFile;
    FILE* infinity;
    char* filePath;
    filePath = argv[arguments(argc, argv)]; 
    inputFile = fopen(filePath, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Error %s!\n",filePath);
        exit(1);
    }
    uint32_t allData;                   
    fscanf(inputFile, "%i", &allData);   
    infinity = fopen(pickAr, "r");
    struct Process Prime[allData]; 

    // ****************scans input and saves into process***************************//
    uint32_t end = 0;
    for (; end < allData; ++end)
    {
        uint32_t A1;
        uint32_t A2;
        uint32_t A3;
        uint32_t A4;
        fscanf(inputFile, " %*c%i %i %i %i%*c", &A1, &A2, &A3, &A4);
        Prime[end].A = A1;
        Prime[end].B = A2;
        Prime[end].C = A3;
        Prime[end].M = A4;
        Prime[end].processID = end;
        Prime[end].condition = 0;
        Prime[end].finishingTime = -1;
        Prime[end].CPUTime = 0;
        Prime[end].Btime = 0;
        Prime[end].currentWaitingTime = 0;
        Prime[end].CPUBurst = ranomize(Prime[end].B, infinity);
        Prime[end].BurstTime = Prime[end].M * Prime[end].CPUBurst;
        Prime[end].operating = false;
        Prime[end].quantum = 2;
        Prime[end].pushBlock = NULL;
        Prime[end].freshPrime = NULL;
        Prime[end].pushPrimeForward = NULL;
        ++countPrime;
    }
    fclose(inputFile);
    fclose(infinity);
    listoutputs(Prime, 0);
    listoutputs(Prime, 1);
    listoutputs(Prime, 2);
    listoutputs(Prime, 3);
    return EXIT_SUCCESS;
} 