#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#define GROUP_COUNT 2
#define DISCIP_COUNT 3
#define TEACHER_COUNT 2
#define LOCATION_COUNT 2
#define MAX_OCCUP_OF_GROUP 3
#define MAX_OCCUP_OF_TEACH 3
#define MAX_OCCUP_IN_DAY 3

enum lesson_state
{
    freeLesson,
    busyLesson
};

struct scheduleLine
{
    enum lesson_state state;
    int discipline;
    int location;
    int teacher;
};

struct solution
{
    struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY];
    bool resolve;
};

int groups[GROUP_COUNT];
int disciplines[DISCIP_COUNT];
int teacher[TEACHER_COUNT];
int locations[LOCATION_COUNT];
int plans[GROUP_COUNT][DISCIP_COUNT];
int employments[TEACHER_COUNT][DISCIP_COUNT];

void init()
{
    for (int I = 0; I < GROUP_COUNT; I++)
    {
        groups[I] = I;
        for (int J = 0; J < DISCIP_COUNT; J++)
        {
            plans[I][J] = 1;
        }
    }

    for (int I = 0; I < DISCIP_COUNT; I++)
    {
        disciplines[I] = I;
    }

    for (int I = 0; I < TEACHER_COUNT; I++)
    {
        teacher[I] = I;
        for (int J = 0; J < DISCIP_COUNT; J++)
        {
            employments[I][J] = 1;
        }
    }

    for (int I = 0; I < LOCATION_COUNT; I++)
    {
        locations[I] = I;
    }

    return;
}

int findLocation(struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY], int location, int lesson)
{
    int res = 0;
    for (int I = 0; I < GROUP_COUNT; I++)
    {
        if (schedules[I][lesson].location == location)
            res++;

        if (res >= 2)
            break;
    }
    return res;
}

int findTeacher(struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY], int teacher, int lesson)
{
    int res = 0;
    for (int I = 0; I < GROUP_COUNT; I++)
    {
        if (schedules[I][lesson].teacher == teacher)
            res++;

        if (res >= 2)
        {
            break;
        }
    }
    return res;
}

int getWeight(struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY])
{
    int count = 0;
    for (int J = 0; J < TEACHER_COUNT; J++)
    {
        int cur_teacher = J;
        bool state = false;
        bool worked = false;
        int lessons = 0;
        for (int K = 0; K < MAX_OCCUP_IN_DAY; K++)
        {
            int res = findTeacher(schedules, cur_teacher, K);

            if (res == 1 && !state)
            {
                state == true;
                count += lessons;
                worked = true;
            }

            if (res == 0 && state)
            {
                state = false;
            }

            if (worked && res == 0) {
                lessons++;
            }

            if (count > MAX_OCCUP_OF_TEACH)
                return -1;

        }
    }
    return count;
}

int verifyResolve(struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY], int check_group)
{
    int res = 0;

    int truthPlans = 0;
    for (int I = 0; I < GROUP_COUNT; I++)
    {

        //  check window in schedule for group and plan
        bool startLessons = false;
        bool endLessons = false;
        int countLessons = 0;

        int currentPlan[DISCIP_COUNT];
        for (int J = 0; J < DISCIP_COUNT; J++)
        {
            currentPlan[J] = 0;
        }

        for (int J = 0; J < MAX_OCCUP_IN_DAY; J++)
        {
            if (schedules[I][J].state == busyLesson && !startLessons)
            {
                startLessons = true;
            }

            if (schedules[I][J].state == freeLesson && startLessons)
            {
                endLessons = true;
            }

            //  windows of group plan
            if (schedules[I][J].state == busyLesson && endLessons)
            {
                return -1;
            }

            if (schedules[I][J].state == busyLesson && startLessons)
            {
                countLessons++;

                if (countLessons > MAX_OCCUP_OF_GROUP)
                {
                    return -1;
                }

                int discp = schedules[I][J].discipline;

                currentPlan[discp]++;
                //  current plan is incorrect
                if (currentPlan[discp] > plans[I][discp])
                {
                    return -1;
                }
            }

            if (schedules[I][J].state == busyLesson)
            {
                int teacherLocation = findTeacher(schedules, schedules[I][J].teacher, J);
                if (teacherLocation >= 2)
                    return -1;

                int locationState = findLocation(schedules, schedules[I][J].location, J);
                if (locationState >= 2)
                {
                    return -1;
                }
            }
        }

        //  check plan
        bool state = true;
        for (int J = 0; J < DISCIP_COUNT; J++)
        {
            if (currentPlan[J] != plans[I][J] && I >= check_group)
            {
                state = false;
                break;
            }

            if (currentPlan[J] != plans[I][J] && I < check_group) {
                return -1;
            }
        }

        //  plan is ok
        if (state)
        {
            truthPlans++;
        }
    }

    //  check teacher
    int truthEmploy = 0;
    for (int I = 0; I < TEACHER_COUNT; I++)
    {
        int cur_teacher = I;
        bool state = false;
        int count = 0;
        int currentEmploy[DISCIP_COUNT];
        for (int J = 0; J < DISCIP_COUNT; J++)
        {
            currentEmploy[J] = 0;
        }

        for (int J = 0; J < MAX_OCCUP_IN_DAY; J++)
        {
            int res = findTeacher(schedules, cur_teacher, J);
            if (res > 2)
            {
                return -1;
            }

            if (res == 1)
            {
                state = true;
                for (int K = 0; K < GROUP_COUNT; K++)
                {
                    if (schedules[K][J].teacher == I)
                    {
                        int discp = schedules[K][J].discipline;
                        currentEmploy[discp]++;

                        if (currentEmploy[discp] > employments[I][discp])
                        {
                            return -1;
                        }

                        break;
                    }
                }
            }

            if (res == 1 && !state)
            {
                state == true;
            }

            if (res == 1 && state)
            {
                count++;
            }

            if (count > MAX_OCCUP_OF_TEACH)
                return -1;
        }

        state = true;
        for (int J = 0; J < DISCIP_COUNT; J++)
        {
            if (currentEmploy[J] != employments[I][J])
            {
                state = false;
                break;
            }
        }

        if (state)
        {
            truthEmploy++;
        }
    }

    if (truthPlans == GROUP_COUNT && truthEmploy == TEACHER_COUNT)
        res = 1;

    return res;
}

struct solution direct(struct solution current, int group, int lesson)
{
    int state = verifyResolve(current.schedules, (group - 1));
    if (state > 0)
    {
        current.resolve = true;
        return current;
    }
    else if (state < 0)
    {
        return current;
    }
    else if (state == 0 && (group == GROUP_COUNT) && (lesson == MAX_OCCUP_IN_DAY))
    {
        return current;
    }

    struct solution optim = current;
    int weight = __INT_MAX__;

    // перебор вариантов
    for (int D = -1; D < DISCIP_COUNT; D++)
    {
        for (int L = 0; L < LOCATION_COUNT; L++)
        {
            for (int T = 0; T < TEACHER_COUNT; T++)
            {
                if (D != -1)
                {
                    current.schedules[group][lesson].state = busyLesson;
                    current.schedules[group][lesson].location = L;
                    current.schedules[group][lesson].teacher = T;
                    current.schedules[group][lesson].discipline = D;
                }
                int next_group = group;
                int next_lesson = lesson;
                bool less = false;
                if (lesson < MAX_OCCUP_IN_DAY - 1)
                {
                    next_lesson++;
                    less = true;
                }
                else if (group < GROUP_COUNT - 1)
                {
                    next_group++;
                    next_lesson = 0;
                    less = true;
                }
                else if (next_lesson == GROUP_COUNT - 1 && next_lesson == MAX_OCCUP_IN_DAY - 1)
                {
                    less = true;
                    next_group++;
                    next_lesson++;
                }

                if (less)
                {
                    struct solution temp = direct(current, next_group, next_lesson);
                    if (temp.resolve)
                    {
                        int curWeight = getWeight(temp.schedules);

                        if (curWeight >= 0 && (optim.resolve == false || ((optim.resolve == true) && (curWeight < weight))))
                        {
                            optim = temp;
                            weight = curWeight;
                            if (curWeight == 0) {
                                return optim;
                            }
                        }
                    }
                }
            }
        }
    }
    return optim;
}

struct solution openMP(struct solution current, int group, int lesson, int thread)
{
    int state = verifyResolve(current.schedules, (group - 1));
    if (state > 0)
    {
        current.resolve = true;
        return current;
    }
    else if (state < 0)
    {
        return current;
    }
    else if (state == 0 && (group == GROUP_COUNT) && (lesson == MAX_OCCUP_IN_DAY))
    {
        return current;
    }
    struct solution optim = current;
    int weight = __INT_MAX__;
    bool w = false;

    #pragma omp parallel num_threads(thread)
    {
        #pragma omp for
        for (int D = -1; D < DISCIP_COUNT; D++)
        {
            if (w) {
                #pragma omp cancel for
            }
            for (int L = 0; L < LOCATION_COUNT; L++)
            {
                if (w) {
                    #pragma omp cancel for
                }
                for (int T = 0; T < TEACHER_COUNT; T++)
                {
                    if (w) {
                        #pragma omp cancel for
                    }
                    if (D != -1)
                    {
                        current.schedules[group][lesson].state = busyLesson;
                        current.schedules[group][lesson].location = L;
                        current.schedules[group][lesson].teacher = T;
                        current.schedules[group][lesson].discipline = D;
                    }
                    int next_group = group;
                    int next_lesson = lesson;
                    bool less = false;
                    if (lesson < MAX_OCCUP_IN_DAY - 1)
                    {
                        next_lesson++;
                        less = true;
                    }
                    else if (group < GROUP_COUNT - 1)
                    {
                        next_group++;
                        next_lesson = 0;
                        less = true;
                    }
                    else if (next_lesson == GROUP_COUNT - 1 && next_lesson == MAX_OCCUP_IN_DAY - 1)
                    {
                        less = true;
                        next_group++;
                        next_lesson++;
                    }

                    if (less)
                    {
                        struct solution temp = openMP(current, next_group, next_lesson, thread);
                        if (temp.resolve)
                        {
                            int curWeight = getWeight(temp.schedules);

                            if (curWeight >= 0 && (optim.resolve == false || ((optim.resolve == true) && (curWeight < weight))))
                            {
                                #pragma omp cancel for
                                {
                                    optim = temp;
                                    weight = curWeight;
                                    if (curWeight == 0) {
                                        w = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return optim;
}

void directInit()
{
    struct solution start;
    start.resolve = false;
    for (int I = 0; I < GROUP_COUNT; I++)
    {
        for (int J = 0; J < MAX_OCCUP_IN_DAY; J++)
        {
            start.schedules[I][J].state = freeLesson;
            start.schedules[I][J].discipline = -1;
            start.schedules[I][J].location = -1;
            start.schedules[I][J].teacher = -1;
        }
    }

    struct solution end = direct(start, 0, 0);
    return;
}

void openMPInit(int threads)
{
    struct solution start;
    start.resolve = false;
    for (int I = 0; I < GROUP_COUNT; I++)
    {
        for (int J = 0; J < MAX_OCCUP_IN_DAY; J++)
        {
            start.schedules[I][J].state = freeLesson;
            start.schedules[I][J].discipline = -1;
            start.schedules[I][J].location = -1;
            start.schedules[I][J].teacher = -1;
        }
    }

    struct solution end = openMP(start, 0, 0, threads);
    return;
}

void main()
{
    init();
    time_t start, end;
    printf("Прямой метод:\n");
    time(&start);
    for (int I=0; I < 100; I++) {
        directInit();
    }
    time(&end);

    double diff = difftime(end, start);
    printf("\tВремя(t): %f\n", (diff/ 100));
    printf("\tВремя работы к числу потоков: %f\n", (diff/ 100));
    printf("\tКПД одного потока: 100\%\n");


    int count_proc[] = {1, 2, 3, 4, 8};
    for (int I = 0; I < 5; I++)
    {
        printf("OpenMP: %d поток(-a)\n", count_proc[I]);
        int iter = 1000;
        if (I == 0)
            iter = 100;
        time(&start);
        for (int J = 0; J < iter; J++) 
        {
            openMPInit(count_proc[I]);
        }
        time(&end);

        diff = difftime(end, start);
        printf("\tВремя(t): %f\n", (diff/ iter));
        printf("\tВремя работы к числу потоков: %f\n", (diff/ (iter * count_proc[I])));
        double cpd = 1.0 / count_proc[I];
        printf("\tКПД одного потока: %.2f\%\n", cpd * 100);
    }
    return;
}