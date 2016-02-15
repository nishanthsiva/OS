#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>

int s,d, h;
int path_size;
int path[100];

void getMessagePath(){
	int temp_s = s;
	int temp_d = d;
	int s_path[h];
	int d_path[h];
	int temp_path[2*h];
	int j=0,i=0;
	int s_len=0, d_len=0;
	s_path[s_len++] = s;
	while(temp_s >= 0){
		if(temp_s == 0){
			break;
		}else{
			if(temp_s%2 == 0){
				temp_s = (temp_s - 2)/2;
			}
			else{
				temp_s = (temp_s - 1)/2;
			}
			s_path[s_len++] = temp_s;
		}
	}
	d_path[d_len++] = d;
	while(temp_d >= 0){
		if(temp_d == 0){
			break;
		}else{
			if(temp_d%2 == 0){
				temp_d = (temp_d - 2)/2;
			}
			else{
				temp_d = (temp_d - 1)/2;
			}
			d_path[d_len++] = temp_d;
		}
	}
	int merge_point[2];
	int merged = 0;
	for(j=0;j<d_len;j++){
		for(i=0;i<s_len;i++){
			if(d_path[j] == s_path[i]){
				merge_point[0]=i;
				merge_point[1]=j;
				merged = 1;
				break;
			}
		}
		if(merged){
			break;
		}
	}
	i=0;j=0;
	for(i=0;i<=merge_point[0];i++){
		path[i] = s_path[i];
	}
	for(j=merge_point[1]-1;j>=0;j--){
		path[i++] = d_path[j];
	}
	path_size = i;
	printf("\n");
}

int nodeInPath(int node_index){
	int i=0;
	for(i=0;i<path_size;i++){
		//printf("checking %d\t",path[i]);
		if(path[i] == node_index){
			return 1;
		}
	}
	return 0;
}

int find_prev_node(int current_node){
	int i=0;
	for(i=0;i<path_size;i++){
		if(i!=0){
			if(path[i] == current_node){
				return path[i-1]; 
			}
		}
	}
	return -1;
}

int find_next_node(int current_node){
	int i=0;
	for(i=0;i<path_size;i++){
		if(i!=(path_size -1)){
			if(path[i] == current_node){
				return path[i+1]; 
			}
		}
	}
	return -1;
}

int main(int argc, char* argv[]){
	int p,i,n,ab;
	h = (int) strtol(argv[1], (char **)NULL, 10);
	h=h+1;
	char *m = malloc(sizeof(argv[2]));
	strcpy(m,argv[2]);
	s = (int) strtol(argv[3], (char **)NULL, 10);
	d = (int) strtol(argv[4], (char **)NULL, 10);
	//s=7;d=8;h=4;
	getMessagePath();
	printf("Final Path -\t");
	for(i=0;i<path_size;i++){
		printf("%d\t",path[i]);
	}
	int pipe_count = (int) (pow(2,h) -1);
	int fd[pipe_count][2];
	for(i=0;i<pipe_count;i++){
		fd[i][0]= -1;
		fd[i][1] = -1;
	}
	pid_t root_pid = getpid();
	printf("\n");
	char readbuffer[100];
	char dest_buffer[100];
	pid_t t1=-1,t2=-1;
	//printf("root is %d, t1 is %d, t2 is %d\n",getpid(),t1,t2);
	int parent_index = 0;
	int nbytes;
	pipe(fd[0]);
	for(i=1;i<=(h-1);i++){
		t1= fork();
		if(t1!=0){
			t2 = fork();
		}
		if(t1==0 && t2!=0){ // first child is invoked
			parent_index = (parent_index*2)+1;
			pipe(fd[parent_index]);
			
		}
		if(t2==0 && t1!=0){ // second child is invoked
			parent_index = (parent_index*2)+2;
			pipe(fd[parent_index]);
		}
		if(t1!=0 && t2!=0){

			break;
		}
		t1=-1;t2=-1;
	}
	//printf("PIPES of %d - %d & %d\n",parent_index,fd[parent_index][0], fd[parent_index][1]);fflush(stdout);
	while(nodeInPath(parent_index) == 1){
		//printf("entering while %d\n",parent_index);fflush(stdout);
		nbytes = 0;
		if(parent_index == s){
			int next_node = find_next_node(s);
			if(next_node < parent_index){ //if next node is parent then write to parent
				write(fd[next_node][1], m, (strlen(m)+1));
				printf("Transmitting from source at %d\n",s);fflush(stdout);
				close(fd[next_node][1]);
			}else{ // else write to self
				write(fd[parent_index][1], m, (strlen(m)+1));
				printf("Transmitting from source at %d\n",s);fflush(stdout);
				close(fd[parent_index][1]);
			}
			exit(0);
		}else if(parent_index != d){
			int next_node = find_next_node(parent_index);
			int prev_node = find_prev_node(parent_index);
			//printf("Preparing to forward at %d\n",parent_index);fflush(stdout);
			if(prev_node < parent_index && next_node > parent_index){//prev is parent and next is child
				nbytes = read(fd[prev_node][0], dest_buffer, 100);
				close(fd[prev_node][0]);
				write(fd[parent_index][1], dest_buffer, sizeof(dest_buffer));
				printf("Forwarding at %d\n",parent_index);fflush(stdout);
				close(fd[parent_index][1]);
				//printf("Writing --%d bytes-- to %d and Exiting %d\n",nbytes,next_node,parent_index);fflush(stdout);

			}else if(prev_node > parent_index && next_node < parent_index){//prev is child and next is parent
				nbytes = read(fd[parent_index][0], dest_buffer, 100);
				close(fd[parent_index][0]);
				write(fd[next_node][1], dest_buffer, sizeof(dest_buffer));
				printf("Forwarding at %d\n",parent_index);fflush(stdout);
				close(fd[next_node][1]);
				//printf("Writing --%d bytes-- to %d and Exiting %d\n",nbytes,next_node,parent_index);fflush(stdout);
			}else if(prev_node > parent_index && next_node > parent_index){
				nbytes = read(fd[parent_index][0], dest_buffer, 100);
				printf("Forwarding at %d\n",parent_index);fflush(stdout);
				write(fd[parent_index][1], dest_buffer, sizeof(dest_buffer));
			}
			exit(0);
			
		}else if(parent_index == d){
			int prev_node = find_prev_node(parent_index);
			if(prev_node < parent_index){ // if prev node is parent
				nbytes = read(fd[prev_node][0], dest_buffer, sizeof(dest_buffer));
				close(fd[prev_node][0]);
			}else{
				nbytes = read(fd[parent_index][0], dest_buffer, sizeof(dest_buffer));
				close(fd[parent_index][0]);
			}
			printf("Transmission complete at %d with %s\n",parent_index,dest_buffer);fflush(stdout);
			exit(0);
			
		}
	}
	wait(NULL);
	wait(NULL);
	//printf("Exiting %d\n",parent_index);
}
