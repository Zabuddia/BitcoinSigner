#ifndef MINUNIT_H
#define MINUNIT_H
#include <stdint.h>

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
extern uint32_t tests_run;

#endif //MINUNIT_H