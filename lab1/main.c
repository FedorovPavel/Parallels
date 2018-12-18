#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define GROUP_COUNT 4
#define DISCIP_COUNT 4
#define TEACHER_COUNT 4
#define LOCATION_COUNT 4
#define MAX_OCCUP_OF_GROUP 4
#define MAX_OCCUP_OF_TEACH 4
#define MAX_OCCUP_IN_DAY 4

enum lesson_state {freeLesson, busyLesson};

struct teacherEmployment {
    int discipline;
    int count;
};

struct teacher {
    int name;
    struct teacherEmployment* employment;
};

struct plan {
    int discipline;
    int count;
};

struct group {
    int name; 
    struct plan* plans;
};

struct scheduleLine {
    enum lesson_state state;
    int discipline;
    int location;
    int teacher;
};

struct solution {
    struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY];
    bool resolve;
};

int groups[GROUP_COUNT];
int disciplines[DISCIP_COUNT];
int teacher [TEACHER_COUNT];
int locations [LOCATION_COUNT];
int plans[GROUP_COUNT][DISCIP_COUNT];
int employments[TEACHER_COUNT][DISCIP_COUNT];

void init() {
    for (int I = 0; I < GROUP_COUNT; I++) {
        groups[I] = I;
        for (int J = 0; J < DISCIP_COUNT; J++) {
            plans[I][J] = 1;
        }
    }

    for (int I = 0; I < DISCIP_COUNT; I++) {
        disciplines[I] = I;
    }

    for (int I = 0; I < TEACHER_COUNT; I++) {
        teacher[I] = I;
        for (int J = 0; J < DISCIP_COUNT; J++) {
            employments[I][J] = 1;
        }
    }

    for (int I = 0; I < LOCATION_COUNT; I++) {
        locations[I] = I;
    }

    return;
}

int findLocation(struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY], int location, int lesson) {
    int res = 0;
    for (int I = 0; I < GROUP_COUNT; I++) {
        if (schedules[I][lesson].location == location)
            res++;
        
        if (res >= 2)
            break;
    }
    return res;
}

int findTeacher(struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY], int teacher, int lesson) {
    int res = 0;
    for (int I = 0; I < GROUP_COUNT; I++) {
        if (schedules[I][lesson].teacher == teacher) 
            res++;

        if (res >= 2) {
            break;
        }
    }
    return res;
}

int verifyStep(struct scheduleLine schedules[GROUP_COUNT][MAX_OCCUP_IN_DAY])
{
    int res = 1;

    for (int I = 0; I < GROUP_COUNT; I++) {

        //  check window in schedule for group
        bool startLessons = false;
        bool endLessons = false;
        int countLessons = 0;

        for(int J = 0; J < MAX_OCCUP_IN_DAY; J++) {
            if (schedules[I][J].state == busyLesson && !startLessons){
                startLessons = true;
            }

            if (schedules[I][J].state == freeLesson && startLessons) {
                endLessons = true;
            }

            //  windows of group plan
            if (schedules[I][J].state == busyLesson && endLessons) {
                return -1;
            }

            if (schedules[I][J].state == busyLesson && startLessons) {
                countLessons++;

                if (countLessons > MAX_OCCUP_OF_GROUP) {
                    return -1;
                }
            }

            if (schedules[I][J].state != freeLesson) {
                int teacherLocation = findTeacher(schedules, schedules[I][J].teacher, J);
                if (teacherLocation >= 2)
                    return -1;

                int locationState = findLocation(schedules, schedules[I][J].location, J);
                if (locationState >= 2) {
                    return -1;
                }
            }
        }

        for (int J = 0; J < TEACHER_COUNT; J++) {
            int cur_teacher = J;
            bool state = false;
            int count = 0;
            for (int K = 0; K < MAX_OCCUP_IN_DAY; K++) {
                int res = findTeacher(schedules, cur_teacher, K);
                if (res == 1)
                    count++;

                if (res == 1 && !state) {
                    state == true;
                }

                if (count > MAX_OCCUP_OF_TEACH)
                    return -1;
            }
        }
    }

    return res;
}

int verifyResolve(struct scheduleLine **schedules) {
    int res = 0;
    
    for (int I = 0; I < GROUP_COUNT; I++) {

        //  check window in schedule for group
        bool startLessons = false;
        bool endLessons = false;
        int countLessons = 0;

        for(int J = 0; J < MAX_OCCUP_IN_DAY; J++) {
            if (schedules[I][J].state == busyLesson && !startLessons){
                startLessons = true;
            }

            if (schedules[I][J].state == freeLesson && startLessons) {
                endLessons = true;
            }

            //  windows of group plan
            if (schedules[I][J].state == busyLesson && endLessons) {
                return -1;
            }

            if (schedules[I][J].state == busyLesson && startLessons) {
                countLessons++;

                if (countLessons > MAX_OCCUP_OF_GROUP) {
                    return -1;
                }
            }

            int teacherLocation = findTeacher(schedules, schedules[I][J].teacher, J);
            if (teacherLocation >= 2)
                return -1;

            int locationState = findLocation(schedules, schedules[I][J].location, J);
            if (locationState >= 2) {
                return -1;
            }
        }

        for (int J = 0; J < TEACHER_COUNT; J++) {
            int cur_teacher = J;
            bool state = false;
            int count = 0;
            for (int K = 0; K < MAX_OCCUP_IN_DAY; K++) {
                int res = findTeacher(schedules, cur_teacher, K);
                if (res == 1)
                    count++;

                if (res == 1 && !state) {
                    state == true;
                }

                if (res == 1 && state) {
                    res++;
                }

                if (res > MAX_OCCUP_OF_TEACH)
                    return -1;
            }
        }

    
    }

    return res;
}

struct solution direct(struct solution current, int group, int lesson) 
{
    struct solution bad;
    bad.resolve = false;
    int state = verifyStep(current.schedules);
    if (state == -1) 
    {
        
        return bad;
    }

    if (state = 0) {
        state = verifyResolve(current.schedules);
        if (state >= 0) 
        {
            current.resolve = true;
            return current;
        }
        return bad;
    }

    struct solution optim = current;
    int weight = __INT_MAX__;

    // перебор вариантов
    for (int D = -1; D < DISCIP_COUNT; D++) 
    {
        for (int L = 0; L< LOCATION_COUNT; L++) 
        {
            for (int T = 0; T< TEACHER_COUNT; T++) 
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
                if (lesson < MAX_OCCUP_IN_DAY) {
                    next_lesson++;
                    less = true;
                } else if (group < GROUP_COUNT){
                    next_group++;
                    next_lesson = 0;
                    less = true;
                } else if (next_lesson == GROUP_COUNT - 1 && next_lesson == MAX_OCCUP_IN_DAY - 1) {
                    less = true;
                }

                if (less) {
                    struct solution temp = direct(current, next_group, next_lesson);
                    if (temp.resolve) 
                    {
                        int curWeight = 10;
                        if (optim.resolve == false || ((optim.resolve == true) && (curWeight < weight))) 
                        {
                            optim = temp;
                            weight = curWeight;
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
    for (int I = 0; I < GROUP_COUNT; I++) {
        for (int J = 0; J < MAX_OCCUP_IN_DAY; J++) {
            start.schedules[I][J].state = freeLesson;
            start.schedules[I][J].discipline = -1;
            start.schedules[I][J].location = -1;
            start.schedules[I][J].teacher = -1;
        }
    }

    struct solution end = direct(start, 0, 0);

    return;
}

void main()
{
    init();
    directInit();
    return;
}