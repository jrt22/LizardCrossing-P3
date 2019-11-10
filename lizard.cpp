/***************************************************************/
/*                                                             */
/* lizard.cpp                                                  */
/*                                                             */
/* To compile, you need all the files listed below             */
/*   lizard.cpp                                                */
/*                                                             */
/* Be sure to use the -lpthread option for the compile command */
/*   g++ -g -Wall -std=c++11 lizard.cpp -o lizard -lpthread    */
/*                                                             */
/* Execute with the -d command-line option to enable debugging */
/* output.  For example,                                       */
/*   ./lizard -d                                               */
/*                                                             */
/***************************************************************/

#include <iostream>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <semaphore.h> // AP JT
#include <time.h> // AP JT
#include <signal.h> // AP JT


using namespace std;

/*
 * This is a stub file.  It contains very little code and what
 * it does contain may need to be altered or removed.  It is
 * only provided for a starting point.
 *
 * The comments are probably useful.
 */




/*
 * Define "constant" values here
 */

/*
 * Make this 1 to check for lizards traveling in both directions
 * Leave it 0 to allow bidirectional travel
 */
#define UNIDIRECTIONAL       0

/*
 * Set this to the number of seconds you want the lizard world to
 * be simulated.
 * Try 30 for development and 120 for more thorough testing.
 */
#define WORLDEND             30

/*
 * Number of lizard threads to create
 */
#define NUM_LIZARDS          20

/*
 * Number of cat threads to create
 */
#define NUM_CATS             2

/*
 * Maximum lizards crossing at once before alerting cats
 */
#define MAX_LIZARD_CROSSING  4

/*
 * Maximum seconds for a lizard to sleep
 */
#define MAX_LIZARD_SLEEP     3

/*
 * Maximum seconds for a cat to sleep
 */
#define MAX_CAT_SLEEP        3

/*
 * Maximum seconds for a lizard to eat
 */
#define MAX_LIZARD_EAT       5

/*
 * Number of seconds it takes to cross the driveway
 */
#define CROSS_SECONDS        2


/*
 * Declare global variables here
 */

/*
mutex lock is for cats and keeping track of crossing lizards
semaphore is for lizards and printing debug statements in order
*/
pthread_mutex_t lock; // AP JT
sem_t lizLock, debugLock; // AP JT


/**************************************************/
/* Please leave these variables alone.  They are  */
/* used to check the proper functioning of your   */
/* program.  They should only be used in the code */
/* I have provided.                               */
/**************************************************/
int numCrossingSago2MonkeyGrass;
int numCrossingMonkeyGrass2Sago;
int debug;
int running;
/**************************************************/

/**
 * This class models a cat that sleep, wakes-up, checks on lizards in the driveway
 * and goes back to sleep. If the cat sees enough lizards it "plays" with them.
 */
class Cat {

	int       _id;     // the Id of the cat
	pthread_t _thread; // the thread simulating the cat

	public:
		Cat (int id);
		int getId();
		void run();
		void wait();

	private:
		static void* runThread(void *param);
		void sleepNow();
};



/**
 * Constructs a cat.
 *
 * @param id - the Id of the cat
 */
Cat::Cat (int id)
{
	_id = id;
}

/**
 * Returns the Id of the cat.
 *
 * @return the Id of a cat
*/
int Cat::getId()
{
    return _id;
}

/**
 * Launches a cat thread.
 *
 * Status: Test AP JT
 */
 void Cat::run()
 {
	 // launch the thread to simulate the cat's behavior
	 pthread_create(&_thread, NULL, runThread, (void *)this); // AP JT

 }

 /**
  * Waits for a cat to finish running.
  *
  * Status: Test AP JT
  */
 void Cat::wait()
 {
	 // wait for the thread to terminate
	 pthread_join(_thread, NULL); // AP JT
 }

 /**
  *
  * This simulates a cat that is sleeping and occasionally checking on
  * the driveway on lizards.
  *
  * @param - N/A
  *
  * @return - N/A
  *
  * Status: Test AP JT
  */
void * Cat::runThread( void * param )
{
    Cat* myCat = (Cat*) param;

	if (debug)
    {
		sem_wait(&debugLock); // AP JT
		cout << "[" << myCat->getId() << "] cat is alive\n";
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }

	while(running)
    {
		myCat->sleepNow();

        //freezes cats and lizards to check lizard crossing
        pthread_mutex_lock(&lock); // AP JT
		/*
	     * Check for too many lizards crossing
	     */
		if (numCrossingSago2MonkeyGrass + numCrossingMonkeyGrass2Sago > MAX_LIZARD_CROSSING)
		{
		  cout << "\tThe cats are happy - they have toys.\n";
		  exit( -1 );
		}
		pthread_mutex_unlock(&lock); // AP JT
    }

	pthread_exit(NULL);
}

/**
 * Simulate a cat sleeping for a random amount of time
 *
 *
 * @return N/A
 *
 * Status: Completed - No need to change any of this code.
 */
void Cat::sleepNow()
{
	int sleepSeconds;

	sleepSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_CAT_SLEEP);

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] cat sleeping for " << sleepSeconds << " seconds" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }

	sleep( sleepSeconds );

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] cat awake" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }
}




class Lizard {
	int       _id;     // the Id of the lizard
	pthread_t _thread; // the thread simulating the lizard

	public:
		Lizard(int id);
		int getId();
		void run();
		void wait();

	private:
		static void* runThread(void *param);

		void sleepNow();
		void sago2MonkeyGrassIsSafe();
		void crossSago2MonkeyGrass();
		void madeIt2MonkeyGrass();
		void eat();
		void monkeyGrass2SagoIsSafe();
		void crossMonkeyGrass2Sago();
		void madeIt2Sago();

};



/**
 * Constructs a lizard.
 *
 * @param id - the Id of the lizard
 */
Lizard::Lizard (int id)
{
	_id = id;
}

/**
 * Returns the Id of the lizard.
 *
 * @return the Id of a lizard
*/
int Lizard::getId()
{
    return _id;
}

/**
 * Launches a lizard thread.
 *
 * Status: Test AP JT
 */
 void Lizard::run()
 {
	 // launch the thread to simulate the lizard's behavior
	 pthread_create(&_thread, NULL, runThread, (void *)this); // AP JT
 }

 /**
  * Waits for a lizard to finish running.
  *
  * Status: Test AP JT
  */
 void Lizard::wait()
 {
	 // wait for the thread to terminate
	 pthread_join(_thread, NULL); // AP JT
 }

 /**
  * Follows the algorithm provided in the assignment
  * description to simulate lizards crossing back and forth
  * between a sago palm and some monkey grass.
  *
  * @param param - N/A
  *
  * @return N/A

  * Status: Incomplete - Make changes as you see are necessary.
  */
void * Lizard::runThread( void * param )
{
    Lizard* myLizard = (Lizard *) param;

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
        cout << "[" << myLizard->getId() << "] lizard is alive" << endl;
        cout << std::flush;
        sem_post(&debugLock); // AP JT
    }

	while(running)
    {
      /*
       * Follow the algorithm given in the assignment
       * using calls to the functions declared above.
       * You'll need to complete the implementation of
       * some functions by filling in the code.  Some
       * are already completed - see the comments.
       */

        //lizard needs to sleep, move, eat, move, repeat

        myLizard->sleepNow(); // AP JT

        myLizard->crossSago2MonkeyGrass(); // AP JT

        myLizard->eat(); // AP JT

        myLizard->crossMonkeyGrass2Sago(); // AP JT
    }

	pthread_exit(NULL);
}




/**
 * Simulate a lizard sleeping for a random amount of time
 *
 * Status: Completed - No need to change any of this code.
 */
void Lizard::sleepNow()
{
	int sleepSeconds;

	sleepSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_LIZARD_SLEEP);

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
        cout << "[" << _id << "] sleeping for " << sleepSeconds << " seconds" << endl;
        cout << std::flush;
        sem_post(&debugLock); // AP JT
    }

	sleep( sleepSeconds );

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
        cout << "[" << _id << "] awake" << endl;
        cout << std::flush;
        sem_post(&debugLock); // AP JT
    }
}




/**
 *
 * Returns when it is safe for this lizard to cross from the sago
 * to the monkey grass.   Should use some synchronization
 * facilities (lock/semaphore) here.
 *
 * Status: Testing AP JT
 */
void Lizard::sago2MonkeyGrassIsSafe()
{
	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] checking  sago -> monkey grass" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }

    sem_wait(&lizLock); // AP JT
    pthread_mutex_lock(&lock); // AP JT
    numCrossingSago2MonkeyGrass++; // AP JT
    pthread_mutex_unlock(&lock); // AP JT



	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] thinks  sago -> monkey grass  is safe" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }

}


/**
 *
 * Delays for 1 second to simulate crossing from the sago to
 * the monkey grass.
 *
 * Status: Test AP JT
 */
void Lizard::crossSago2MonkeyGrass()
{
    //check to see if it is safe then starts
    sago2MonkeyGrassIsSafe(); //AP JT


	if (debug)
    {
        sem_wait(&debugLock); // AP JT
        cout << "[" << _id << "] crossing  sago -> monkey grass" << endl;
        cout << std::flush;
        sem_post(&debugLock); // AP JT
    }

	/*
	 * One more crossing this way
	 */


	/*
     * Check for lizards cross both ways
     */
    pthread_mutex_lock(&lock); // AP JT
	if (numCrossingMonkeyGrass2Sago && UNIDIRECTIONAL)
    {
		cout << "\tCrash!  We have a pile-up on the concrete." << endl;
		cout << "\t" << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t" << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }
    pthread_mutex_unlock(&lock); // AP JT

	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

    /*
     * That one seems to have made it
     */
    madeIt2MonkeyGrass(); // AP JT
}


/**
 * Tells others they can go now
 *
 * Status: Testing AP JT
 */
void Lizard::madeIt2MonkeyGrass()
{
	/*
     * Whew, made it across
     */
    pthread_mutex_lock(&lock); // AP JT
    numCrossingSago2MonkeyGrass--;
    sem_post(&lizLock); // AP JT
    pthread_mutex_unlock(&lock); // AP JT

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] made the  sago -> monkey grass  crossing" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }
}


/**
 * Simulate a lizard eating for a random amount of time
 *
 * Status: Completed - No need to change any of this code.
 */
void Lizard::eat()
{
	int eatSeconds;

	eatSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_LIZARD_EAT);

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] eating for " << eatSeconds << " seconds" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }

	/*
     * Simulate eating by blocking for a few seconds
     */
	sleep( eatSeconds );

	if (debug)
    {
        sem_wait(&debugLock); // AP JT
        cout << "[" << _id << "] finished eating" << endl;
        cout << std::flush;
        sem_post(&debugLock); // AP JT
    }
}


/**
 * Returns when it is safe for this lizard to cross from the monkey
 * grass to the sago.   Should use some synchronization
 * facilities (lock/semaphore) here.
 *
 * Status: Testing AP JT
 */
void Lizard::monkeyGrass2SagoIsSafe()
{
	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] checking  monkey grass -> sago" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }

    sem_wait(&lizLock); // AP JT
    pthread_mutex_lock(&lock); // AP JT
    numCrossingMonkeyGrass2Sago++; // AP JT
    pthread_mutex_unlock(&lock); // AP JT



	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] thinks  monkey grass -> sago  is safe" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }
}



/**
 * Delays for 1 second to simulate crossing from the monkey
 * grass to the sago.
 *
 * Status: Testing AP JT
 */
void Lizard::crossMonkeyGrass2Sago()
{
    //Checks to see if it is safe then starts
    monkeyGrass2SagoIsSafe(); // AP JT


	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] crossing  monkey grass -> sago" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }

    /*
     * One more crossing this way
     */



    /*
     * Check for lizards cross both ways
     */
    pthread_mutex_lock(&lock); // AP JT
	if (numCrossingSago2MonkeyGrass && UNIDIRECTIONAL)
    {
		cout << "\tOh No!, the lizards have cats all over them." << endl;
		cout << "\t " << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t " << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }
    pthread_mutex_unlock(&lock); // AP JT

	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

	/*
     * That one seems to have made it
     */
	madeIt2Sago(); // AP JT
}


/**
 *
 * Tells others they can go now
 *
 * Status: Testing AP JT
 */
void Lizard::madeIt2Sago()
{
	/*
     * Whew, made it across
     */
    pthread_mutex_lock(&lock); // AP JT
    numCrossingMonkeyGrass2Sago--;
    sem_post(&lizLock); // AP JT
    pthread_mutex_unlock(&lock); // AP JT
	if (debug)
    {
        sem_wait(&debugLock); // AP JT
		cout << "[" << _id << "] made the  monkey grass -> sago  crossing" << endl;
		cout << std::flush;
		sem_post(&debugLock); // AP JT
    }
}





/*
 * main()
 *
 * Should initialize variables, locks, semaphores, etc.
 * Should start the cat thread and the lizard threads
 * Should block until all threads have terminated
 * Status: Incomplete - Make changes to this code.
 */
int main(int argc, char **argv)
{
	/*
	 * Declare local variables
     */
    int i; // AP JT


	/*
     * Check for the debugging flag (-d)
     */
	debug = 0;
	if (argc > 1)
		if (strncmp(argv[1], "-d", 2) == 0)
			debug = 1;


	/*
     * Initialize variables
     */
	numCrossingSago2MonkeyGrass = 0;
	numCrossingMonkeyGrass2Sago = 0;
	running = 1;


	/*
     * Initialize random number generator
	 */
	srandom( (unsigned int)time(NULL) );


	/*
     * Initialize locks and/or semaphores
     */
    pthread_mutex_init(&lock, NULL); // AP JT
    sem_init(&lizLock, 0, MAX_LIZARD_CROSSING); // AP JT
    sem_init(&debugLock, 0, 1); // AP JT



	/*
     * Create NUM_LIZARDS lizard threads
     */
    Lizard** allLizards = new Lizard*[NUM_LIZARDS]; // AP JT
    for(i=0; i<NUM_LIZARDS; i++) // AP JT
        allLizards[i] = new Lizard(i); // AP JT


    /*
     * Create NUM_CATS cat threads
     */
    Cat** allCats = new Cat*[NUM_CATS]; // AP JT
    for(i=0; i<NUM_CATS; i++) // AP JT
        allCats[i] = new Cat(i); // AP JT



    // run lizard and cat threads
    for(i=0; i<NUM_LIZARDS; i++) // AP JT
        allLizards[i]->run(); // AP JT

    for(i=0; i<NUM_CATS; i++) // AP JT
        allCats[i]->run(); // AP JT


	/*
     * Now let the world run for a while
     */
	sleep( WORLDEND );


	/*
     * That's it - the end of the world
     */
	running = 0;


    /*
     * Wait until all threads terminate
     */
    for(i=0; i<NUM_LIZARDS; i++) // AP JT
        allLizards[i]->wait(); // AP JT


    for(i=0; i<NUM_CATS; i++) // AP JT
        allCats[i]->wait(); // AP JT






	/*
     * Delete the locks and semaphores
     */
    pthread_mutex_destroy(&lock); // AP JT
    sem_destroy(&lizLock); // AP JT
    sem_destroy(&debugLock); // AP JT

    /*
     * Delete all cat and lizard objects
     */
    for(i=0; i<NUM_LIZARDS; i++) // AP JT
        delete allLizards[i]; // AP JT
    delete allLizards;
    for(i=0; i<NUM_CATS; i++) // AP JT
        delete allCats[i]; // AP JT
    delete allCats; // AP JT

	/*
     * Exit happily
     */
	return 0;
}






