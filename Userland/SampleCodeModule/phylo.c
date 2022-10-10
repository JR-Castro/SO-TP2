// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <phylo.h>

static void philosopher(int argc, char ** argv);
static void eat(int id);
static void leave(int id);
static void addPhil();
static void removePhil();
static void createPhilProcess();
static void phyloStatus()

int philoCount = 0;
static char forks[MAX_PHYL][3];
static char status[MAX_PHYL];
static unsigned long long philoPids[MAX_PHYL] = {0};

void phylo_main() {

    //se inician los tenedores
    for (int i = 0; i < MAX_PHYL; i++){
        char semName[3];
        itoa(i, semName);

        forks[i][0] = semName[0];
        forks[i][1] = semName[1];
        forks[i][2] = semName[2];

        // Abrimos los semaforos 
        if(sys_sem_open(forks[i], 1) == 0){
            print("Error creando tenedores\n");

            for (int j = 0; j < i; j++)
                sys_sem_close(forks[j]);  
            return;
        }
    }

    // Creamos el semaforo del comedor (room). El mismo admite hasta philCount-1
    if(sys_sem_open(ROOM_SEM_NAME, INITIAL_PHYL-1) == 0){
        for (int j = 0; j < MAX_PHYL; j++)
                sys_sem_close(forks[j]);  
        return;
    };
    
    print("\n");
    for (int i = 0; i < MAX_PHYL; i++){
        status[i] = WAITING_CHAR;
    }
    printStatus();

    for (int i = 0; i < INITIAL_PHYL; i++){
        createPhilProcess();
    }

    /* Mientras algun filosofo siga comiendo, se podran agregar o quitar filosofos
     * en tiempo de ejecucion */
    for (int i = 0; i < MAX_PHYL; i++){
        char c;
        while (sys_exists(philoPids[i])){
            c = getChar();
            if(c == ADD_PHILO_KEY)
                addPhil();
            else if (c == REMOVE_PHILO_KEY)
                removePhil();
        }
    }
    
    for (int i = 0; i < MAX_PHYL; i++)
        sys_sem_close(forks[i]);
    sys_sem_close(ROOM_SEM_NAME);

    philoCount = 0;
}

void philosopher(int argc, char ** argv){
    
    int id = atoi(argv[1]);
    int lFork = id;
    int rFork = (id+1) % philoCount;

    sys_sem_wait(ROOM_SEM_NAME);
    sys_sem_wait(forks[lFork]);
    sys_sem_wait(forks[rFork]);

    eat(id);
    leave(id);

    sys_sem_post(forks[lFork]);
    sys_sem_post(forks[rFork]);
    sys_sem_post(ROOM_SEM_NAME);
}

static void eat(int id){
    status[id] = EATING_CHAR;
    printStatus();
    sys_yield();
}

static void leave(int id){
    status[id] = LEAVE_CHAR;
    printStatus();
}

static void addPhil(){
    if(philoCount == MAX_PHYL)
        return;

    createPhilProcess();
    sys_sem_post(ROOM_SEM_NAME);
}

static void removePhil(){
    if(philoCount == INITIAL_PHYL)
        return;

    sys_killPs(philoPids[philoCount]);
    philoPids[philoCount] = 0;
    philoCount--;
}

static void createPhilProcess(){
    char id[] = {philoCount+'0', 0};
    char * philoArgv[] = {"philosopher", id};

    int processMode = sys_getProcessMode();

    philoPids[philoCount] = sys_createProcess(&philosopher, 2, philoArgv, NULL, processMode);

    if(philoPids[philoCount] == 0){
        //error al crear al filosofo
        return;
    }

    philoCount++;
}

void phyloStatus() {
    for (int i = 0; i < philoCount; i++) {
        switch (status[i]){
            case EATING_CHAR:
                printf(EATING_CHAR);
                break;
            case LEAVE_CHAR:
                printf(LEAVE_CHAR);
                break;
            default:
                printf(WAITING_CHAR);
                break;
                
            }
            putChar(' ');
    }
    putChar('\n');
}