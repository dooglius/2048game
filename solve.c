#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

struct state{
	unsigned int arr[4][4];
	int score;
};

//const char reclist[] = {8,7,6,6,5,5,4,4,4,4,4,4,3,3,3};
const char reclist[] = {7,6,5,5,5,5,4,4,4,4,4,4,4,4,3};

int fd;

void print(struct state *s){
	char i,j;
	printf("#################################\n");
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			char v = s->arr[j][i];
			if(v==0) printf("\t# ");
			else printf("%d\t# ",1 << v);
		}
		printf("\n");
		printf("#################################\n");
	}
	printf("\n\n");
}

bool move(struct state *s, char dir){
	char c,i,j,next;
	bool canmix = false;
	char newscore = s->score;
	bool ans = false;
	switch(dir){
	case UP:
		for(i = 0; i < 4; i++){
			canmix=false;
			c = 0;
			for(j = 0; j < 4; j++){
				next = s->arr[i][j];
				if(next == 0) continue;
				if(canmix && s->arr[i][c-1] == next){
					s->arr[i][c-1] = next+1;
					canmix = false;
					newscore++;
					ans=true;
				} else {
					s->arr[i][c] = next;
					if(c != j) ans=true;
					canmix = true;
					c++;
				}
			}
			for(; c<4; c++){
				s->arr[i][c] = 0;
			}
		}
		break;
	case LEFT:
		for(i = 0; i < 4; i++){
			canmix=false;
			c = 0;
			for(j = 0; j < 4; j++){
				next = s->arr[j][i];
				if(next == 0) continue;
				if(canmix && s->arr[c-1][i] == next){
					s->arr[c-1][i] = next+1;
					canmix = false;
					newscore++;
					ans=true;
				} else {
					s->arr[c][i] = next;
					if(c != j) ans=true;
					canmix = true;
					c++;
				}
			}
			for(; c<4; c++){
				s->arr[c][i] = 0;
			}
		}
		break;
	case DOWN:
		for(i = 0; i < 4; i++){
			canmix=false;
			c = 0;
			for(j = 0; j < 4; j++){
				next = s->arr[i][3-j];
				if(next == 0) continue;
				if(canmix && s->arr[i][4-c] == next){
					s->arr[i][4-c] = next+1;
					canmix = false;
					newscore++;
					ans=true;
				} else {
					s->arr[i][3-c] = next;
					if(c != j) ans=true;
					canmix = true;
					c++;
				}
			}
			for(; c<4; c++){
				s->arr[i][3-c] = 0;
			}
		}
		break;
	case RIGHT:
		for(i = 0; i < 4; i++){
			canmix=false;
			c = 0;
			for(j = 0; j < 4; j++){
				next = s->arr[3-j][i];
				if(next == 0) continue;
				if(canmix && s->arr[4-c][i] == next){
					s->arr[4-c][i] = next+1;
					canmix = false;
					newscore++;
					ans=true;
				} else {
					s->arr[3-c][i] = next;
					if(c != j) ans=true;
					canmix = true;
					c++;
				}
			}
			for(; c<4; c++){
				s->arr[3-c][i] = 0;
			}
		}
		break;
	}
	s->score=newscore;
	return ans;
}

double go2(struct state *s, char rec);

double go(struct state *s, char rec){
	struct state temp;
	double ans = -100;

	int i;
	for(i=0; i<4; i++){
		temp=*s;
		if(move(&temp, i)){
			double t = go2(&temp,rec);
			if(t>ans) ans=t;
		}
	}
	return ans;
}

double go2(struct state *s, char rec){
	if(rec == 0) return s->score;
	char i,j,c;
	double ans;
	c = 0;
	ans = 0;
	s->score--;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(s->arr[i][j] == 0){
				s->arr[i][j] = 1;
				if(rec>3){
					double t = 0.9*go(s, rec-1);
					s->arr[i][j] = 2;
					ans += t+0.1*go(s,rec-1);
				} else {
					ans += go(s,rec-1);
				}
				s->arr[i][j] = 0;
				c++;
			}
		}
	}
	s->score++;
	if(c==0) return -100;
	return ans/c;
}


unsigned char randbyte(void){
	unsigned char r;
/*
	if(read(fd,&r,1) != 1){
		printf("Error getting random number\n");
		exit(1);
	}
*/
	r = rand();
	return r;
}

void addrand(struct state *s){
	char v,x,y;
	unsigned char r;
	bool parity = true;
	do{
		if(parity) r = randbyte();
		else r = r >> 4;
		x = r&3;
		y = (r&12) >> 2;
		parity = !parity;
	} while(s->arr[x][y] != 0);

	while(1){
		if(parity) r = randbyte();
		else r = r >> 4;
		if(r<8) v=1;
		else if(r<10) v=2;
		else{
			parity = !parity;
			continue;
		}
		break;
	}
	s->arr[x][y]=v;
	s->score--;
}

int main(int argc, char** argv){
	int x,y,v,m;
	struct state s;
	char i,j;

	if(argc <= 1 || (m=atoi(argv[1])) == 0){
		printf("Mode 1: copy from start of browser game\n");
		printf("Mode 2: start from a specific position\n");
		printf("Mode 3: play automatically\n");
		while(1){
			printf("Select mode: ");
			scanf("%d",&m);
			if(m<=0 || m>3){
				printf("Bad input\n");
			} else {
				break;
			}
		}
	}
	if(m==1){
		char score = 0;
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				int t;
				printf("(%d,%d): ",i+1,j+1);
				scanf("%d",&t);
				if(t==0){
					score++;
					s.arr[i][j] = 0;
				} else {
					s.arr[i][j] = (int)round(log2((double)t));
				}	
			}
		}
		s.score=score;
	} else if(m==2){
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				s.arr[i][j]=0;
			}
		}
		while(1){
			printf("Enter x y v: ");
			scanf("%d %d %d",&x,&y,&v);
			if(x<1 || x>4 || y<1 || y>4 || (v != 2 && v != 4)){
				printf("bad input!\n");
			} else {
				break;
			}
		}
		x--;
		y--;
		s.arr[x][y]=v/2;
		s.score = 15;
	} else {
		fd = open("/dev/urandom",O_RDONLY);

		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				s.arr[i][j]=0;
			}
		}
		s.score=16;
		addrand(&s);
		addrand(&s);
	}

	while(1){
		if(m==2){
			m=1;
		} else if(m==1){
			while(1){
				printf("Enter x y v: ");
				scanf("%d %d %d",&x,&y,&v);
				if(x<1 || x>4 || y<1 || y>4 || (v != 2 && v != 4)){
					printf("bad input!\n");
				} else {
					break;
				}
			}
			x--;
			y--;
			s.arr[x][y]=v/2;
			s.score--;
		}
	
		struct state dir[4];
//		char rec = reclist[s.score];
		char rec = 3;
		
		double top = -101;
		struct state *best = NULL;

		for(i=0; i<4; i++){
			dir[i] = s;
			if(move(&(dir[i]),i)){
				double t = go2(&(dir[i]), rec);
				if(t>top){
					top=t;
					best=&(dir[i]);
				}
			}
		}

		if(best==NULL){
			printf("No good moves, ending in this state:\n\n\n");
			print(&s);
			return 0;
		}

		if(best == &(dir[0])){
			printf("Up\n");
		} else if(best == &(dir[1])){
			printf("Right\n");
		} else if(best == &(dir[2])){
			printf("Down\n");
		} else {
			printf("Left\n");
		}

		s = *best;

		if(m == 3) addrand(&s);

		print(&s);
	}
}
