#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */

#include "semaforo_mutex_posix.h"

/* Ejemplo de creación e inicialización de semáforos POSIX para MUTEX con "semáforos con nombre" (named) */

sem_t *initSem() {
	/* name debe ser un nombre de caracteres ascii que comienze con "/", p.e. "/mimutex" */
	sem_t *sem;

	sem = sem_open(SEM_NAME, O_CREAT, S_IRWXU, SEM_INIT_VALUE);
	if (sem == SEM_FAILED) {
		return NULL;
	}
	return sem;
}

void deleteSem() {
	sem_unlink(SEM_NAME);
}

void signalSem(sem_t *sem) {
	sem_post(sem);
}

void waitSem(sem_t *sem) {
	sem_wait(sem);
}
