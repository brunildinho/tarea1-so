//La función se encarga de actualizar el estado de las tareas como ready o blocked.
void UpdateTasks(unsigned time, unsigned n){
    unsigned i;
    /*Itera sobre la cola de tareas*/
    for(i=0; i<n; i++) {
        /*Evalua cada tarea según su estado actual*/
        switch(tasks[i].state){
            /*Si la tarea ya está lista, calcula cuánto tiempo lleva en espera (rtime++)*/
            case ready:
                tasks[i].rtime++;
                break;
            /*Si la tarea está bloqueada (está en I/O), calcula su tiempo de espera
            y el tiempo restante para que termine el I/O */
            case blocked:
                tasks[i].rtime++;
                tasks[i].run[tasks[i].index].io--;
                /*Si la tarea ya terminó el I/O, avanza a la siguiente etapa de la tarea
                (la sección que requeria el I/O para continuar) y cambia el estado de la tarea a lista)*/
                if (tasks[i].run[tasks[i].index].io == 0){
                    tasks[i].index++;
                    tasks[i].state = ready;
                }
                break;
            //Si la tarea actual no está lista o ya está corriendo, no hace nada.
            case notready:
            case running:
            default: break;
        }
    }
}