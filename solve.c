#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define NORAND

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

struct state{
	unsigned int arr[4][4];
	int score;
};

const char reclist[] = {5,5,4,4,3,2,2,2,2,2,2,2,1,1,1,1};
const char extralist[]={2,1,2,1,1,2,2,1,1,1,1,1,2,2,2,2};

int fd;

void print(const struct state* restrict s){
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

bool moveup(const struct state* restrict in, struct state* restrict out){
	char c,i,j,next;
	bool canmix = false;
	char newscore = in->score;
	bool ans = false;
	for(i = 0; i < 4; i++){
		canmix=false;
		c = 0;
		for(j = 0; j < 4; j++){
			next = in->arr[i][j];
			if(next == 0) continue;
			if(canmix && out->arr[i][c-1] == next){
				out->arr[i][c-1] = next+1;
				canmix = false;
				newscore++;
				ans=true;
			} else {
				out->arr[i][c] = next;
				if(c != j) ans=true;
				canmix = true;
				c++;
			}
		}
	}
	out->score=newscore;
	return ans;
}
bool moveleft(const struct state* restrict in, struct state* restrict out){
	char c,i,j,next;
	bool canmix = false;
	char newscore = in->score;
	bool ans = false;
	for(i = 0; i < 4; i++){
		canmix=false;
		c = 0;
		for(j = 0; j < 4; j++){
			next = in->arr[j][i];
			if(next == 0) continue;
			if(canmix && out->arr[c-1][i] == next){
				out->arr[c-1][i] = next+1;
				canmix = false;
				newscore++;
				ans=true;
			} else {
				out->arr[c][i] = next;
				if(c != j) ans=true;
				canmix = true;
				c++;
			}
		}
	}
	out->score=newscore;
	return ans;
}
bool movedown(const struct state* restrict in, struct state* restrict out){
	char c,i,j,next;
	bool canmix = false;
	char newscore = in->score;
	bool ans = false;
	for(i = 0; i < 4; i++){
		canmix=false;
		c = 0;
		for(j = 0; j < 4; j++){
			next = in->arr[i][3-j];
			if(next == 0) continue;
			if(canmix && out->arr[i][4-c] == next){
				out->arr[i][4-c] = next+1;
				canmix = false;
				newscore++;
				ans=true;
			} else {
				out->arr[i][3-c] = next;
				if(c != j) ans=true;
				canmix = true;
				c++;
			}
		}
	}
	out->score=newscore;
	return ans;
}
bool moveright(const struct state* restrict in, struct state* restrict out){
	char c,i,j,next;
	bool canmix = false;
	char newscore = in->score;
	bool ans = false;
	for(i = 0; i < 4; i++){
		canmix=false;
		c = 0;
		for(j = 0; j < 4; j++){
			next = in->arr[3-j][i];
			if(next == 0) continue;
			if(canmix && out->arr[4-c][i] == next){
				out->arr[4-c][i] = next+1;
				canmix = false;
				newscore++;
				ans=true;
			} else {
				out->arr[3-c][i] = next;
				if(c != j) ans=true;
				canmix = true;
				c++;
			}
		}
	}
	out->score=newscore;
	return ans;
}

double go2(struct state* restrict s, unsigned char rec, unsigned char extra);

double go(struct state* restrict s, unsigned char rec, unsigned char extra){
	double ans = 0;

	int i;
	struct state up={0};
	if(moveup(s,&up)){
		ans = go2(&up,rec,extra);
	}
	struct state left={0};
	if(moveleft(s,&left)){
		double t = go2(&left,rec,extra);
		if(t>ans) ans=t;
	}
	struct state down={0};
	if(movedown(s,&down)){
		double t = go2(&down,rec,extra);
		if(t>ans) ans=t;
	}
	struct state right={0};
	if(moveright(s,&right)){
		double t = go2(&right,rec,extra);
		if(t>ans) return t;
	}
	return ans;
}

double go2(struct state* restrict s, unsigned char rec, unsigned char extra){
	if(rec==0 && extra == 0) return s->score;
	char i,j,c;
	double ans;
	c = 0;
	ans = 0;
	s->score--;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(s->arr[i][j] == 0){
				s->arr[i][j] = 1;
				if(rec!=0){
					double t = 0.9*go(s,rec-1,extra);
					s->arr[i][j] = 2;
					ans += t+0.1*go(s,rec-1,extra);
				} else {
					ans += go(s,rec,extra-1);
				}
				s->arr[i][j] = 0;
				c++;
			}
		}
	}
	s->score++;
	if(c==0) return -100 - rec;
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

void addrand(struct state* restrict s){
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
	
		struct state dir[4]={0};
		
		double top = -100000;
		struct state *best = NULL;

		bool u,d,r,l;
		u = moveup(&s,&(dir[0]));
		r = moveright(&s,&(dir[1]));
		d = movedown(&s,&(dir[2]));
		l = moveleft(&s,&(dir[3]));

		int nextmax = 0;
		if(dir[0].score > nextmax) nextmax = dir[0].score;
		if(dir[1].score > nextmax) nextmax = dir[1].score;
		if(dir[2].score > nextmax) nextmax = dir[2].score;
		if(dir[3].score > nextmax) nextmax = dir[3].score;
		int rec = reclist[nextmax];
		int extra = extralist[nextmax];

		int count = 0;
		if(u) count++;
		if(r) count++;
		if(d) count++;
		if(l) count++;
		if(count == 0){
			printf("No good moves, ending in this state:\n\n\n");
			print(&s);
			return 0;
		}
		if(u){
			if(count>1){
				double t = go2(&(dir[0]), rec, extra);
				if(t>top){
					top=t;
					best=&(dir[0]);
				}
			} else {
				best=&(dir[0]);
			}
		}
		if(r){
			if(count>1){
				double t = go2(&(dir[1]), rec, extra);
				if(t>top){
					top=t;
					best=&(dir[1]);
				}
			} else {
				best=&(dir[1]);
			}
		}
		if(d){
			if(count>1){
				double t = go2(&(dir[2]), rec, extra);
				if(t>top){
					top=t;
					best=&(dir[2]);
				}
			} else {
				best=&(dir[2]);
			}
		}
		if(l){
			if(count>1){
				double t = go2(&(dir[3]), rec, extra);
				if(t>top){
					top=t;
					best=&(dir[3]);
				}
			} else {
				best=&(dir[3]);
			}
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
