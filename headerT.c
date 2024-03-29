#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include "headerT.h"

extern int sid;
extern int sid_neu;

extern char buf[MAX_SIZE];

extern struct group g_info, g_itti, g_math;

extern int god_mode;

void remove_newline(char * s)
/* entfernt den hintersten Zeilenumbruch */
{
	int i;
	for (i = strlen(s); i > 0; i--) {
		if (s[i] == '\n') {
			s[i] = '\0';
			return;
		}
	}
}

void auth_user()
/* Das Protokoll lautet wie folgt:
 * 1. Client sendet das Zauberwort "auth\n"
 *		(wird schon in receive_command erledigt)
 * 2. Client sendet den Benutzernamen
 * 3. Server überprüft Benutzernamen
 * 4. Client sendet das Passwort
 * 5. Server überprüft Passwort
 * 6. stimmt beides, setzt der Server den god mode :)
 */
{
	char admin_name[] = "admin";
	char admin_pw[] = "pony";
	char user_buf[MAX_USERNAME], pw_buf[_PASSWORD_LEN];
	int user_ok = 0, pw_ok = 0;
	if (receive(user_buf, MAX_USERNAME)) { // 2.
		remove_newline(user_buf);
		printf("bekam: %s\n", user_buf);
		if (!strcmp(admin_name, user_buf)) { // 3.
			user_ok = 1;
		}
	}
	if (receive(pw_buf, _PASSWORD_LEN)) { // 4.
		printf("bekam: %s\n", pw_buf);
		if (!strcmp(admin_pw, pw_buf)) { // 5.
			pw_ok = 1;
		}
	}
	if (user_ok && pw_ok) { // 6.
		god_mode = 1;
		strcpy(buf, "god mode granted.\n");
		sending(buf, strlen(buf));
	} else {
		strcpy(buf, "hipster mode granted.\n");
		sending(buf, strlen(buf));

		close(sid_neu);
		exit(0);
	}
	return;
}

//legt studenten an, Name, Geburtsdatum, StudentID und Noten werden eingegeben
//bei return ist der Student an den server gesendet
//sendet den übergebenen namen des studenten an server
//im main programm muss dann gleich die receive_student_name funktion aufgerufen werden
void send_student_name(char *name) {
	int len=strlen(name)+1;
	sending(name, len);
}
//sendet den übergebenen vornamen des studenten an den server - zugehörige receive fkt aufrufen
void send_student_firstname(char *firstname) {
	int len=strlen(firstname)+1;
	sending(firstname, len);
}
//sendet übergebenes gebdatum an server - zugehörige receive fkt aufrufen
void send_student_dateofbirth(char *dateofbirth) {
	int len=strlen(dateofbirth)+1;
	sending(dateofbirth, len);
}
//sendet übergebene id an server - zugehörige receive fkt aufrufen
void send_student_id(int student_id) {
	char st_id[15];
	int len=sprintf(st_id, "%d", student_id)+1;
	sending(st_id, len);
}
void send_student_course_marks(int c_ms[]) {
	// 100 noten max in einem studiengang möglich,
	// gespeichert als zahl aus zwei ziffern, leerzeichen dazwischen
	char all_marks[300]; 
	int j=0;
	while (c_ms[j]>0 && j<MAX_MARK) {
	//ende der notenliste durch negative zahl
		//die schleife speichert die notenliste in einem char:
		sprintf(all_marks[j*3], "%d", c_ms[j]);
		all_marks[(j+1) * 3 - 1] = ' ';
		j++;
	}
	all_marks[j*3-1]= '\0';
	int len=strlen(all_marks)+1;
	sending(all_marks, len);
}
//sendet Daten zwischen client und server
//die zu sendenden Daten sind im *buf
//len ist die Länge des bufs
//send ruft die Socketfunktion Send auf
//bei return 0 sind die Daten nicht korrekt gesendet worden, bei return
//1 wurden sie gesendet
int sending(const char *buf,int len) {
	int x,y;
	//send gibt die menge der gesendeten daten zurück:
	x = send(sid_neu, buf, len, 0);
	if (x < 0) { //-1 ist rückgabewert bei fehler
		return 0;
	}
	return 1;
}
//empfängt Daten auf server bzw client
//die zu empfangenden daten sind im *buf
//ruft die socketfunktion recv auf
int receive(const char *buf, int len) { //funktionsweise wie sending
	int x, y;
	x = recv(sid_neu, buf, len, 0);

	if (x < 0) {
		perror("Daten konnten von receive() nicht empfangen werden");
	}
	return 1;
}

//es folgen die zugehörigen receive funktionen zu den gesendeten daten
//dafür muss ein pointer auf einen in der main fkt angelegten studenten übergeben werden gegebenenfalls auch einer auf eine gruppe
//der verwendete buf ist ebenfalls in der main fkt anzulegen und mit ausreichend speicher zu versehen
//die receive funktionen für namen und vornamen  bzw für dateofbirth und id sind an sich gleich, wegen der
//übersicht in der main fkt haben wir aber für jedes receive extra eine fkt zu schreiben statt receive_string
//beispielsweise

void receive_student() {
	struct student *st;
	st = (struct student *) malloc(sizeof(struct student));
	bzero(buf, MAX_SIZE);
	char msg[30];
	strcpy(msg, "Name: "); sending(msg, strlen(msg));
	receive_student_name(st);
	strcpy(msg, "Vorname: "); sending(msg, strlen(msg));
	receive_student_firstname(st);
	strcpy(msg, "Geburtsdatum: "); sending(msg, strlen(msg));
	receive_student_dateofbirth(st);
	strcpy(msg, "ID: "); sending(msg, strlen(msg));
	receive_student_id(st);
	strcpy(msg, "Noten: "); sending(msg, strlen(msg));
	receive_student_course_marks(st);
	strcpy(msg, "Studiengang: "); sending(msg, strlen(msg));
	receive_student_program(st);
	match_to_group(st);
}

void receive_student_name(struct student *s) {
	int e = receive(buf, MAX_NAME);
	remove_newline(buf);
	strcpy(s->name, buf);
}

void receive_student_firstname(struct student *s) {
	int e = receive(buf, MAX_FNAME);
	remove_newline(buf);
	strcpy(s->firstname, buf);
}

void receive_student_dateofbirth(struct student *s) {
	int e = receive(buf, MAX_DAT+1);
	remove_newline(buf);
	strcpy(s->dateofbirth, buf);
}

void receive_student_id(struct student *s) {
	int e = receive(buf, MAX_DAT);
	remove_newline(buf);
	s->student_id=atoi(buf);
}

void receive_student_course_marks (struct student *s) {
	int e = receive(buf, MAX_SIZE);
	remove_newline(buf);
	float gpa=0;
	if (buf[0]!='\0') {
		int i=0,	// Position im course_marks array
			j=0;	// Position im Buffer
		while (j < strlen(buf)) {
			// atoi ignoriert alle whitespaces am anfang und liefert uns
			// die erste gefundene zahl im string:
			s->course_marks[i++] = atoi(buf);
			// diese gefundene zahl wird jetzt durch leerzeichen
			// überschrieben
			// (bitte oh bitte benutzer: halte dich an das format!)
			for (; j < 3*i-1; j++) {
				buf[j] = ' ';
			}
		}

		// alle noten aufsummieren (aber nur solange noch noten
		// vorhanden sind)
		// get it?
		for (i = 0; i < MAX_MARK; i++) {
			gpa += s->course_marks[i];
		}

		// anzahl elemente in s->course_marks[] bestimmen
		// es wird eins zu viel bestimmt, aber das stört uns nur einmal
		// und kommt uns zweimal zu gute
		i = 0;
		while (s->course_marks[i++]) { }
		// und den durschschnitt ausrechnen:
		s->gpa = (gpa/(i-1))/10;

		// das ende der Notenliste wird durch -1 markiert
		// (eigentlich könnte es jede negative zahl sein)
		if (i<MAX_MARK) {
			s->course_marks[i] = -1;
		}
	}
}

void receive_student_program(struct student *s) {
	int e = receive(buf, MAX_PRO);
	remove_newline(buf);
	strcpy(s->program, buf);
}

//ordnet den übermittelten Studenten einer gruppe zu
//eingegeben wird der aktuelle student - muss in der main funktion direkt nach dem receive der ganzen daten erfolgen
//bei return 0 wurde der student in die zugehörige gruppendatei gelesen
//bei return 1 gab es keine passende gruppe (wir haben ja nur 3: ITTI, INFO, MATH)
int match_to_group(struct student *st) {
	FILE * datei;
	int j=0;

	if (strcmp(st->program,"ITTI") == 0){
		datei = fopen("ITTI.txt","a+");
		fprintf(datei, "%s %s %s %d %f\n",
				st->name,
				st->firstname,
				st->dateofbirth,
				st->student_id,
				st->gpa
			   );
		fclose(datei);
		while (strcmp(g_itti.student[j].name,"") != 0) { j++; }
		g_itti.student[j] = *st;
		return 0;
	}
	else if (strcmp(st->program,"INFO") == 0) {
		datei = fopen("INFO.txt","a+");
		fprintf(datei, "%s %s %s %d %f\n",
				st->name,
				st->firstname,
				st->dateofbirth,
				st->student_id,
				st->gpa
			   );
		fclose(datei);
		while (strcmp(g_info.student[j].name,"") != 0) { j++; }
		g_info.student[j] = *st;
		return 0;
	}
	else if (strcmp(st->program,"MATH") == 0) {
		datei = fopen ("MATH.txt","a+");
		fprintf(datei, "%s %s %s %d %f\n",
				st->name,
				st->firstname,
				st->dateofbirth,
				st->student_id,
				st->gpa
			   );
		fclose(datei);
		while (strcmp(g_math.student[j].name,"") != 0) { j++; }
		g_math.student[j] = *st;
		return 0;
	} else {
		return 1;
	}
}

//clientfunktion, sendet anfrage nach topstudenten an server
//ACHTUNG: in der main fuktion muss wirklich "anfrage" gesendet werden
//als char
void request_top() {
	sending("anfrage", 8);
	return;
}

//empfängt anfrage nach topstudenten auf dem server
//ACHTUNG: in der main funktion muss wirklich "anfrage" gesendet werden als char
void work_request_top() {
	char anfrage[MAX_SIZE];
	receive(anfrage,MAX_SIZE);
	if (strcmp(anfrage,"anfrage")==0) {
		struct student s1;
		struct student s2;
		struct student s3;
		struct student best;
		s1=get_best_group_gpa(g_itti);
		s2=get_best_group_gpa(g_info);
		s3=get_best_group_gpa(g_math);
		best=get_best_at_all(s1, s2, s3);
		send_topstudents(s1, s2, s3, best);
	}
}


//sucht den besten einer gruppe raus
//muss in der mainfunktion gespeichert werden und dann an die send_topstudents funktion übergeben werden
struct student get_best_group_gpa(struct group group) {
	int k;
	float x;
	float best_gpa=10;
	struct student best_student;
	int j=0;

	while(group.student[j].name) {
		k=j;
		j++;
	}
	for (j=0; j<k+1; j++) {
		x=group.student[j].gpa;
		if (x<best_gpa) {
			best_gpa=x;
			best_student=group.student[j];
		}
	}
	return best_student;
}

//findet den besten studenten aller gruppen
//gibt diesen aus - muss in der mainfunktion gespeichert werden und dann an die send_topstudents funktion übergeben werden
struct student get_best_at_all(struct student st1, struct student st2, struct student st3) {
	float x;
	float y;
	float z;
	struct student overall_best=st1;
	struct student test1=st2;
	struct student test2=st3;
	x=test1.gpa;
	y=overall_best.gpa;
	z=test2.gpa;
	if (x<y) {
		overall_best=test1;
	}
	if(z<x) {
		overall_best=test2;
	}
	return overall_best;
}

//sendet topstudenten und den gesamtbesten an client
void send_topstudents(struct student group1, struct student group2, struct student group3, struct student at_all) {
	int i, len, id;
	char name;
	char firstname;
	char dateofbirth;
	float gpa;
	char gpa_char[3];
	char all_marks[300]; 
	char st_id[15];
	int j=0;

	//besten studenten aus gruppe 1 senden
		strcpy(name, group1.name);
		strcpy(firstname, group1.firstname);
		strcpy(dateofbirth, group1.dateofbirth);
		id=group1.student_id;
		gpa=group1.gpa;
		while (group1.course_marks[j]>0 && j<MAX_MARK) {
			sprintf(all_marks[j*3], "%d", group1.course_marks[j]);
			all_marks[(j+1) * 3 - 1] = ' ';
			j++;
		}
		all_marks[j*3-1]= '\0';
		
		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);
		
		len=strlen(dateofbirth)+1;
		sending(dateofbirth, len);

		len=sprintf(st_id, "%d", id)+1;
		sending(st_id, len);

		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);
	
		len=strlen(all_marks)+1;
		sending(all_marks, len);

	//besten studenten aus gruppe 2 senden
		strcpy(name, group2.name);
		strcpy(firstname, group2.firstname);
		strcpy(dateofbirth, group2.dateofbirth);
		id=group2.student_id;
		gpa=group2.gpa;
		j=0;
		while (group2.course_marks[j]>0 && j<MAX_MARK) {
			sprintf(all_marks[j*3], "%d", group2.course_marks[j]);
			all_marks[(j+1) * 3 - 1] = ' ';
			j++;
		}
		all_marks[j*3-1]= '\0';
		
		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);
		
		len=strlen(dateofbirth)+1;
		sending(dateofbirth, len);

		len=sprintf(st_id, "%d", id)+1;
		sending(st_id, len);

		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);
	
		len=strlen(all_marks)+1;
		sending(all_marks, len);
	//besten studenten aus gruppe 3 senden
		strcpy(name, group3.name);
		strcpy(firstname, group3.firstname);
		strcpy(dateofbirth, group3.dateofbirth);
		id=group3.student_id;
		gpa=group3.gpa;
		j=0;
		while (group3.course_marks[j]>0 && j<MAX_MARK) {
			sprintf(all_marks[j*3], "%d", group3.course_marks[j]);
			all_marks[(j+1) * 3 - 1] = ' ';
			j++;
		}
		all_marks[j*3-1]= '\0';
		
		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);
		
		len=strlen(dateofbirth)+1;
		sending(dateofbirth, len);

		len=sprintf(st_id, "%d", id)+1;
		sending(st_id, len);

		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);
	
		len=strlen(all_marks)+1;
		sending(all_marks, len);

	//besten studenten von allen senden
		strcpy(name, at_all.name);
		strcpy(firstname, at_all.firstname);
		strcpy(dateofbirth, at_all.dateofbirth);
		id=at_all.student_id;
		gpa=at_all.gpa;
		j=0;
		while (at_all.course_marks[j]>0 && j<MAX_MARK) {
			sprintf(all_marks[j*3], "%d", at_all.course_marks[j]);
			all_marks[(j+1) * 3 - 1] = ' ';
			j++;
		}
		all_marks[j*3-1]= '\0';
		
		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);
		
		len=strlen(dateofbirth)+1;
		sending(dateofbirth, len);

		len=sprintf(st_id, "%d", id)+1;
		sending(st_id, len);

		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);
	
		len=strlen(all_marks)+1;
		sending(all_marks, len);
}
