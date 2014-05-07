#include <math.h>
#include <stdbool.h>
#include <stdint.h>
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
const char reclist[] = {6,5,4,4,4,4,4,3,3,3,3,3,3,3,3};

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

bool moveup(struct state *s){
	char c,i,j,next;
	bool canmix = false;
	char newscore = s->score;
	bool ans = false;
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
	s->score=newscore;
	return ans;
}
bool moveleft(struct state *s){
	char c,i,j,next;
	bool canmix = false;
	char newscore = s->score;
	bool ans = false;
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
	s->score=newscore;
	return ans;
}
bool movedown(struct state *s){
	char c,i,j,next;
	bool canmix = false;
	char newscore = s->score;
	bool ans = false;
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
	s->score=newscore;
	return ans;
}
bool moveright(struct state *s){
	char c,i,j,next;
	bool canmix = false;
	char newscore = s->score;
	bool ans = false;
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
	s->score=newscore;
	return ans;
}

double go2(struct state *s, char rec);

double go(struct state *s, char rec){
	struct state temp;
	double ans = -101;

	int i;
	temp = *s;
	if(moveup(&temp)){
		double t = go2(&temp,rec);
		if(t>ans) ans=t;
	}
	temp = *s;
	if(moveleft(&temp)){
		double t = go2(&temp,rec);
		if(t>ans) ans=t;
	}
	temp = *s;
	if(movedown(&temp)){
		double t = go2(&temp,rec);
		if(t>ans) ans=t;
	}
	temp = *s;
	if(moveright(&temp)){
		double t = go2(&temp,rec);
		if(t>ans) ans=t;
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


uint32_t randbytes(void){
	uint32_t r;
#ifdef NORAND
	r = rand();
#else
	if(read(fd,&r,sizeof(uint32_t)) != 4){
		printf("Error getting random number\n");
		exit(1);
	}
#endif
	return r;
}

uint32_t r;
int randsize=0;

void addrand(struct state *s){
	char v,x,y;
	do{
		if(randsize==0){
			r = randbytes();
			randsize = 32;
		}
		else r = r >> 4;
		x = r&3;
		y = (r&12) >> 2;
		randsize -= 4;
	} while(s->arr[x][y] != 0);

	while(1){
		if(randsize==0){
			r = randbytes();
			randsize = 32;
		}
		else r = r >> 4;
		int t = r & 0xF;
		randsize -= 4;
		if(x<9) v=1;
		else if(x<10) v=2;
		else{
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
		char rec = reclist[s.score];
//		char rec = 3;
		
		double top = -102;
		struct state *best = NULL;

		dir[0] = dir[1] = dir[2] = dir[3] = s;
		if(moveup(&(dir[0]))){
			double t = go2(&(dir[0]), rec);
			if(t>top){
				top=t;
				best=&(dir[0]);
			}
		}
		if(moveright(&(dir[1]))){
			double t = go2(&(dir[1]), rec);
			if(t>top){
				top=t;
				best=&(dir[1]);
			}
		}
		if(movedown(&(dir[2]))){
			double t = go2(&(dir[2]), rec);
			if(t>top){
				top=t;
				best=&(dir[2]);
			}
		}
		if(moveleft(&(dir[3]))){
			double t = go2(&(dir[3]), rec);
			if(t>top){
				top=t;
				best=&(dir[3]);
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
