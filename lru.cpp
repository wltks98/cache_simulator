
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_CACHE_SIZE 8192
#define TRACE_FILE_NAME "ref_stream.txt"


typedef unsigned long element;
typedef struct buffer {
	element data;
	struct buffer* llink;
	struct buffer* rlink;

} buffer;

buffer cache_buffer[MAX_CACHE_SIZE + 1];
int j = 0;

buffer* search(buffer* phead, element data);
int is_full(void);
void ddelete(buffer* head, buffer* removed);
void switched(buffer* head, buffer* moved);

unsigned long hit = 0, miss = 0;
float ratio;

void init(buffer* head)
{
	head->llink = head;
	head->rlink = head;

}



void dinsert(buffer* before, buffer* head, element data)
{

	if (search(head, data) == NULL) {
		if (is_full()) {

			buffer* p = head->llink;

			ddelete(head, head->llink);
			
			buffer* newnode = p;

			newnode->data = data;
			newnode->llink = before;
			newnode->rlink = before->rlink;
			before->rlink->llink = newnode;
			before->rlink = newnode;
			miss++;
			

		}

		else {
			buffer* newnode = &cache_buffer[j++];


			newnode->data = data;
			newnode->llink = before;
			newnode->rlink = before->rlink;
			before->rlink->llink = newnode;
			before->rlink = newnode;
			miss++;
		}
	}
	else {


		switched(head, search(head, data));

		hit++;
	}


}

void switched(buffer* head, buffer* moved) {

	moved->llink->rlink = moved->rlink;
	moved->rlink->llink = moved->llink;
	moved->rlink = head->rlink;
	moved->llink = head;
	head->rlink->llink = moved;
	head->rlink = moved;




}


void ddelete(buffer* head, buffer* removed)
{


	if (removed == head) return;
	removed->llink->rlink = removed->rlink;
	removed->rlink->llink = removed->llink;



}

int is_full(void) {


	return (j == MAX_CACHE_SIZE + 1);


}


buffer* search(buffer* head, element data) {

	buffer* p;

	for (p = head->rlink; p != head; p = p->rlink) {
		if (p->data == data)
			return p;


	}
	return 0;
}


int main(int argc, char* argv[])
{
	buffer* head = &cache_buffer[j++];
	
	init(head);

	FILE* fp = NULL;
	char bb[100];


	if ((fp = fopen(TRACE_FILE_NAME, "r")) == NULL) {
		printf("%s trace file open fail.\n", TRACE_FILE_NAME);

		return 0;
	}
	else {
		printf("start simulation!\n");
	}

	char* notfound;


	while (fgets(bb, 100, fp) != NULL) {
		dinsert(head, head, strtol(bb, &notfound, 10));
		

	}

	ratio = 100.0 * hit / (hit + miss);

	printf("hit ratio: %f%%", ratio);


	fclose(fp);


	return 0;
}







