#define MAX_MARK 100			//max Notenanzahl
#define MAX_FNAME 20			//max länge des vornamens
#define MAX_NAME 30			//max länge des nachnamens
#define MAX_SIZE 500
#define MAX_DAT 10
#define MAX_PRO 5
int sid;

struct student {
	char name[MAX_NAME+1];
	char firstname[MAX_FNAME+1];
	char dateofbirth[MAX_DAT+1];
	int student_id;
	int course_marks[MAX_MARK];
	float gpa;
	char program[MAX_PRO];
}

struct group {
	char groupname[MAX_NAME];
	struct student student[10];
	struct group group*; 
}

int sending(const char *buf,int len);	//funktion zum senden der daten zwischen server und client
int receive(const char *buf,int len);	//funktion zum empfangen von daten zwischen server und client
