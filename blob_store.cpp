#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct user{
	char user_name[28];
	long int file_offset;
};

void register_user(FILE *);
void view_files(FILE *);
void add_file(FILE *, char *);
void delete_file(FILE *, char *);
void login_user(FILE *);
void download_file(FILE *, char *);
void file_options(FILE *fp, char *);

int main(){
	FILE *fp;
	fp = fopen("blob_store.bin", "rb+");
	long int free_space;
	int user_count = 0;
	fread(&free_space, 4, 1, fp);
	if (!free_space){
		free_space = 8 + 32 * 20;
		fseek(fp, 0, SEEK_SET);
		fwrite(&free_space, 4, 1, fp);
		fwrite(&user_count, 4, 1, fp);
	}
	int choice;
	while (1){
		printf("1. Login\n2. Register\n3. Exit\n");
		scanf("%d", &choice);
		switch (choice){
		case 1:
			login_user(fp);
			break;
		case 2:
			register_user(fp);
			break;
		default:
			exit(0);
			break;
		}
	}
	fclose(fp);
	return 0;
}


void register_user(FILE *fp){
	int user_count;
	struct user users;
	char username[28];
	printf("\n=================================Register=================================\n");
	printf("Enter user name:\n");
	scanf("%s", username);
	strcpy(users.user_name, username);
	users.file_offset = 0;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4, SEEK_CUR);
	fread(&user_count, 4, 1, fp);
	user_count++;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4, SEEK_CUR);
	fwrite(&user_count, 4, 1, fp);
	fseek(fp, 32 * (user_count - 1), SEEK_CUR);
	fwrite(&users, 32, 1, fp);
	system("cls");
	printf("\n=================================Welcome %s=================================\n", users.user_name);
	file_options(fp, username);
	return;
}

void login_user(FILE *fp){
	char username[28];
	printf("Enter user name:\n");
	fflush(stdin);
	gets(username);
	int user_count;
	struct user users;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4, SEEK_CUR);
	fread(&user_count, 4, 1, fp);
	for (int i = 0; i < user_count; i++){
		fread(&users, 32, 1, fp);
		if (!strcmp(users.user_name, username)){
			system("cls");
			printf("\n=================================Welcome %s=================================\n", users.user_name);
			file_options(fp, username);
		}
	}
}

void file_options(FILE *fp, char *login_user){
	int choice;
	char* file_name = (char*)malloc(sizeof(char) * 28);
	while (1){
		printf("\n1. Add files");
		printf("\n2. View the list of files");
		printf("\n3. Delete a file");
		printf("\n4. Download a file");
		printf("\n5. Logout");
		printf("\nEnter choice : ");
		scanf("%d", &choice);
		printf("--------------------------------------------------------------------------------\n");
		switch (choice){
		case 1:
			printf("Enter file name : \n");
			fflush(stdin);
			gets(file_name);
			add_file(fp, file_name);
			break;
		case 2:
			view_files(fp);
			break;
		case 3:printf("Enter file name : \n");
			fflush(stdin);
			gets(file_name);
			delete_file(fp, file_name);
			break;
		case 4:
			printf("Enter file name : \n");
			fflush(stdin);
			gets(file_name);
			download_file(fp, file_name);
			break;
		case 5:
			return;
		default:printf("\nInvalid choice");
			break;
		}
	}
	printf("LOGOUT\n");
}

void add_file(FILE *fp, char *file_name){
	long int offset_position;
	long int file_offset;
	long int next_file_offset = 0;
	long int free_space;
	int flag = 0;
	char c;
	int file_size = 0;
	fseek(fp, -4, SEEK_CUR);
	offset_position = ftell(fp);
	fread(&file_offset, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fread(&free_space, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, free_space, SEEK_CUR);
	fwrite(file_name, 28, 1, fp);
	fwrite(&next_file_offset, 4, 1, fp);
	FILE *add = fopen(file_name, "r");
	int size = ftell(fp);
	fseek(fp, 4, SEEK_CUR);
	while (!feof(add)){
		c = fgetc(add);
		fwrite(&c, 1, 1, fp);
		file_size++;
	}
	fclose(add);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, size, SEEK_CUR);
	fwrite(&file_size, 4, 1, fp);
	if (!file_offset){
		fseek(fp, 0, SEEK_SET);
		fseek(fp, offset_position, SEEK_CUR);
		fwrite(&free_space, 4, 1, fp);
	}
	else{
		fseek(fp, 0, SEEK_SET);
		fseek(fp, file_offset, SEEK_CUR);
		while (file_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, file_offset, SEEK_CUR);
			fseek(fp, 28, SEEK_CUR);
			fread(&file_offset, 4, 1, fp);
		}
		fseek(fp, -4, SEEK_CUR);
		fwrite(&free_space, 4, 1, fp);
	}
	free_space = size + file_size + 4;
	fseek(fp, 0, SEEK_SET);
	fwrite(&free_space, 4, 1, fp);
	fseek(fp, offset_position + 4, SEEK_SET);
	printf("--------------------------------------------------------------------------------------\n");
}

void view_files(FILE *fp){
	long int offset = ftell(fp);
	long int file_offset;
	int count = 0;
	char* file_name = (char*)malloc(sizeof(char) * 28);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, offset - 4, SEEK_CUR);
	fread(&file_offset, 4, 1, fp);
	if (!file_offset){
		printf("No files to view\n");
		return;
	}
	else{
		while (file_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, file_offset, SEEK_CUR);
			fread(file_name, 28, 1, fp);
			count++;
			printf("%d. %s\n", count, file_name);
			fread(&file_offset, 4, 1, fp);
		}
	}
	fseek(fp, 0, SEEK_SET);
	fseek(fp, offset, SEEK_CUR);
	printf("--------------------------------------------------------------------------------\n");
}

void delete_file(FILE *fp, char *file_name){
	long int offset = ftell(fp);
	long int file_offset;
	int x;
	long int previous_offset = offset - 4;
	int count = 0;
	char* file = (char*)malloc(sizeof(char) * 28);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, offset - 4, SEEK_CUR);
	fread(&file_offset, 4, 1, fp);
	if (!file_offset){
		printf("No files to delete\n");
		return;
	}
	else{
		while (file_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, file_offset, SEEK_CUR);
			x = file_offset;
			fread(file, 28, 1, fp);
			fread(&file_offset, 4, 1, fp);
			if (!strcmp(file, file_name)){
				fseek(fp, 0, SEEK_SET);
				fseek(fp, previous_offset, SEEK_CUR);
				fwrite(&file_offset, 4, 1, fp);
				break;
			}
			else{
				previous_offset = x + 28;
			}
		}
	}
	fseek(fp, 0, SEEK_SET);
	fseek(fp, offset, SEEK_CUR);
	printf("Successfully deleted\n--------------------------------------------------------------------------------------\n");
}

void download_file(FILE *fp, char *file_name){
	FILE *copy = fopen("output.txt", "w");
	long int offset = ftell(fp);
	long int file_offset;
	long int size;
	char* file = (char*)malloc(sizeof(char) * 28);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, offset - 4, SEEK_CUR);
	fread(&file_offset, 4, 1, fp);
	if (!file_offset){
		printf("No files to download\n");
		return;
	}
	else{
		while (file_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, file_offset, SEEK_CUR);
			fread(file, 28, 1, fp);
			fread(&file_offset, 4, 1, fp);
			fread(&size, 4, 1, fp);
			if (!strcmp(file, file_name)){
				char * string = (char*)malloc(sizeof(char) * size);
				fread(string, size, 1, fp);
				fwrite(string, size-1, 1, copy);
				break;
			}
		}
	}
	fseek(fp, 0, SEEK_SET);
	fseek(fp, offset, SEEK_CUR);
	printf("Successfully downloaded\n--------------------------------------------------------------------------------\n");
	fclose(copy);
}