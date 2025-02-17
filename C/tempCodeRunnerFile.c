for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }