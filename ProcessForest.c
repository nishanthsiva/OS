#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int s_tree=0,d_tree=0,s,d, h;
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
	if(s_tree == d_tree){
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
	}else{
		for(i=0;i<s_len;i++){
			path[i] = s_path[i];
		}
		path[i++] = -10;
		for(j=d_len-1;j>=0;j--){
			path[i++] = d_path[j];
		}
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

int nodeInForestPath(int node_index, int tree_index){
	int i=0;
	if(tree_index != s_tree && tree_index != d_tree){
		return 0;
	}
	else if(s_tree == d_tree){
		if(tree_index != s_tree)
			return 0;
		else{
			for(i=0;i<path_size;i++){
				if(path[i] == node_index){
					return 1;
				}
			}
			return 0;
		}
	}else{
		if(tree_index == s_tree){
			for(i=0;path[i] != -10;i++){
				if(path[i] == node_index){
					return 1;
				}
			}
			return 0;
		}else{
			int tree_switch = 0;
			for(i=0;i<path_size;i++){
				if(path[i] == -10){
					tree_switch = 1;
				}
				if(tree_switch == 0){
					continue;
				}
				if(path[i] == node_index){
					return 1;
				}
			}
			return 0;
		}
	}
}

int find_prev_node(int current_node, int tree_index){
	int i=0;
	if(tree_index == s_tree){
		for(i=0;path[i] != -10 && i<path_size;i++){
			if(i!=0){
				if(path[i] == current_node){
					return path[i-1];
				}
			}
		}
		return -1;
	}else{
		int tree_switch = 0;
		for(i=0;i<path_size;i++){
			if(path[i] == -10){
				tree_switch = 1;
			}
			if(tree_switch == 0){
				continue;
			}
			if(i!=0){
				if(path[i] == current_node){
					return path[i-1]; 
				}
			}
		}
		return -1;
	}
	
	return -1;
}

int find_next_node(int current_node, int tree_index){
	int i=0;
	if(tree_index == s_tree){
		for(i=0;path[i] != -10 && i<path_size;i++){
			if(i!=path_size-1){
				if(path[i] == current_node){
					return path[i+1];
				}
			}
		}
		return -1;
	}else{
		int tree_switch = 0;
		for(i=0;i<path_size;i++){
			if(path[i] == -10){
				tree_switch = 1;
			}
			if(tree_switch == 0){
				continue;
			}
			if(i!=(path_size -1)){
				if(path[i] == current_node){
					return path[i+1]; 
				}
			}
		}
		return -1;
	}
	
	return -1;
}

int main(){
	int p,i,n,ab;
	s_tree = 1;d_tree = 1;
	s=7;d=0;h=4;
	getMessagePath();
	printf("Final Path -\t");
	for(i=0;i<path_size;i++){
		printf("%d\t",path[i]);
	}
	//printf("\nIS in PAth - %d\n",nodeInForestPath(6,1));
	int fd[30][2];
	for(i=0;i<30;i++){
		fd[i][0]= -1;
		fd[i][1] = -1;
	}
	pid_t root_pid = getpid();
	printf("\n");
	char readbuffer[50];
	char dest_buffer[50];
	pid_t t1=-1,t2=-1;
	printf("root is %d, t1 is %d, t2 is %d\n",getpid(),t1,t2);
	int parent_index = -10;
	int tree_index = 0;
	int nbytes;
	for(i=1;i<=h;i++){
		t1= fork();
		if(t1!=0){
			t2 = fork();
		}
		if(t1==0 && t2!=0){ // first child is invoked
			if(parent_index == -10){
				parent_index = 0;
			}else{
				parent_index = (parent_index*2)+1;
				//pipe(fd[parent_index]);
			}
			pipe(fd[(tree_index*15)+parent_index]);
			
		}
		if(t2==0 && t1!=0){ // second child is invoked
			if(parent_index == -10){
				parent_index = 0;
				tree_index = 1;
			}else{
				parent_index = (parent_index*2)+2;
			}
			pipe(fd[(tree_index*15)+parent_index]);
		}
		if(t1!=0 && t2!=0){

			break;
		}
		t1=-1;t2=-1;
	}
	
	//printf("PIPES of %d in %d tree - %d & %d\n",parent_index,tree_index,fd[(tree_index*15)+parent_index][0], fd[(tree_index*15)+parent_index][1]);fflush(stdout);
	while(nodeInForestPath(parent_index, tree_index) == 1){
		//printf("Prev Node for %d from Tree %d in Path - %d\n",parent_index,tree_index,find_prev_node(parent_index,tree_index));fflush(stdout);
		/*
		*ignore process with index -10
		*
		*/
		nbytes = 0;
		if(parent_index == s && tree_index == s_tree){
			int next_node = find_next_node(s, s_tree);
			if( next_node == -10){
				//write to shared memory
			}else if(next_node != -1){
				if(next_node < parent_index){ //if next node is parent then write to parent
					write(fd[(15*tree_index)+next_node][1], "I am message", (strlen("I am message")+1));
					printf("Transmitting from source at %d of Tree %d through %d pipe of %d\n",s,tree_index,fd[(15*tree_index)+next_node][1],(15*tree_index)+next_node);fflush(stdout);
					close(fd[(15*tree_index)+next_node][1]);
				}else{ // else write to self
					write(fd[(15*tree_index)+parent_index][1], "I am message", (strlen("I am message")+1));
					printf("Transmitting from source at %d of Tree %d through %d pipe of %d\n",s,tree_index,fd[(15*tree_index)+parent_index][1],(15*tree_index)+parent_index);fflush(stdout);
					close(fd[(15*tree_index)+parent_index][1]);
				}
			}
			
			exit(0);
		}else if(parent_index != d){
			int next_node = find_next_node(parent_index,tree_index);
			int prev_node = find_prev_node(parent_index,tree_index);
			if(prev_node == -10){
				//read from shared mem and write to self
			}
			if(next_node == -10){
				//read from self and write to shared mem
			}
			if(prev_node != -10 && next_node != -10){
				//printf("Preparing to forward at %d\n",parent_index);fflush(stdout);
				if(prev_node < parent_index && next_node > parent_index){//prev is parent and next is child
					nbytes = read(fd[(15*tree_index)+prev_node][0], dest_buffer, (strlen("I am message")+1)*sizeof(char));
					close(fd[(15*tree_index)+prev_node][0]);
					write(fd[(15*tree_index)+parent_index][1], dest_buffer, sizeof(dest_buffer));
					printf("Forwarding at %d of Tree %d\n",parent_index,tree_index);fflush(stdout);
					close(fd[(15*tree_index)+parent_index][1]);
					//printf("Writing --%d bytes-- to %d and Exiting %d\n",nbytes,next_node,parent_index);fflush(stdout);

				}else if(prev_node > parent_index && next_node < parent_index){//prev is child and next is parent
					nbytes = read(fd[(15*tree_index)+parent_index][0], dest_buffer, (strlen("I am message")+1)*sizeof(char));
					close(fd[(15*tree_index)+parent_index][0]);
					write(fd[(15*tree_index)+next_node][1], dest_buffer, sizeof(dest_buffer));
					printf("Forwarding at %d of Tree %d\n",parent_index,tree_index);fflush(stdout);
					close(fd[(15*tree_index)+next_node][1]);
					//printf("Writing --%d bytes-- to %d and Exiting %d\n",nbytes,next_node,parent_index);fflush(stdout);
				}
				exit(0);
			}
			
			
		}else if(parent_index == d && tree_index == d_tree){
			int prev_node = find_prev_node(parent_index, tree_index);
			if(prev_node == -10){
				//read from shared memory
			}else if(prev_node != -1){
				if(prev_node < parent_index){ // if prev node is parent
					nbytes = read(fd[(15*tree_index)+prev_node][0], dest_buffer, sizeof(dest_buffer));
					close(fd[(15*tree_index)+prev_node][0]);
				}else{
					nbytes = read(fd[(15*tree_index)+parent_index][0], dest_buffer, sizeof(dest_buffer));
					close(fd[(15*tree_index)+parent_index][0]);
				}
				printf("Transmission complete at %d of Tree %d with %s\n",parent_index,tree_index,dest_buffer);fflush(stdout);
				exit(0);
			}
		}
	}
	wait(NULL);
	wait(NULL);
	//printf("Exiting %d on %d\n",parent_index, tree_index);
}
