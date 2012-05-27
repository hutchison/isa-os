#ifndef _headerT_h_
#define _headerT_h_

#include <string.h>

#define MAX_MARK 100	//max Notenanzahl
#define MAX_FNAME 20	//max länge des vornamens
#define MAX_NAME 30	//max länge des nachnamens
#define MAX_SIZE 500
#define MAX_DAT 10
#define MAX_PRO 5

int sid, sid_neu;
char buf[MAX_SIZE];

struct student {
	char name[MAX_NAME+1];
	char firstname[MAX_FNAME+1];
	char dateofbirth[MAX_DAT+1];
	int student_id;
	int course_marks[MAX_MARK];
	float gpa;
	char program[MAX_PRO];
};

struct group {
	char groupname[MAX_NAME];
	struct student student[10];
	struct group *group;
} g_info, g_itti, g_math;

//funktion zum senden der daten zwischen server und client:
int sending(const char *buf,int len);
//funktion zum empfangen von daten zwischen server und client:
int receive(const char *buf,int len);
void send_student_name(char *name);
void send_student_firstname(char *firstname);
void send_student_dateofbirth(char *dateofbirth);
void send_student_id(int student_id);
void send_student_course_marks(int course_marks[]);
void receive_student_name(struct student *s);
void receive_student_firstname(struct student *s);
void receive_student_dateofbirth(struct student *s);
void receive_student_id(struct student *s);
void receive_student_course_marks (struct student *s);
void receive_student_program(struct student *s);
void request_top();
void receive_request_top();
void send_topstudents(struct student group1, struct student group2, struct student group3, struct student at_all);
int match_to_group(struct student *st);
//struct student get_best_group_gpa (struct group group);
struct student get_best_at_all (struct student student1, struct student student2, struct student student3);
void send_hello();

#endif
