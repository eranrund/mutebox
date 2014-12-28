#include <signal.h>
#include <stdio.h>
#include <pthread.h>

pthread_t t1, t2, t3;
pthread_mutex_t m1, m2;


void thread1(void * arg) {
    int i = 0;
    char buf[] = "T1 00\n";
    while (1) {
        i++;
        buf[4] = (i % 10) + '0';
        write(1, buf, 6);
        sleep(1);
    }
}

void thread2(void * arg) {
    int i = 0;
    char buf[] = "T2 00\n";
    while (1) {
        i++;
        buf[4] = (i % 10) + '0';
        write(1, buf, 6);
        sleep(1);
    }
}
void thread3(void * arg) {
    printf("--->\n");
    pthread_mutex_lock(&m1);
    pthread_kill(t1, SIGUSR1);
    pthread_mutex_lock(&m2);
    pthread_kill(t2, SIGUSR1);
    for (int i = 0; i < 3; i++) {        
        printf("aaaaa\n");
        sleep(1);
    }

    pthread_mutex_unlock(&m1);
    pthread_mutex_unlock(&m2);
}

void sig_func(int sig) {
    if (pthread_self() == t1) {
        write(1, "T1\n", 3);
        pthread_mutex_lock(&m1);
        write(1, "t1\n", 3);
        pthread_mutex_unlock(&m1);

    } else if (pthread_self() == t2) {
        write(1, "T2\n", 3);
        pthread_mutex_lock(&m2);
        write(1, "t2\n", 3);
        pthread_mutex_unlock(&m2);

    } else {
        write(1, "??\n", 3);
    }
    signal(SIGUSR1,sig_func);
}


main() {
    signal(SIGUSR1,sig_func);

    pthread_mutex_init(&m1,NULL);
    pthread_mutex_init(&m2,NULL);

    pthread_create(&t1, NULL, thread1, NULL);    
    pthread_create(&t2, NULL, thread2, NULL);    
    sleep(3);
    pthread_create(&t3, NULL, thread3, NULL);    

    while(1) sleep(100);

    while (1) {
        printf("SIG T1\n");
        pthread_kill(t1, SIGUSR1);
//        sleep(1);
        printf("SIG T2\n");
        pthread_kill(t2, SIGUSR1);
  //      sleep(1);

    }
}
