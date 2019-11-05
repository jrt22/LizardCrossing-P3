/***************************************************************/
/*                                                             */
/* lizard.cpp                                                  */
/*                                                             */
/* To compile, you need all the files listed below             */
/*   lizard.cpp                                                */
/*                                                             */
/* Be sure to use the -lpthread option for the compile command */
/*   g++ -g -Wall lizard.cpp -o lizard -lpthread               */
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

{


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
}

/*
 * Declare global variables here
 */

/*
mutex lock is for cats and keeping track of crossing lizards
semaphore is for lizards
*/
pthread_mutex_t lock; // AP JT
sem_t lizLock; // AP JT

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
		void run();
		void wait();

	private:
		void* runThread(void *param);
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
 * Launches a cat thread.
 *
 * Status: Test AP JT
 */
 void Cat::run()
 {
	 // launch the thread to simulate the cat's behavior
	 pthread_create(&_thread, NULL, runThread, &NULL); // AP JT

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
	if (debug)
    {
		cout << "[" << _id << "] cat is alive\n";
		cout << std::flush;
    }

	while(running)
    {
		sleepNow();

        //freezes cats and lizards to check lizard crossing
        pthread_mutex_lock(&catLock); // AP JT
		/*
	     * Check for too many lizards crossing
	     */
		if (numCrossingSago2MonkeyGrass + numCrossingMonkeyGrass2Sago > MAX_LIZARD_CROSSING)
		{
		  cout << "\tThe cats are happy - they have toys.\n";
		  exit( -1 );
		}
		pthread_mutex_unlock(&catLock); // AP JT
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
		cout << "[" << _id << "] cat sleeping for " << sleepSeconds << " seconds" << endl;
		cout << std::flush;
    }

	sleep( sleepSeconds );

	if (debug)
    {
		cout << "[" << _id << "] cat awake" << endl;
		cout << std::flush;
    }
}


class Lizard {
	int       _id;     // the Id of the lizard
	pthread_t _thread; // the thread simulating the lizard

	public:
		Lizard(int id);
		void run();
		void wait();

	private:
		void* runThread(void *param);
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
 * Launches a lizard thread.
 *
 * Status: Test AP JT
 */
 void Lizard::run()
 {
	 // launch the thread to simulate the lizard's behavior
	 pthread_create(&_thread, NULL, runThread, &NULL); // AP JT
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
	if (debug)
    {
      cout << "[" << _id << "] lizard is alive" << endl;
      cout << std::flush;
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
      cout << "[" << _id << "] sleeping for " << sleepSeconds << " seconds" << endl;
      cout << std::flush;
    }

	sleep( sleepSeconds );

	if (debug)
    {
      cout << "[" << _id << "] awake" << endl;
      cout << std::flush;
    }
}




/**
 *
 * Returns when it is safe for this lizard to cross from the sago
 * to the monkey grass.   Should use some synchronization
 * facilities (lock/semaphore) here.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::sago2MonkeyGrassIsSafe()
{
	if (debug)
    {
		cout << "[" << _id << "] checking  sago -> monkey grass" << endl;
		cout << std::flush;
    }




	if (debug)
    {
		cout << "[" << _id << "] thinks  sago -> monkey grass  is safe" << endl;
		cout << std::flush;
    }
}


/**
 *
 * Delays for 1 second to simulate crossing from the sago to
 * the monkey grass.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::crossSago2MonkeyGrass()
{
	if (debug)
    {
      cout << "[" << _id << "] crossing  sago -> monkey grass" << endl;
      cout << std::flush;
    }

	/*
	 * One more crossing this way
	 */
	numCrossingSago2MonkeyGrass++;

	/*
     * Check for lizards cross both ways
     */
	if (numCrossingMonkeyGrass2Sago && UNIDIRECTIONAL)
    {
		cout << "\tCrash!  We have a pile-up on the concrete." << endl;
		cout << "\t" << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t" << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }


	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

    /*
     * That one seems to have made it
     */
    numCrossingSago2MonkeyGrass--;
}


/**
 * Tells others they can go now
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::madeIt2MonkeyGrass()
{
	/*
     * Whew, made it across
     */
	if (debug)
    {
		cout << "[" << _id << "] made the  sago -> monkey grass  crossing" << endl;
		cout << std::flush;
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
		cout << "[" << _id << "] eating for " << eatSeconds << " seconds" << endl;
		cout << std::flush;
    }

	/*
     * Simulate eating by blocking for a few seconds
     */
	sleep( eatSeconds );

	if (debug)
    {
      cout << "[" << _id << "] finished eating" << endl;
      cout << std::flush;
    }
}


/**
 * Returns when it is safe for this lizard to cross from the monkey
 * grass to the sago.   Should use some synchronization
 * facilities (lock/semaphore) here.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::monkeyGrass2SagoIsSafe()
{
	if (debug)
    {
		cout << "[" << _id << "] checking  monkey grass -> sago" << endl;
		cout << std::flush;
    }





	if (debug)
    {
		cout << "[" << _id << "] thinks  monkey grass -> sago  is safe" << endl;
		cout << std::flush;
    }
}



/**
 * Delays for 1 second to simulate crossing from the monkey
 * grass to the sago.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::crossMonkeyGrass2Sago()
{
	if (debug)
    {
		cout << "[" << _id << "] crossing  monkey grass -> sago" << endl;
		cout << std::flush;
    }

    /*
     * One more crossing this way
     */
	numCrossingMonkeyGrass2Sago++;


    /*
     * Check for lizards cross both ways
     */
	if (numCrossingSago2MonkeyGrass && UNIDIRECTIONAL)
    {
		cout << "\tOh No!, the lizards have cats all over them." << endl;
		cout << "\t " << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t " << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }

	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

	/*
     * That one seems to have made it
     */
	numCrossingMonkeyGrass2Sago--;
}


/**
 *
 * Tells others they can go now
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::madeIt2Sago()
{
	/*
     * Whew, made it across
     */
	if (debug)
    {
		cout << "[" << _id << "] made the  monkey grass -> sago  crossing" << endl;
		cout << std::flush;
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




	/*
     * Create NUM_LIZARDS lizard threads
     */


    /*
     * Create NUM_CATS cat threads
     */



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






	/*
     * Delete the locks and semaphores
     */



	/*
     * Exit happily
     */
	return 0;
}






