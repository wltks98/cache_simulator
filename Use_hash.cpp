
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#pragma warning(disable:4996)
#define MAX_CACHE_SIZE 8192
#define TRACE_FILE_NAME "ref_stream.txt"
#define HASH_SIZE  MAX_CACHE_SIZE*3/4



typedef struct buffer {

	unsigned long blkno;
	struct buffer* next, * prev;
	struct buffer* hash_next, * hash_prev;

}buffer;


buffer lrulist;

buffer cache_buffer[MAX_CACHE_SIZE];

buffer hash_table[HASH_SIZE];

unsigned long hit, miss, curtime;

int hash_function(int key)
{
	return key % HASH_SIZE;
}


void hash_add(unsigned long blkno, buffer* ptr) {




	int hash_value = hash_function(blkno);

	struct buffer* head = &hash_table[hash_value];


	ptr->hash_prev = head->hash_prev;
	ptr->hash_next = head;
	head->hash_prev->hash_next = ptr;
	head->hash_prev = ptr;



}

void hash_delete(buffer* removed) {



	removed->hash_prev->hash_next = removed->hash_next;
	removed->hash_next->hash_prev = removed->hash_prev;



}



void lruin(buffer* bp) {  //제일 앞에 삽입

	buffer* dp = &lrulist;  //헤드

	bp->next = dp->next;
	bp->prev = dp;
	(dp->next)->prev = bp;
	dp->next = bp;

	hash_add(bp->blkno, bp);

}

void lruin2(buffer* bp) {  //제일 앞에 삽입 (hash_add 없는 버전)

	buffer* dp = &lrulist;  //헤드

	bp->next = dp->next;
	bp->prev = dp;
	(dp->next)->prev = bp;
	dp->next = bp;



}


buffer* lruout() { //제일 끝을 삭제
	buffer* bp;

	bp = lrulist.prev;

	(bp->prev)->next = bp->next;
	(bp->next)->prev = bp->prev;

	hash_delete(bp);


	return bp;
}


void reorder(buffer* bp) {   //순서를 바꿈

	(bp->prev)->next = bp->next;
	(bp->next)->prev = bp->prev;

	lruin2(bp);
}



void init()
{
	int i;

	lrulist.next = lrulist.prev = &lrulist;



	for (i = 0; i < HASH_SIZE; i++) {

		hash_table[i].blkno = ~0;
		hash_table[i].hash_next = hash_table[i].hash_prev = &hash_table[i];



	}



	i = 0;
	for (i = 0; i < MAX_CACHE_SIZE; i++) {
		cache_buffer[i].blkno = ~0;
		cache_buffer[i].next = cache_buffer[i].prev = NULL;

		lruin(&cache_buffer[i]);


	}




	return;

}

buffer* findblk(unsigned long blkno) {  // 버퍼에 존재하는지 search 

	buffer* head, * p;

	int hash_value = hash_function(blkno);




	head = &hash_table[hash_value];
	for (p = head->hash_next; p != head; p = p->hash_next) {

		if (p->blkno == blkno)
			return p;


	}



	return NULL;

}

void pgref(unsigned long blkno) {   //메모리 읽기

	buffer* bufp;



	bufp = findblk(blkno);
	if (bufp) {
		hit++;

		reorder(bufp);
	}
	else {
		miss++;

		bufp = lruout();
		bufp->blkno = blkno;
		lruin(bufp);

	}




}


int main(int argc, char* argv[])
{
	int ret;
	unsigned long blkno;
	init();
	FILE* fp = NULL;

	curtime = miss = hit = 0;


	if ((fp = fopen(TRACE_FILE_NAME, "r")) == NULL) {
		printf("%s trace file open fail.\n", TRACE_FILE_NAME);

		return 0;
	}
	else {
		printf("start simulation!\n");
	}

	char* notfound;

	clock_t start, stop;
	double duration;

	start = clock();
	while ((ret = fscanf(fp, "%lu\n", &blkno)) != EOF) {
		curtime++;
		pgref(blkno);


	}

	stop = clock();
	printf("%d\n%d\n%d\n%f\n", curtime, hit, miss, (100.0 * hit / (hit + miss)));

	duration = (double)(stop - start) / CLOCKS_PER_SEC;
	printf("%f초\n", duration);


	fclose(fp);


	return 0;
}


