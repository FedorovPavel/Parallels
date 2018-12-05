#include <stdio.h>
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

struct group *groups;
int *disciplines;
struct teacher *teachers;
int *locations;

void destroyInput() {
    free(groups);
    free(disciplines);
    free(teachers);
    free(locations);
}

void initInput()
{
    groups = (struct group *)calloc(GROUP_COUNT, sizeof(struct group));
    if (!groups) {
        printf("Fail to init groups");
        exit(-1);
    }
    for (int I = 0; I < GROUP_COUNT; I++) {
        groups[I].name = I;
        groups[I].plans = (struct plan *)calloc(DISCIP_COUNT, sizeof(struct plan));
        if (!groups[I].plans) {
            destroyInput();
            printf("Fail to init plan for group %d", I);
            exit(-2);
        }

        for (int J = 0; J < DISCIP_COUNT; I++) {
            groups[I].plans[J].count = 1;
            groups[I].plans[J].discipline = J;
        }
    }

    disciplines = calloc(DISCIP_COUNT, sizeof(int));
    if (!disciplines) {
        printf("Fail to init disciplines");
        destroyInput();
        exit(-3);
    }
    for (int I = 0; I < DISCIP_COUNT; I++) {
        disciplines[I] = I;
    }

    teachers = (struct teacher*)calloc(TEACHER_COUNT, sizeof(struct teacher));
    if (!teachers) {
        printf("Fail to init teachers");
        destroyInput();
        exit(-4);
    }
    for (int I = 0; I < TEACHER_COUNT; I++) {
        teachers[I].name = I;
        teachers[I].employment = (struct teacherEmployment *)calloc(DISCIP_COUNT, sizeof(struct teacherEmployment));
        if (!teachers[I].employment) {
            printf("Fail to init teacher %d employment", I);
            destroyInput();
            exit(-4);
        }
        for (int J = 0; J < DISCIP_COUNT; J++) {
            teachers
        }
    }

    locations = calloc(LOCATION_COUNT, sizeof(int));
    if (!locations) {
        printf("Fail to init locations");
        destroyInput();
        exit(-5);
    }
    for (int I = 0; I < LOCATION_COUNT; I++) {
        locations[I] = I;
    }

    plan = (struct line *)calloc(DISCIP_COUNT, sizeof(struct line));
    if (!plan) {
        printf("Fail to init plan");
        destroyInput();
        exit(-6);
    }
    for (int I = 0; I < DISCIP_COUNT; I++) {
        plan[I].count = 1;
        plan[I].discipline = I;
    }

    // teacherLoads = (struct load *)calloc()

    printf("Success init input data");
    return;
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

        }
    }
}

void direct()
{
    struct scheduleLine **schedules = (struct scheduleLine **)calloc(sizeof(struct scheduleLine *), GROUP_COUNT);
    if (!schedules) {
        printf("Fail to init schedules");
        destroyInput();
        exit(-7);
    }
    for (int I = 0; I < GROUP_COUNT; I++) {
        schedules[I] = (struct scheduleLine *)calloc(sizeof(struct scheduleLine), MAX_OCCUP_IN_DAY);
        if (!schedules[I]) {
            printf("Fail to init schedules by %d group", I);
            destroyInput();
            exit(-8);
        }
    }

    printf("Success create schedules");



    printf("Free scheduls");
    for (int I = 0; I < GROUP_COUNT; I++) {
        free(schedules[I]);
    }

    free(schedules);
    return;
}

void main()
{
    initInput();
    direct();
    destroyInput();
    return;
}