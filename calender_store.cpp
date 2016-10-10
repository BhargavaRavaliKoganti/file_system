#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <conio.h>
#include <string.h>
#include <time.h>

struct date{
	int day;
	int month;
	int year;
};

void options(FILE *);
void category_options(FILE *, int);
void register_into_category(FILE *);
void view_details(FILE *);
void appointment(FILE *, int);
int date_validation(struct date, struct date);
int is_valid_date(struct date);
int six_month_date_validation(struct date);

int main(){
	long int free_space;
	int offset = 0;
	FILE *fp;
	fp = fopen("calender_store.bin", "rb+");
	char categories[10][28] = { "doctors", "technicians", "plumbers", "electricians", "teachers", "drivers", "chefs", "security", "servants", "managers" };
	fseek(fp, 0, SEEK_SET);
	fread(&free_space, 4, 1, fp);
	if (!free_space){
		fseek(fp, -4, SEEK_CUR);
		free_space = 4 + 32 * 10;
		fwrite(&free_space, 4, 1, fp);
		for (int i = 0; i < 10; i++){
			fwrite(categories[i], 28, 1, fp);
			fwrite(&offset, 4, 1, fp);
		}
	}
	options(fp);
	fclose(fp);
	return 0;
}

void options(FILE *fp){
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4, SEEK_CUR);
	char *cateogry = (char *)malloc(sizeof(char) * 28);
	int choice;
	int category;
	while (1) {
		printf("\n-------------------------------------CATEGORIES---------------------------------\n");
		for (int i = 0; i < 10; i++){
			fread(cateogry, 28, 1, fp);
			fseek(fp, 4, SEEK_CUR);
			printf("%d. %s\n", i + 1, cateogry);
		}
		printf("\n--------------------------------------------------------------------------------\n");
		printf("1. To select category\n2. exit\nEnter choice:\n");
		scanf("%d", &choice);
		switch (choice)	{
		case 1:
			printf("Select a category:\n");
			scanf("%d", &category);
			category_options(fp, category);
			break;
		default:
			exit(0);
			break;
		}
	}
}

void category_options(FILE *fp, int category){
	fseek(fp, 0, SEEK_SET);
	fseek(fp, (category - 1) * 32 + 4, SEEK_CUR);
	int choice;
	while (1){
		printf("\n--------------------------------------------------------------------------------\n");
		printf("1. Register\n2. Book appointment\n3. back\nEnter choice:\n");
		scanf("%d", &choice);
		switch (choice)	{
		case 1:
			register_into_category(fp);
			break;
		case 2:
			view_details(fp);
			break;
		default:
			return;
			break;
		}
	}
}

void register_into_category(FILE *fp){
	long int free_space;
	long int category_offset;
	long int offset = 0;
	char *name = (char *)malloc(sizeof(char) * 28);
	char *role = (char *)malloc(sizeof(char) * 28);
	char *mobile = (char *)malloc(sizeof(char) * 11);
	category_offset = ftell(fp);
	printf("%d category\n", category_offset);
	fseek(fp, 0, SEEK_SET);
	fread(&free_space, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, free_space, SEEK_CUR);
	printf("Enter Name:\n");
	scanf("%s", name);
	fwrite(name, 28, 1, fp);
	printf("Enter Role:\n");
	scanf("%s", role);
	fwrite(role, 28, 1, fp);
	printf("Enter phone number:\n");
	scanf("%s", mobile);
	fwrite(mobile, 11, 1, fp);
	fwrite(&offset, 4, 1, fp);
	fwrite(&offset, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, category_offset+28, SEEK_CUR);
	fread(&offset, 4, 1, fp);
	if (!offset){
		fseek(fp, -4, SEEK_CUR);
		printf("%d\n", ftell(fp));
		fwrite(&free_space, 4, 1, fp);
		free_space = free_space + 28 + 28 + 11 + 4 + 4;
	}
	else{
		while (offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, offset + 28 + 28 + 11, SEEK_CUR);
			fread(&offset, 4, 1, fp);
		}
		fseek(fp, -4, SEEK_CUR);
		fwrite(&free_space, 4, 1, fp);
		free_space = free_space + 28 + 28 + 11 + 4 + 4;
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(&free_space, 4, 1, fp);
	printf("Successfully registerd\n--------------------------------------------------------------------------------\n");
	fseek(fp, 0, SEEK_SET);
	fseek(fp, category_offset, SEEK_CUR);
}

void view_details(FILE *fp){
	long int category_offset = ftell(fp);
	long int offset = ftell(fp) + 28;
	int count = 0;
	char *name = (char *)malloc(sizeof(char) * 28);
	char *role = (char *)malloc(sizeof(char) * 28);
	char *mobile = (char *)malloc(sizeof(char) * 11);
	int choice;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, offset, SEEK_CUR);
	fread(&offset, 4, 1, fp);
	if (offset){
		printf("\n--------------------------------------------------------------------------------\n");
		while (offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, offset, SEEK_CUR);
			fread(name, 28, 1, fp);
			fread(role, 28, 1, fp);
			fread(mobile, 11, 1, fp);
			printf("%s", mobile);
			count++;
			printf("%d. %s\t%s\t%s\n", count, name, role, mobile);
			fread(&offset, 4, 1, fp);
		}
		printf("Enter choice:\n");
		scanf("%d", &choice);
		fseek(fp, 0, SEEK_SET);
		fseek(fp, category_offset, SEEK_CUR);
		appointment(fp, choice);
	}
	else{
		printf("No persons to take appointment\n");
	}
	fseek(fp, 0, SEEK_SET);
	fseek(fp, category_offset, SEEK_CUR);
}

void appointment(FILE *fp, int choice){
	long int category_offset = ftell(fp);
	long int offset;
	long int free_space;
	int count = 0;
	long int client_offset;
	char *name = (char *)malloc(sizeof(char) * 28);
	struct date appointment_date;
	struct date temp_date;
	long int next_client_offset = 0;
	printf("Enter name:\n");
	scanf("%s", name);
	printf("Enter date of appointment(dd/mm/yyyy):\n");
	scanf("%d/%d/%d", &appointment_date.day, &appointment_date.month, &appointment_date.year);
	if (!is_valid_date(appointment_date)){
		printf("Enter valid date\n");
		return;
	}
	if (!six_month_date_validation(appointment_date)){
		printf("Can take appointment for the next six months only\n");
		return;
	}
	fseek(fp, 28, SEEK_CUR);
	fread(&offset, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fread(&free_space, 4, 1, fp);
	while (offset){
		fseek(fp, 0, SEEK_SET);
		fseek(fp, offset, SEEK_CUR);
		count++;
		if (count == choice){
			break;
		}
		fseek(fp, 28 + 28 + 11, SEEK_CUR);
		fread(&offset, 4, 1, fp);
	}
	printf("%d\n", ftell(fp));
	fseek(fp, 28 + 28 + 11 + 4, SEEK_CUR);
	fread(&client_offset, 4, 1, fp);
	if (!client_offset){
		fseek(fp, -4, SEEK_CUR);
		fwrite(&free_space, 4, 1, fp);
		fseek(fp, 0, SEEK_SET);
		fseek(fp, free_space, SEEK_CUR);
		fwrite(name, 28, 1, fp);
		fwrite(&appointment_date, 4 + 4 + 4, 1, fp);
		fwrite(&next_client_offset, 4, 1, fp);
		free_space = free_space + 28 + 4 + 4 + 4 + 4;
		fseek(fp, 0, SEEK_SET);
		fwrite(&free_space, 4, 1, fp);
	}
	else{
		while (client_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, client_offset + 28, SEEK_CUR);
			fread(&temp_date, 4 + 4 + 4, 1, fp);
			if (!date_validation(appointment_date, temp_date)){
				printf("Entered date is already booked\n");
				return;
			}
			fread(&client_offset, 4, 1, fp);
		}
		fseek(fp, -4, SEEK_CUR);
		fwrite(&free_space, 4, 1, fp);
		fseek(fp, 0, SEEK_SET);
		fseek(fp, free_space, SEEK_CUR);
		fwrite(name, 28, 1, fp);
		fwrite(&appointment_date, 4 + 4 + 4, 1, fp);
		fwrite(&next_client_offset, 4, 1, fp);
		free_space = free_space + 28 + 4 + 4 + 4 + 4;
		fseek(fp, 0, SEEK_SET);
		fwrite(&free_space, 4, 1, fp);
	}

	fseek(fp, 0, SEEK_SET);
	fseek(fp, category_offset, SEEK_CUR);
}

int date_validation(struct date d1, struct date d2){
	if (d1.day == d2.day && d1.month == d2.month && d1.year == d2.year)
		return 0;
	return 1;
}

int is_valid_date(struct date d){
	int dd = d.day;
	int mm = d.month;
	int yy = d.year;
	if (yy >= 1900 && yy <= 9999){
		if (mm >= 1 && mm <= 12){
			if ((dd >= 1 && dd <= 31) && (mm == 1 || mm == 3 || mm == 5 || mm == 7 || mm == 8 || mm == 10 || mm == 12))
				return 1;
			else if ((dd >= 1 && dd <= 30) && (mm == 4 || mm == 6 || mm == 9 || mm == 11))
				return 1;
			else if ((dd >= 1 && dd <= 28) && (mm == 2))
				return 1;
			else if (dd == 29 && mm == 2 && (yy % 400 == 0 || (yy % 4 == 0 && yy % 100 != 0)))
				return 1;
			else
				return 0;
		}
		else{
			return 0;
		}
	}
	else{
		return 0;
	}
}

int six_month_date_validation(struct date d){
	struct tm tm;
	time_t x;
	long given_date;
	long system_date;
	tm.tm_mday = d.day;
	tm.tm_mon = d.month-1;
	tm.tm_year = d.year - 1900;
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_isdst = 0;
	x = mktime(&tm);
	printf("seconds since the Epoch: %ld\n", (long)x);
	given_date = (long)x;
	time_t rawtime;
	rawtime = time(NULL);
	struct tm *timeinfo = localtime(&rawtime);
	time_t y;
	y = mktime(timeinfo);
	printf("seconds since the Epoch: %ld\n", (long)y);
	system_date = (long)y;
	if (given_date - system_date < 0 || given_date - system_date >(3 * 30 + 3 * 31) * 24 * 3600)
		return 0;
	return 1;
}