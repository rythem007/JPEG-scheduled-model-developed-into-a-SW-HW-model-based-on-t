//-----Function definitions for classes os, os_taski and os_sem---
//-----You must modify this file as indicated by TODO comments----
//-----You should reuse code from Programming Assignment 3--------
//-----as indicated by the REUSE comments-------------------------

//-----Note that new code must be written for os_sem methods------
//-----The time_wait method in OS must be rewritten---------------
//-----to account for preemption----------------------------------

#include "systemc.h"
#include "os.h"

using namespace std;

// implementation of os_task methods

//placeholder constructor since we have a pool
os_task::os_task() {
}

os_task::~os_task(){
}

void os_task::instantiate(sc_core::sc_process_handle handle, const char *name, unsigned int priority){
	static unsigned int count = 0;
	task_name = name;
	// All tasks are initially ready when created
	task_state = READY;
	task_id = count;
	task_priority = priority;
	task_handle = handle;
	activation_flag = false;
	count++;
}

const char *os_task::get_name(){
	return task_name.data();
}

enum state os_task::get_state(){
	return task_state;
}

unsigned int os_task::get_id(){
	return task_id;
}

unsigned int os_task::get_priority(){
	return task_priority;
}

sc_core::sc_process_handle os_task::get_handle(){
	return task_handle;
}

void os_task::activate(){
	// REUSE: set the activation flag and notify the event
	
	
	   activation_flag=true;
	notify(activation_event);
	//task_state = RUNNING;
	
	return;
}

void os_task::wait_for_activation(){
	// REUSE: If the activation flag is not set,
	// REUSE: wait for it to be set
	if(activation_flag == false)
	{
	   wait(activation_event);
}
        // REUSE: reset the activation flag
		activation_flag = false;

	// REUSE: update the task state
	    task_state = READY;

	return;
}

//os class method implementation

os::os(){
	// empty constructor
}

os::~os(){

}

// dynamic task creation function
// this function is called by the main SC thread in the top module
// it may also be called by tasks to dynamically create children

os_task *os::task_create(
        const char* name,
        unsigned int priority,
	sc_core::sc_process_handle h)
{
        // get a task object from the pool 
        static int count = 0;
	os_task *t = &(task_pool[count++]);

	// REUSE: instantiate the user task in the OS model
	t->instantiate(h, name,priority);

        // REUSE: add the task to the ALL_TASKS list
		ALL_TASKS.push_back(t);

        //newly created task is in the ready state
        // REUSE: add it to READY vector list
		READY_TASKS.push_back(t);

	// return the task pointer
        return t;
}

// task initialization function
// this function is called by the SystemC thread
// for the task, at the very beginning

void os::task_init(){

        // REUSE: find this task's pointer "t"
		int i_counter = 0;
        os_task *t = NULL;
		sc_process_handle p = sc_get_current_process_handle();		
		int x;
		while(i_counter < ALL_TASKS.size())
		{
			  if (ALL_TASKS[i_counter]->get_handle() == p){
				t = ALL_TASKS[i_counter];
				x = i_counter; 
				break;
			  }
			  i_counter++;
		}
		
		t = ALL_TASKS[x];

        // REUSE: wait for the task to be scheduled by the OS
		
		t->wait_for_activation(); 

        cout << sc_time_stamp() << ": Task " << t->get_name() << " is initialized" << endl;

	return;
}

// task termination function
// this function is called by the running task
// to terminate itself

void os::task_terminate(){
	// REUSE: update the state of the task

	// REUSE: do a rescheduling on remaining tasks
	// REUSE: wait till end of simulation if no tasks remain
	
	
	
	
	int i_counter = 0;
		os_task *t;
		sc_process_handle p = sc_get_current_process_handle();	
		int x;
		while(i_counter < ALL_TASKS.size())
		{
					      
			  if (ALL_TASKS[i_counter]->get_handle() == p){
				t = ALL_TASKS[i_counter];
				x = i_counter; 
				break;
			  }
			  i_counter++;
		}
        
		t = ALL_TASKS[x];	
	t->task_state= TERMINATED;
	
	int j_counter = 0;
	while(j_counter < READY_TASKS.size())
		{
			os_task *temp_t = READY_TASKS[j_counter];	      
			  if(temp_t == t){
					READY_TASKS.erase(READY_TASKS.begin()+j_counter);
					break;
				}
			  j_counter++;
		}
	
	schedule();
	
	if(ALL_TASKS.size() == 0)
	
	{
		wait();
}
}

// this function displays the states of
// all the tasks in the order of their creation 
// It is called by the OS schedule method
// whenever a rescheduling takes place

void os::status(){
  cout << endl <<  sc_time_stamp() << ":" << endl;

  for (int i = 0; i < ALL_TASKS.size(); i++){
    os_task *t = ALL_TASKS[i];
    cout << t->get_name() << ": ";
    if (t->get_state() == RUNNING)
	cout << "RUNNING" << endl;
    else if (t->get_state() == READY)
	cout << "READY" << endl;
    else if (t->get_state() == SUSPENDED)
	cout << "SUSPENDED" << endl;
    else if (t->get_state() == TERMINATED)
	cout << "TERMINATED" << endl;
  }
}

void os::time_wait(sc_time t)
{
  

  // TODO: use simultaneous wait on time and preemption event
  // TODO: if preemption event arrives before all the time
  // TODO: has been consumed, go to ready state, wait for activation
  // TODO: then consume the remainder time
  // TODO: repeat until all of "t" has been consumed
  sc_time minus,back_time,ahead_time,consumed(0,SC_US),null(0,SC_US);
	minus = t;
	os_task *temp_t = CURRENT;
	consumed = ahead_time - back_time;
	while ( minus > null )
	{
  	back_time = sc_time_stamp();
        wait(minus,preemption);  
  	ahead_time = sc_time_stamp();
  	consumed = ahead_time - back_time + consumed;
  	minus = t - consumed;
  	if ( minus > null )
  	{
			temp_t->task_state = READY;
			READY_TASKS.push_back(temp_t);
			CURRENT=NULL;
			schedule();
			temp_t->wait_for_activation();
			
	}
}
temp_t=NULL;
  return;
} // end time_wait

ERROR os::schedule() {

  // TODO: If there are no tasks or all tasks have terminated
   int count11 = 0;
   for (int i = 0;i < ALL_TASKS.size(); i++)
   {
		os_task *t = ALL_TASKS[i];
		if (t->get_state() != TERMINATED)
		{
			count11=1;
		}
	}

   // TODO: Pring the status and return code E_NO_TASKS
   if(count11==0)
	{
		status();
		return E_NO_TASKS;
	}
	
   // TODO: If all tasks are suspended, just display
   // TODO: the states and return code E_OK
   // otherwise, we have some scheduling to do!
   // TODO: find the highest priority ready task
   unsigned int Pri = 0;
   for(int i = 0;i < READY_TASKS.size();i++){
       if(READY_TASKS[i]->get_priority() >= Pri){
	       Pri = READY_TASKS[i]->get_priority();
	   }
   }
   
  // TODO: remove the highest priority task from the ready list
  // TODO: and activate it!
  for(int i = 0;i < READY_TASKS.size();i++){
      if(Pri == READY_TASKS[i]->get_priority()){
	      CURRENT = READY_TASKS[i];
	      CURRENT->task_state = RUNNING;
	      READY_TASKS.erase(READY_TASKS.begin()+i);   //*** where should we put the erased task? in the ALLTASK lists?
	  }                                        //*** Do we need to refresh the task_state in ALLTASK list
  }
  CURRENT->activate();
  // print out the status of all the tasks
  status();
  return E_OK;
} // end schedule

void os::run() {
  // all the initial tasks have been created
  // kickstart the scheduler
  cout << "Initializing Scheduler..." <<endl;
  schedule();
}

// os_sem method implementation
//-------------------------------------------------------------

int os_sem::wait() {
  int retval;
  class os_task *temp_t = OS->CURRENT;
  // TODO: if the semaphore value is 0
  // TODO: the accessing task must be blocked
  if (sc_semaphore :: get_value() ==0) {
          // TODO: block the caller
		  OS->CURRENT = NULL;
		  blocked_task = temp_t;
		  temp_t->task_state = SUSPENDED;
		  cout << sc_time_stamp() <<": Task "<< temp_t->get_name() << " blocked on semaphore " << sc_semaphore::name() << "\n";
		   
		   
		 

          // TODO: set the blocked_task pointer
		
		   
          // TODO: find the next active task and run it
		  OS->schedule();
		  blocked_task->wait_for_activation();
		 
		  
		 
  }
  temp_t = OS->CURRENT;

  // proceed by running the base class method
  retval = sc_semaphore::wait();
  return retval;
} // end os_sem wait method


int os_sem::post() {
 int retval ;
  bool S = 1;
  bool term=1;
 // bool T = 1;
  //class os_task *temp_task = OS->CURRENT;
  // increment the semaphore value
  // by calling the base class method
  retval = sc_semaphore::post();
  
  // TODO: if there is a blocked task, it should be unblocked
    if (blocked_task != NULL)
    {
    for ( int i=0 ; i < OS->ALL_TASKS.size() ; i++)
 			{
	 			os_task *t = OS->ALL_TASKS[i];
                S = ((( t->get_state() == SUSPENDED ) && S) || (( t->get_state() == TERMINATED)&&term))&& S;
            }
            
                    if ( S )
                    {
                        blocked_task->task_state = READY;
                        OS->READY_TASKS.push_back(blocked_task);
                        OS->schedule();   
                	}
                
  
		 // TODO: if all tasks are suspended
         // TODO: run the scheduler here               
		 // TODO: after waking up the blocked task
    
         else if ( OS->CURRENT->get_priority() < blocked_task->get_priority()) // TODO: if there is a running task with lower priority
         {
	          	// TODO: the running task should be preempted
	          	OS->CURRENT->task_state = READY;
                OS->READY_TASKS.push_back(blocked_task);
                os_task *temp_task = blocked_task;

                //TODO: the message below should be printed only in the case of preemption
                cout << sc_time_stamp() <<": Task "<< OS->CURRENT->get_name() << " preempted by task " << temp_task->get_name() << "\n";
                OS->preemption.notify();
         }   
        blocked_task = NULL;
       
        }
  return retval;
  
} // end os_sem post method

