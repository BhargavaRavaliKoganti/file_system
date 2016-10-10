#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <conio.h>
#include <string.h>
//fsutil file createnew <filename> 8388608
struct user{
	char user_name[28];
	int category_count;
};
struct category{
	char category_name[28];
	long int message_offset;
};
struct message{
	char message_of_user[128];
	char user_name[28];
	long int reply_offset;
	long int message_offset;
};
struct reply{
	char reply_of_user[128];
	char user_name[28];
	long int reply_offset;
};


long int free_space;
char login_username[28];
int message_count = 1;
long int view_message_offset;
long int view_reply_offset;
int category_offset_in_replies;
//long int deleted_message_space;
//long int deleted_reply_space;

void login(FILE *);
void register_user(FILE *);
void check_category(FILE *);
void view_categories(FILE *);
void category_options(FILE *);
void dump_category(FILE *);
void message_options(FILE *);
void add_messages(FILE *);
void view_messages(FILE *);
void select_category(FILE *);
void print_messages(FILE *, long int);
void reply_options(FILE *);
void add_reply(FILE *);
void view_replies(FILE *);
void print_replies(FILE *, long int);
void delete_messages(FILE *);
void delete_replies(FILE *);

int main(){
	FILE *fp;
	if (!(fopen("message_store2.bin", "rb+"))){
		system("fsutil file createnew message_store2.bin 8388608");
	}
	fp = fopen("message_store2.bin", "rb+");
	fread(&free_space, 4, 1, fp);
	if (!free_space){
		free_space = 32 * 20 + 8 + 8 + 32 * 100;
		fseek(fp, 0, SEEK_SET);
		fwrite(&free_space, 4, 1, fp);
	}
	fseek(fp, 0, SEEK_SET);
	int choice;
	printf("1. Login\n2. Register\n3. Exit\n");
	scanf("%d", &choice);
	switch (choice){
		case 1:
			login(fp);
			break;
		case 2:
			register_user(fp);
			break;
		default:
			exit(0);
			break;
	}
	fclose(fp);
	return 0;
}

void register_user(FILE *fp){
	struct user users;
	char username[28];
	int user_count = 0;
	printf("Enter user name:\n");
	scanf("%s", username);
	strcpy(users.user_name, username);
	strcpy(login_username, username);
	users.category_count = 0;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4 + 4 + 4, SEEK_CUR);
	fread(&user_count, 4, 1, fp);
	user_count++;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4 + 4 + 4, SEEK_CUR);
	fwrite(&user_count, 4, 1, fp);
	fseek(fp, 32 * (user_count-1), SEEK_CUR);
	fwrite(&users, 32, 1, fp);
	system("cls");
	printf("\n=================================Welcome %s=================================\n", users.user_name);
	category_options(fp);
}

void login(FILE *fp){
	char username[28];
	printf("Enter user name:\n");
	fflush(stdin);
	gets(username);
	int user_count;
	struct user users;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4 + 4 + 4, SEEK_CUR);
	fread(&user_count, 4, 1, fp);
	for (int i = 0; i < user_count; i++){
		fread(&users.user_name, 32, 1, fp);
		if (!strcmp(users.user_name, username)){
			strcpy(login_username, username);
			system("cls");
			printf("\n=================================Welcome %s=================================\n", users.user_name);
			category_options(fp);
		}
	}
	printf("Invalid user login again!\n");
}


void category_options(FILE *fp){
	int choice;
	while (1){
		printf("1. Add category\n2. view categories\n3. logout\nEnter choice:\n");
		scanf("%d", &choice);
		printf("--------------------------------------------------------------------------------\n");
		switch (choice)	{
		case 1:
			check_category(fp);
			break;
		case 2:
			view_categories(fp);
			break;
		default:
			exit(0);
			break;
		}
	}
}

void check_category(FILE *fp){
	fseek(fp, -4, SEEK_CUR);
	int category_count = 0;
	int user_index;
	struct user users;
	fread(&category_count, 4, 1, fp);
	if (category_count == 5){
		printf("Reached maximum categories entry\n");
	}
	else{
		fseek(fp, -32, SEEK_CUR);
		dump_category(fp);
	}
}

void dump_category(FILE *fp){
	int user_index = 0;
	struct user users;
	int category_count;
	user_index = (ftell(fp) - 8) / 32;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 32 * user_index + 8 + 8, SEEK_CUR);
	fread(&users, 32, 1, fp);
	category_count = users.category_count;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 32 * 20 + 8 + 8 + (32 * user_index * 5) + (32*(category_count)), SEEK_CUR);
	struct category categories;
	char categoryname[28];
	printf("Enter the category name:\n");
	fflush(stdin);
	gets(categoryname);
	strcpy(categories.category_name, categoryname);
	categories.message_offset = 0;
	fwrite(&categories, 32, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 32 * user_index + 8 + 8, SEEK_CUR);
	fread(&users, 32, 1, fp);
	users.category_count++;
	fseek(fp, -32, SEEK_CUR);
	fwrite(&users, 32, 1, fp);
	printf("Succesfully created\n--------------------------------------------------------------------------------\n");
}

void view_categories(FILE *fp){
	int user_count;
	struct user users;
	struct category categories;
	int index = 1;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4 + 4 + 4, SEEK_CUR);
	fread(&user_count, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	int cateogory_present = 0;
	for (int i = 0; i < user_count; i++){
		fseek(fp, (32 * i) + 8 + 8, SEEK_CUR);
		fread(&users, 32, 1, fp);
		if (users.category_count){
			cateogory_present = 1;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 32 * 20 + 8 + 8 + 32 * i * 5, SEEK_CUR);
			for (int j = 0; j < users.category_count; j++){
				fread(&categories, 32, 1, fp);
				printf("%d. %s\n", index, categories.category_name);
				index++;
			}
			fseek(fp, 0, SEEK_SET);
		}
	}
	if (cateogory_present){
		select_category(fp);
	}
	else{
		printf("No categories to view! Add categories\n");
		printf("--------------------------------------------------------------------------------------\n");
	}
}

void select_category(FILE *fp){
	int choice;
	int user_count;
	struct user users;
	printf("\n--------------------------------------------------------------------------------\nselect category:\n");
	scanf("%d", &choice);
	printf("--------------------------------------------------------------------------------\n");
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4 + 4 + 4, SEEK_CUR);
	fread(&user_count, 4, 1, fp);
	for (int i = 0; i < user_count; i++){
		fread(&users, 32, 1, fp);
		if (choice <= users.category_count){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 32 * 20 + 8 + 8 + 32 * i * 5 + 32 * (choice-1), SEEK_CUR);
			message_options(fp);
		}
		else{
			choice -= users.category_count;
		}

	}

}

void message_options(FILE *fp){
	int choice;
	while (1){
		printf("1. Add message\n2. View messages\n3. Back\nEnter choice:\n");
		scanf("%d", &choice);
		printf("--------------------------------------------------------------------------------\n");
		switch (choice){
		case 1:
			add_messages(fp);
			break;
		case 2:
			message_count = 1;
			view_messages(fp);
			break;
		default:
			category_options(fp);
			break;
		}
	}
}

void add_messages(FILE *fp){
	int category_offset = ftell(fp);
	long int delete_message_space;
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4, SEEK_CUR);
	fread(&delete_message_space, 4, 1, fp);
	long int last_message_offset;
	struct category categories;
	char message_of_user[128];
	struct message messages;
	struct message temp_message;
	struct message deleted_message;
	int flag = 0;
	printf("Enter message:\n");
	fflush(stdin);
	gets(message_of_user);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, category_offset, SEEK_CUR);
	fread(&categories, 32, 1, fp);
	strcpy(messages.message_of_user, message_of_user);
	strcpy(messages.user_name, login_username);
	if (!categories.message_offset){
		if (!delete_message_space){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, free_space, SEEK_CUR);
		}
		else{
			flag = 1;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, delete_message_space, SEEK_CUR);
			fread(&deleted_message, 128 + 28 + 4 + 4, 1, fp);
			delete_message_space = deleted_message.message_offset;
			fseek(fp, -(128 + 28 + 4 + 4), SEEK_CUR);
		}
		categories.message_offset = ftell(fp);
		messages.message_offset = 0;
		messages.reply_offset = 0;
		fwrite(&messages, 128 + 28 + 4 + 4, 1, fp);
		if (flag){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 4, SEEK_CUR);
			fwrite(&delete_message_space, 4, 1, fp);
		}
		else{
			free_space = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			fwrite(&free_space, 4, 1, fp);
		}
		fseek(fp, 0, SEEK_SET);
		fseek(fp, category_offset, SEEK_CUR);
		fwrite(&categories, 32, 1, fp);
	}
	else{
		fseek(fp, 0, SEEK_SET);
		fseek(fp, categories.message_offset, SEEK_CUR);
		last_message_offset = ftell(fp);
		fread(&temp_message, 128 + 28 + 4 + 4, 1, fp);
		while (temp_message.message_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, temp_message.message_offset, SEEK_CUR);
			last_message_offset = ftell(fp);
			fread(&temp_message, 128 + 28 + 4 + 4, 1, fp);
		}
		if (!delete_message_space){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, free_space, SEEK_CUR);
		}
		else{
			flag = 1;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, delete_message_space, SEEK_CUR);
			fread(&deleted_message, 128 + 28 + 4 + 4, 1, fp);
			delete_message_space = deleted_message.message_offset;
			fseek(fp, -(128 + 28 + 4 + 4), SEEK_CUR);
		}
		temp_message.message_offset = ftell(fp);
		messages.message_offset = 0;
		messages.reply_offset = 0;
		fwrite(&messages, 128 + 28 + 4 + 4, 1, fp);
		if (flag){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 4, SEEK_CUR);
			fwrite(&delete_message_space, 4, 1, fp);
		}
		else{
			free_space = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			fwrite(&free_space, 4, 1, fp);
		}
		fseek(fp, 0, SEEK_SET);
		fseek(fp, last_message_offset, SEEK_CUR);
		fwrite(&temp_message, 128 + 28 + 4 + 4, 1, fp);
	}
	fseek(fp, 0, SEEK_SET);
	fseek(fp, category_offset, SEEK_CUR);
	printf("Succesfully Added\n--------------------------------------------------------------------------------\n");
}

void view_messages(FILE *fp){
	int offset = ftell(fp);
	int choice;
	struct category categories;
	fread(&categories, 32, 1, fp);
	print_messages(fp, categories.message_offset);
	while (1){
		printf("1. select message\n2. to view next set of messages\n3. delete a message\n4. back\n");
		fflush(stdin);
		scanf("%d", &choice);
		printf("--------------------------------------------------------------------------------\n");
		switch (choice){
		case 1:
			fseek(fp, 0, SEEK_SET);
			fseek(fp, offset, SEEK_CUR);
			reply_options(fp);
			break;
		case 2:
			print_messages(fp, view_message_offset);
			break;
		case 3:
			fseek(fp, 0, SEEK_SET);
			fseek(fp, offset, SEEK_CUR);
			delete_messages(fp);
			break;
		case 4:
			category_options(fp);
			break;
		}
	}
}

void print_messages(FILE *fp, long int message_offset){
	int a = 1;
	struct message messages;
	while (a < 10 ){
		if (message_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, message_offset, SEEK_CUR);
			fread(&messages, 128 + 28 + 4 + 4, 1, fp);
			printf("%d .%s\n", message_count, messages.message_of_user);
			message_offset = messages.message_offset;
			view_message_offset = messages.message_offset;
			a++;
			message_count++;
		}
		else{
			printf("---------------------------------Reached maximum--------------------------------\n");
			break;
		}
	}
}

void reply_options(FILE *fp){
	int category_offset = ftell(fp);
	category_offset_in_replies = category_offset;
	int message_choice;
	int count = 0;
	struct category categories;
	struct message messages;
	long int offset;
	int choice;
	printf("select a message to view:\n");
	fflush(stdin);
	scanf("%d", &message_choice);
	printf("--------------------------------------------------------------------------------\n");
	fread(&categories, 32, 1, fp);
	offset = categories.message_offset;
	while (count < message_choice && offset){
		fseek(fp, 0, SEEK_SET);
		fseek(fp, offset, SEEK_CUR);
		fread(&messages, 128 + 28 + 4 + 4, 1, fp);
		offset = messages.message_offset;
		count++;
	}
	fseek(fp, -(128 + 28 + 4 + 4), SEEK_CUR);
	while (1){
		printf("1. Add reply\n2. View replies\n3. back\n");
		fflush(stdin);
		scanf("%d", &choice);
		printf("--------------------------------------------------------------------------------\n");
		switch (choice){
			case 1:
				add_reply(fp);
				break;
			case 2:
				view_replies(fp);
			case 3:
				fseek(fp, 0, SEEK_SET);
				fseek(fp, category_offset, SEEK_CUR);
				message_options(fp);
				break;
		}
	}
}

void add_reply(FILE *fp){
	int message_offset = ftell(fp);
	long int delete_reply_space;
	struct reply deleted_reply;
	struct message messages;
	struct reply replies;
	struct reply temp_reply;
	long int last_reply_offset;
	char reply_of_user[128];
	int flag = 0;
	printf("--------------------------------------------------------------------------------\n");
	printf("Enter reply:\n");
	fflush(stdin);
	gets(reply_of_user);
	strcpy(replies.reply_of_user, reply_of_user);
	strcpy(replies.user_name, login_username);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 8, SEEK_CUR);
	fread(&delete_reply_space, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, message_offset, SEEK_CUR);
	fread(&messages, 128 + 28 + 4 + 4, 1, fp);
	if (!messages.reply_offset){
		if (!delete_reply_space){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, free_space, SEEK_CUR);
		}
		else{
			flag = 1;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, delete_reply_space, SEEK_CUR);
			fread(&deleted_reply, 128 + 28 + 4, 1, fp);
			delete_reply_space = deleted_reply.reply_offset;
			fseek(fp, -(128 + 28 + 4), SEEK_CUR);
		}
		messages.reply_offset = ftell(fp);
		replies.reply_offset = 0;
		fwrite(&replies, 128 + 28 + 4, 1, fp);
		if (flag){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&delete_reply_space, 4, 1, fp);
		}
		else{
			free_space = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			fwrite(&free_space, 4, 1, fp);
		}
		fseek(fp, 0, SEEK_SET);
		fseek(fp, message_offset, SEEK_CUR);
		fwrite(&messages, 128 + 28 + 4 + 4, 1, fp);
	}
	else{
		fseek(fp, 0, SEEK_SET);
		fseek(fp, messages.reply_offset, SEEK_CUR);
		last_reply_offset = ftell(fp);
		fread(&temp_reply, 128 + 28 + 4, 1, fp);
		while (temp_reply.reply_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, temp_reply.reply_offset, SEEK_CUR);
			last_reply_offset = ftell(fp);
			fread(&temp_reply, 128 + 28 + 4, 1, fp);
		}
		if (!delete_reply_space){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, free_space, SEEK_CUR);
		}
		else{
			flag = 1;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, delete_reply_space, SEEK_CUR);
			fread(&deleted_reply, 128 + 28 + 4, 1, fp);
			delete_reply_space = deleted_reply.reply_offset;
			fseek(fp, -(128 + 28 + 4), SEEK_CUR);
		}
		temp_reply.reply_offset = ftell(fp);
		replies.reply_offset = 0;
		fwrite(&replies, 128 + 28 + 4, 1, fp);
		if (flag){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&delete_reply_space, 4, 1, fp);
		}
		else{
			free_space = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			fwrite(&free_space, 4, 1, fp);
		}
		fseek(fp, 0, SEEK_SET);
		fseek(fp, last_reply_offset, SEEK_CUR);
		fwrite(&temp_reply, 128 + 28 + 4, 1, fp);
	}
	fseek(fp, 0, SEEK_SET);
	fseek(fp, message_offset, SEEK_CUR);
	printf("Successfully added\n--------------------------------------------------------------------------------\n");
}

void view_replies(FILE *fp){
	int offset = ftell(fp);
	int choice;
	struct message messages;
	fread(&messages, 128 + 28 + 4 + 4, 1, fp);
	print_replies(fp, messages.reply_offset);
	while (1){
		printf("1. to view next set of replies\n2. delete reply\n3. back\n");
		fflush(stdin);
		scanf("%d", &choice);
		printf("--------------------------------------------------------------------------------\n");
		switch (choice){
		case 1:
			print_replies(fp, view_reply_offset);
			break;
		case 2:
			fseek(fp, 0, SEEK_SET);
			fseek(fp, offset, SEEK_CUR);
			delete_replies(fp);
			break;
		case 3:
			fseek(fp, 0, SEEK_SET);
			fseek(fp, category_offset_in_replies, SEEK_CUR);
			reply_options(fp);
			break;
		}
	}
}

void print_replies(FILE *fp, long int reply_offset){
	int a = 1;
	struct message replies;
	int count = 1;
	while (a < 10){
		if (reply_offset){
			fseek(fp, 0, SEEK_SET);
			fseek(fp, reply_offset, SEEK_CUR);
			fread(&replies, 128 + 28 + 4, 1, fp);
			printf("%d .%s\n", count, replies.message_of_user);
			reply_offset = replies.reply_offset;
			view_reply_offset = replies.reply_offset;
			a++;
			count++;
		}
		else{
			printf("---------------------------------Reached maximum--------------------------------\n");
			break;
		}
	}
}

void delete_messages(FILE *fp){
	int category_offset = ftell(fp);
	long int deleted_message_space;
	long int deleted_reply_space;
	int message_offset;
	int message_choice;
	long int offset;
	int count = 1;
	long int previous_message_offset;
	struct category categories;
	struct message messages;
	struct message temp_message;
	struct reply replies;
	long int replies_offset;
	printf("select a message to delete :\n");
	fflush(stdin);
	scanf("%d", &message_choice);
	fread(&categories, 32, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 4, SEEK_CUR);
	fread(&deleted_message_space, 4, 1, fp);
	fread(&deleted_reply_space, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, categories.message_offset, SEEK_CUR);
	offset = categories.message_offset;
	if (message_choice == 1){
		fread(&messages, 128 + 28 + 4 + 4, 1, fp);
		categories.message_offset = messages.message_offset;
		fseek(fp, 0, SEEK_SET);
		fseek(fp, category_offset, SEEK_CUR);
		fwrite(&categories, 32, 1, fp);
		if (!deleted_message_space){
			deleted_message_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 4, SEEK_CUR);
			fwrite(&deleted_message_space, 4, 1, fp);
		}
		else{
			messages.message_offset = deleted_message_space;
			deleted_message_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 4, SEEK_CUR);
			fwrite(&deleted_message_space, 4, 1, fp);
		}
	}
	else{
		while (count <= message_choice && offset){
			previous_message_offset = offset;
			fread(&messages, 128 + 28 + 4 + 4, 1, fp);
			offset = ftell(fp) - (128 + 28 + 4 + 4);
			count++;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, messages.message_offset, SEEK_CUR);
		}
		fseek(fp, 0, SEEK_SET);
		fseek(fp, previous_message_offset, SEEK_CUR);
		fread(&temp_message, 128 + 28 + 4 + 4, 1, fp);
		temp_message.message_offset = messages.message_offset;
		fseek(fp, -(128 + 28 + 4 + 4), SEEK_CUR);
		fwrite(&temp_message, 128 + 28 + 4 + 4, 1, fp);
		if (!deleted_message_space){
			deleted_message_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 4, SEEK_CUR);
			fwrite(&deleted_message_space, 4, 1, fp);
		}
		else{
			messages.message_offset = deleted_message_space;
			deleted_message_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 4, SEEK_CUR);
			fwrite(&deleted_message_space, 4, 1, fp);
		}
	}
	if (messages.reply_offset){
		replies_offset = messages.reply_offset;
		if (!deleted_reply_space){
			deleted_reply_space = messages.reply_offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&deleted_reply_space, 4, 1, fp);
		}
		else{
			while (replies_offset){
				fseek(fp, 0, SEEK_SET);
				fseek(fp, replies_offset, SEEK_CUR);
				fread(&replies, 128 + 28 + 4, 1, fp);
				replies_offset = replies.reply_offset;
			}
			replies.reply_offset = deleted_reply_space;
			deleted_reply_space = messages.reply_offset;
			fseek(fp, -(128 + 28 + 4), SEEK_CUR);
			fwrite(&replies, 128 + 28 + 4, 1, fp);
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&deleted_reply_space, 4, 1, fp);
		}
	}
	printf("Successfully Deleted\n--------------------------------------------------------------------------------\n");
}

void delete_replies(FILE *fp){
	int message_offset = ftell(fp);
	struct message messages;
	struct reply replies;
	struct reply temp_reply;
	long int previous_reply_offset;
	int reply_choice;
	long int deleted_reply_space;
	long int offset;
	int count = 1;
	printf("--------------------------------------------------------------------------------\nselect a reply to delete :\n");
	fflush(stdin);
	scanf("%d", &reply_choice);
	fread(&messages, 128 + 28 + 4 + 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 8, SEEK_CUR);
	fread(&deleted_reply_space, 4, 1, fp);
	fseek(fp, 0, SEEK_SET);
	fseek(fp, messages.reply_offset, SEEK_CUR);
	offset = messages.reply_offset;
	if (reply_choice == 1){
		fread(&replies, 128 + 28 + 4, 1, fp);
		messages.reply_offset = replies.reply_offset;
		fseek(fp, 0, SEEK_SET);
		fseek(fp, message_offset, SEEK_CUR);
		fwrite(&messages, 128 + 28 + 4 + 4, 1, fp);
		if (!deleted_reply_space){
			deleted_reply_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&deleted_reply_space, 4, 1, fp);
		}
		else{
			replies.reply_offset = deleted_reply_space;
			deleted_reply_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&deleted_reply_space, 4, 1, fp);
		}
	}
	else{
		while (count <= reply_choice && offset){
			previous_reply_offset = offset;
			fread(&replies, 128 + 28 + 4, 1, fp);
			offset = ftell(fp) - (128 + 28 + 4);
			count++;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, replies.reply_offset, SEEK_CUR);
		}
		fseek(fp, 0, SEEK_SET);
		fseek(fp, previous_reply_offset, SEEK_CUR);
		fread(&temp_reply, 128 + 28 + 4, 1, fp);
		temp_reply.reply_offset = replies.reply_offset;
		fseek(fp, -(128 + 28 + 4), SEEK_CUR);
		fwrite(&temp_reply, 128 + 28 + 4, 1, fp);
		if (!deleted_reply_space){
			deleted_reply_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&deleted_reply_space, 4, 1, fp);
		}
		else{
			replies.reply_offset = deleted_reply_space;
			deleted_reply_space = offset;
			fseek(fp, 0, SEEK_SET);
			fseek(fp, 8, SEEK_CUR);
			fwrite(&deleted_reply_space, 4, 1, fp);
		}
	}
	printf("Successfully Deleted\n--------------------------------------------------------------------------------\n");
}