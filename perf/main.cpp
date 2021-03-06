#include <vector>
#include <unistd.h>
#include "libsk.h"

#define SHM_PATH_PREFIX "/libsk-perf"

// NOTE: LOOP_COUNT must be 100 * N
#define LOOP_COUNT 100000 // 1000000

#define TINY_COUNT 1000
#define HUGE_COUNT 100

#define SIZE_RANGE_MIN 32
#define SIZE_RANGE_MAX (1024 * 1024)

#define FIXED_SIZE_COUNT 3

inline int rand_range(int min, int max) {
    if (min > max) std::swap(min, max);

    return min == max ? min : (rand() % (max - min + 1) + min);
}

inline bool rand_bool() {
    return rand_range(0, 1) == 1;
}

size_t random_size() {
    return rand_range(SIZE_RANGE_MIN, SIZE_RANGE_MAX);
}

size_t fixed_size() {
    static bool first = true;
    static size_t sizes[FIXED_SIZE_COUNT] = {0};

    if (first) {
        for (int i = 0; i < FIXED_SIZE_COUNT; ++i)
            sizes[i] = rand_range(SIZE_RANGE_MIN, SIZE_RANGE_MAX);

        first = false;
    }

    return sizes[rand_range(0, FIXED_SIZE_COUNT - 1)];
}

char *shm_new(size_t size, sk::shm_ptr<void>& ptr) {
    ptr = sk::shm_malloc(size);
    return char_ptr(ptr.get());
}

void shm_del(sk::shm_ptr<void> ptr) {
    sk::shm_free(ptr);
}

char *heap_new(size_t size, char*& ptr) {
    ptr = new char[size];
    return ptr;
}

void heap_del(char *ptr) {
    delete[] ptr;
}

template<typename P, size_t(*S)(), char*(*M)(size_t, P&), void(*F)(P)>
int test_allocation_deallocation() {
    std::vector<P> pointers;
    pointers.reserve(LOOP_COUNT);

    int c = 0;
    while (++c <= LOOP_COUNT) {
        size_t size = S();
        P p;
        char *addr = M(size, p);
        if (!addr) {
            printf("allocation<size: %lu> failed.", size);
            return -1;
        }

        pointers.push_back(p);

        if (c % 100 == 0) {
            for (int i = 0; i < (int) pointers.size(); ++i)
                F(pointers[i]);

            pointers.clear();
        }
    }

    return 0;
}

void print_time_cost(const char *test_type, const timeval& begin, const timeval& end) {
    timeval handle;
    if (end.tv_usec < begin.tv_usec) {
        handle.tv_usec = (1000000 + end.tv_usec) - begin.tv_usec;
        handle.tv_sec  = end.tv_sec - begin.tv_sec - 1;
    } else {
        handle.tv_usec = end.tv_usec - begin.tv_usec;
        handle.tv_sec  = end.tv_sec - begin.tv_sec;
    }

    printf("test type: %s;\t\t time cost: %f ms.\n", test_type, (float) (handle.tv_sec * 1000.0 + handle.tv_usec / 1000.0));
}

void parse_parameters(int argc, char **argv, size_t& shm_size) {
    shm_size  = 0;

    int opt;
    while ((opt = getopt(argc, argv, "s:h:")) != -1) {
        switch (opt) {
        case 's':
            shm_size = atol(optarg);
            break;
        case 'h':
            fprintf(stdout, "Usage: %s -s shm_size.\n", argv[0]);
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Usage: %s -s shm_size.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (shm_size == 0) {
        fprintf(stderr, "Usage: %s -s shm_size.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main() {
    srand(time(NULL));

    // parse_parameters(argc, argv, shm_size);

    int ret = sk::shm_init(SHM_PATH_PREFIX, false);
    if (ret != 0) {
        fprintf(stderr, "cannot init shm, ret: %d\n", ret);
        exit(EXIT_FAILURE);
    }

    timeval begin_time, end_time;

    // 1. new/delete, random size
    {
        gettimeofday(&begin_time, NULL);
        test_allocation_deallocation<char *, random_size, heap_new, heap_del>();
        gettimeofday(&end_time, NULL);

        print_time_cost("new/delete, random size", begin_time, end_time);
    }

    // 2. new/delete, fixed size
    {
        gettimeofday(&begin_time, NULL);
        test_allocation_deallocation<char *, fixed_size, heap_new, heap_del>();
        gettimeofday(&end_time, NULL);

        print_time_cost("new/delete,  fixed size", begin_time, end_time);
    }

    // 3. shm mgr, random size
    {
        gettimeofday(&begin_time, NULL);
        test_allocation_deallocation<sk::shm_ptr<void>, random_size, shm_new, shm_del>();
        gettimeofday(&end_time, NULL);

        print_time_cost("shm_mgr,    random size", begin_time, end_time);
    }

    // 4. shm mgr, fixed size
    {
        gettimeofday(&begin_time, NULL);
        test_allocation_deallocation<sk::shm_ptr<void>, fixed_size, shm_new, shm_del>();
        gettimeofday(&end_time, NULL);

        print_time_cost("shm_mgr,     fixed size", begin_time, end_time);
    }

    // sk::shm_mgr::get()->report();
    sk::shm_fini();

    return 0;
}
