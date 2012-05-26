


//legt studenten an, Name, Geburtsdatum, StudentID und Noten werden eingegeben
//bei return ist der Student an den server gesendet

void send_student_name(char *name){

	int len=strlen(name)+1;
	sending(name, len);
}
void send_student_firstname(char *firstname){
	len=strlen(firstname)+1;
	sending(firstname, len);
}
void send_student_dateofbirth(char *dateofbirth){
	len=strlen(dateofbirth)+1;
	sending(dateofbirth, len);
}
void send_student_id(int student_id){
	char st_id[15];
	len=sprintf(st_id, "%d", student_id)+1;
	sending(st_id, len);
}
void send_student_course_marks(int course_marks[]) {
	char all_marks[300];					//100 noten max in einem studiengang möglich, gespeichert als zahl aus zwei ziffern, leerzeichen dazwischen
	int j=0;
	while (c_ms[j]<0 && j<MAX_MARK) {		//ende der notenliste durch negative zahl
		sprintf(all_marks[j*3], "%d", c_ms[j]);	//die schleife speichert die liste der noten in einem char
		all_marks[(j+1) * 3 - 1] = ' ';
		j++;
	}
	all_marks[j*3-1]= '\0';
	len=strlen(all_marks)+1; 	
	sending(c_ms, len);
}
//sendet Daten zwischen client und server
//die zu sendenden Daten sind im *buf
//len ist die Länge des bufs
//send ruft die Socketfunktion Send auf
//bei return 0 sind die Daten nicht korrekt gesendet worden, bei return 1 wurden sie gesendet
int sending(const char *buf,int len) {
	int x,y;
	x=send(sid, buf, len, 0);		//send gibt die menge der gesendeten daten zurück
	if (x==-1) {					//-1 ist rückgabewert bei fehler
		return 0;
	}
	while (x<len) {					//bei x<len wurden nicht alle datenn gesendet
									//neuer sendeversuch ab dem fehler
		y=send(sid, buf[x], len-x, 0);
		x=x+y;

		if (y==-1) {				//erneutes testen auf fehler
		return 0;
		}	
	}
	return 1;
}
//empfängt Daten auf server bzw client
//die zu empfangenden daten sind im *buf
//ruft die socketfunktion recv auf
int receive(char *buf,int len) {			//funktionsweise wie sending
	int x,y;
	x=recv(sid, buf, len, 0);

	if (x==-1) {
		printf("Die Daten konnten nicht empfangen werden");
		return x;
	}
	while (x<len) {
		printf("Die Daten wurden nicht vollständig empfangen");
		y=recv(sid, buf[x], len-x, 0);
		x=x+y;

		if(y==-1) {
			printf("Es ist ein Fehler aufgetreten");
			return y;
		}
	}
	return 1;
}

//empfängt und speichert studenten auf server
//bei return wurde der student erfolgreich gespeichert

void receive_student_name(struct student *s) {
	int e = receive(buf, MAX_NAME);
	strcpy(s->name, buf);
	/*	
	for(i=0; i!='\0', i<=MAX_NAME;i++) {
		s.name[i]=buf[i];
	}
	*/
 }

void receive_student_firstname(struct student *s) {
	int e = receive(buf, MAX_FNAME);
	strcpy(s->firstname, buf);
}


void receive_student_dateofbirth(struct student *s) {
	int e = receive(buf, MAX_DAT);
	strcpy(s->dateofbirth, buf);
}

void receive_student_id struct student *s) {
	int e = receive(buf, MAX_DAT);
	s->student_id=atoi(buf);
}

void receive_student_course_marks (struct student *s) {
	int e=receive(buf, MAX_SIZE);
	float gpa;
	if (buf[0]!='\0') {

		for (i==0) {
			s->course_marks[i]=atoi(buffer[i*3]);
			gpa=gpa+s->course_marks[i];
			i++;
		}

		for(i=1;buf[i*3-1]!='\0', i<=MAX_SIZE; i++) {
			s->course_marks[i]=atoi(buf[i*3]);
			gpa=gpa+s->course_marks[i];
		}
		gpa=(gpa/i)/10;	
		s->gpa=gpa;		//nachkommastellen evtl noch verändern - jetzt ist es ein float mit xx nachkommastellen
		if (i<MAX_MARK) {
			s->course_marks[i]=-1;
		}
	}
}

void receive_student_program(struct group *g, struct student *s) {
	int j=0;
	int e = receive(buf, MAX_PRO);
	strcpy(g->groupname, buf);
	while (g->student[j]!='\0') {
		j++;
	}
	g->student[j]=s;
	strcpy(s->program,g->groupname);
}


//ordnet den übermittelten Studenten einer gruppe zu
//eingegeben wird der aktuelle student - muss in der main funktion direkt nach dem receive der ganzen daten erfolgen
//bei return 0 wurde der student in die zugehörige gruppendatei gelesen
//bei return 1 gab es keine passende gruppe
int match_to_group(struct student *st) {
	char n[MAX_NAME]; f, d, id, liste;
	char f[MAX_FNAME];
	char d[MAX_DAT];
	int id;
	char program[MAX_PRO];
	float gpa;

	n=st->name;
	f=st->firstname;
	d=st->dateofbirth;
	id=st->student_id;
	gpa=st->gpa;
	studiengang=st->program;

		if (strcmp(studiengang,"ITTI")==0){
			datei = fopen ("ITTI.txt","a+");
			fprintf(datei, "%s, %s, %s, %d, %f\n", n, f, d, id, gpa);
			fclose (datei);
			return 0;
		}
		else if (strcmp(studiengang,"INFO")==0) {
			datei = fopen ("INFO.txt","a+");
			fprintf(datei, "%s, %s, %s, %d, %f\n", n, f, d, id, gpa);
			fclose (datei);
			return 0;
		}
		else if (strcmp(studiengang,"MATH")==0) {
			datei = fopen ("MATH.txt","a+");
			fprintf(datei, "%s, %s, %s, %d, %f\n", n, f, d, id, gpa);
			fclose (datei);
			return 0;
		}
		else {
			return 1;
		}
}}

//sendet anfrage nach topstudenten an server
//ACHTUNG: in der main fuktion muss wirklich "anfrage" gesendet werden als char
void request_top(char *anfrage) {
	sending(anfrage, 8);
	return;
}

//empfängt anfrage nach topstudenten auf dem server
//ACHTUNG: in der main funktion muss wirklich "anfrage" gesendet werden als char
void receive_request_top() {
	char anfrage[MAX_SIZE];
	receive(anfrage,MAX_SIZE);
	if (anfrage='anfrage') {
		send_topstudents();
	}

}

//sucht den besten einer gruppe raus
//muss in der mainfunktion gespeichert werden und dann an die send_topstudents funktion übergeben werden
struct student get_best_group_gpa (struct group group) {
	int k;
	float x;
	float best_gpa=10;
	struct student best_student;
	int j=0;

	while(group.student[j]!='\0') {
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
return best.student;
}

//findet den besten studenten aller gruppen
//gibt diesen aus - muss in der mainfunktion gespeichert werden und dann an die send_topstudents funktion übergeben werden
struct student get_best_at_all (struct group group1, struct group group2, struct group group3) {
	float x;
	float y;
	float z;
	struct student overall best=get_best_group_gpa(group1);
	struct student test1=get_best_group_gpa(group2);
	struct student test2=get_best_group_gpa(group3);
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
}}

//sendet topstudenten und den gesamtbesten an client
void send_topstudents(struct student group1, struct student group2, struct student group3, struct student at_all) {
	int i,len;
	char name;
	char firstname;
	float gpa;

	//besten studenten aus gruppe 1 senden
		name=group1.name;
		firstname=group1.firstname;
		gpa=group1.gpa;

		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);

		char gpa_char[3];
		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);

	//besten studenten aus gruppe 2 senden
		name=group2.name;
		firstname=group2.firstname;
		gpa=group2.gpa;

		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);

		char gpa_char[3];
		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);

	//besten studenten aus gruppe 3 senden
		name=group3.name;
		firstname=group3.firstname;
		gpa=group3.gpa;

		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);

		char gpa_char[3];
		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);

	//besten studenten von allen senden
		name=at_all.name;
		firstname=at_all.firstname;
		gpa=at_all.gpa;

		len=strlen(name)+1;
		sending(name, len);

		len=strlen(firstname)+1;
		sending(firstname, len);

		char gpa_char[3];
		len=sprintf(gpa_char, "%f", gpa)+1;
		sending(gpa_char, len);


}
