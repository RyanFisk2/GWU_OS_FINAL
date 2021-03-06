#include "types.h"
#include "user.h"

/*
        prio_set(pid, prio); Returns 0 on success, -1 on fail.


*/



int
main(void)
{
        int pid1,pid2, i, muxid, num_preemptions;
        char *shm_addr;
        printf(1,"TESTING 2 PROCS WITH DIFFERENT PRIORITIES\n");
        sleep(200); /* give time to read above print */
        shm_addr = shm_get("test4");
        pid1 = fork();
        if (pid1 != 0) pid2 = fork();
        if (pid1 != 0 && pid2 != 0) {
                
                muxid = mutex_create("test4");

                if (prio_set(pid1,1) == -1) {
                        printf(1,"set fail\n");
                        wait();
                        wait();
                        exit();
                }
                if (prio_set(pid2,2) == -1) {
                        printf(1,"set fail\n");
                        wait();
                        wait();
                        exit();
                }

                cv_signal(muxid);
                cv_signal(muxid);

                wait();
                wait();
                exit();
        } else if(pid1 == 0){
                
                muxid = mutex_create("test4");
                mutex_lock(muxid);
                cv_wait(muxid);
                mutex_unlock(muxid);

                num_preemptions = 0;
                *shm_addr = 1;
                for (i = 0; i < 500; i++) {
                        printf(1,"1\n");
                        
                        if (*shm_addr != 1) {
                                printf(1,"RACE!\n");
                                num_preemptions++;
                        }       
                        *shm_addr = 1;
                }
                *(shm_addr+1) = num_preemptions;
                
                exit();
        }else{
                muxid = mutex_create("test4");
                mutex_lock(muxid);
                cv_wait(muxid);
                mutex_unlock(muxid);


                *shm_addr = 0;
                for (i = 0; i < 500; i++) {
                        printf(1,"2\n");
                        *shm_addr = 0;
                }
                printf(1,"child1(prio=1) preempted by child2(prio=2) %d times\n",*(shm_addr+1));
                exit();
        }
}