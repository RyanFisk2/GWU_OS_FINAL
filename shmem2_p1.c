#include "types.h"
#include "user.h"

int
main(void)
{
        printf(1,"TESTING PASSING BETWEEN PROCS IN DIFF ANCESTRY:\n");
        //*ptr = 1;
        char *shmem_start = shm_get("test2");
        printf(1,"      setting shared mem 'test2' to 'from_prog1'\n");
        char *var = "from_prog1";
        strcpy(shmem_start,var);
        while(1);
        exit();
}