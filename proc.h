#ifndef SHM_MAXNUM
#define SHM_MAXNUM 4
#define MUX_MAXNUM 16              // max # of mutexes
#endif

// Per-CPU state
struct cpu {
	uchar            apicid;     // Local APIC ID
	struct context * scheduler;  // swtch() here to enter scheduler
	struct taskstate ts;         // Used by x86 to find stack for interrupt
	struct segdesc   gdt[NSEGS]; // x86 global descriptor table
	volatile uint    started;    // Has the CPU started?
	int              ncli;       // Depth of pushcli nesting.
	int              intena;     // Were interrupts enabled before pushcli?
	struct proc *    proc;       // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int        ncpu;

// PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
	uint edi;
	uint esi;
	uint ebx;
	uint ebp;
	uint eip;
};

struct shmem {
	int  in_use;
	char name[16];
	char* va;
	struct shared_mem *global_ptr;
};

struct mutex {
	uint 			locked;   // Is the lock held? 0 if not, 1 if it is
	int	    		isAlloc; // 0 if false, 1 if true
	int		    	pid; 	 // pid of who locked this lock
   	char			name[20];    // name of lock
	int				refcount;	// Number of processes that can reference this mutex
	int 			container_id;	// container id, 0 if global (normal xv6 environment), >0 if in container
	int				cv;				// this thingy
};

enum procstate
{
	UNUSED,
	EMBRYO,
	SLEEPING,
	RUNNABLE,
	RUNNING,
	ZOMBIE
};

// Per-process state
struct proc {
	uint              sz;            // Size of process memory (bytes)
	pde_t *           pgdir;         // Page table
	char *            kstack;        // Bottom of kernel stack for this process
	enum procstate    state;         // Process state
	int               pid;           // Process ID
	struct proc *     parent;        // Parent process
	struct trapframe *tf;            // Trap frame for current syscall
	struct context *  context;       // swtch() here to run process
	void *            chan;          // If non-zero, sleeping on chan
	int               killed;        // If non-zero, have been killed
	struct file *     ofile[NOFILE]; // Open files
	struct inode *    cwd;           // Current directory
	char              name[16];      // Process name (debugging)
	struct shmem	  shared_mem[SHM_MAXNUM]; //virtual addresses for the shared memory mapped into this proc
	struct container *container;	//holds the container id of the container this proc is in (0 == global or CM, >1 == in a container)
	uint	          container_id;
	struct mutex *	  mutex[MUX_MAXNUM];// Which locks this process has access to
	uint		  priority;
	struct proc *     next;
};

struct list {
	struct proc *head;
	struct proc *tail;
	int	     size;
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
