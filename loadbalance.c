/*************************************************************************
	> File Name: loadbalance.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2016年10月11日 星期二 23时28分31秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<sys/epoll.h>
#include<assert.h>
#define MAXFD 60000

static int nump = 0;
typedef struct my_args{
	int fd;
	int sockfd;
}my_args;

void fds_add(int epfd,int fd){
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev) == -1){
		perror("epoll_add error\n");
	}
}

void fds_del(int epfd,int fd){
	if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL) == -1){
		perror("epoll_del error\n");
	}
}

int conser1(){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(1500);
	saddr.sin_addr.s_addr = inet_addr("192.168.0.132");

	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res == -1){
		printf("链接服务器错误\n");		
		return 0;
	}
	

	return sockfd;
}
int conser2(){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(1600);
	saddr.sin_addr.s_addr = inet_addr("192.168.0.132");

	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res == -1){
		printf("链接服务器错误\n");		
		return 0;
	}

	return sockfd;
}
int conser3(){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(1700);
	saddr.sin_addr.s_addr = inet_addr("192.168.0.132");

	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res == -1){
		printf("链接服务器错误\n");		
		return 0;
	}
	return sockfd;
}

int my_bind(){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);

	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;	
	saddr.sin_port = htons(1200);
	saddr.sin_addr.s_addr = inet_addr("192.168.0.132");	

	int res = bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);
	
	return sockfd;
}
void *dowork(void *args){
	my_args *p = (struct my_args*)args;
	int epfd = epoll_create(MAXFD);
	assert(epfd != -1);
	struct epoll_event fds[MAXFD];

	struct sockaddr_in saddr,caddr;

	listen(p->sockfd,5);
	
	fds_add(epfd,p->sockfd);
	while(1){
		int n = epoll_wait(epfd,fds,MAXFD,-1);
		if( n == -1){perror("epoll  wait error\n");}
		else if( n == 0){perror("time our\n");}
		else {
			int i = 0;
			for(;i < n;++i){
				if(fds[i].events & EPOLLIN){
					if(fds[i].data.fd == p->sockfd){
					    socklen_t  len = sizeof(caddr);
						int c = accept(p->sockfd,(struct sockaddr*)&caddr,&len);
						if(c < 0){
							perror("accept error\n");
							printf("num~~~~~~~~~~~~~~~~~~~~~~~~~~~%d\n",nump+1);
							continue;
						}
						fds_add(epfd,c);
						printf("new client accept     already connet number is %d\n",nump);
						nump++;
					}
				
					else{
					char buff[255];
					memset(buff,0,255);
					int tmp;
					tmp = recv(fds[i].data.fd,buff,255,0);
					if(tmp <= 0){
						fds_del(epfd,fds[i].data.fd);
						close(fds[i].data.fd);
						printf("one client over\n");
					}
					send(p->fd,buff,tmp,0);
			     	} 
				}
			}
		}
	}
}
void *dowork_1(void *args){
	my_args *p = (struct my_args*)args;
	int epfd = epoll_create(MAXFD);
	assert(epfd != -1);
	struct epoll_event fds[MAXFD];

	struct sockaddr_in saddr,caddr;

	listen(p->sockfd,5);
	
	fds_add(epfd,p->sockfd);
	while(1){
		int n = epoll_wait(epfd,fds,MAXFD,-1);
		if( n == -1){perror("epoll  wait error\n");}
		else if( n == 0){perror("time our\n");}
		else {
			int i = 0;
			for(;i < n;++i){
				if(fds[i].events & EPOLLIN){
					if(fds[i].data.fd == p->sockfd){
					    socklen_t  len = sizeof(caddr);
						int c = accept(p->sockfd,(struct sockaddr*)&caddr,&len);
						if(c < 0){
							perror("accept error\n");
							printf("num~~~~~~~~~~~~~~~~~~~~~~~~~~~%d\n",nump+1);
							continue;
						}
						fds_add(epfd,c);
						printf("new client accept     already connet number is %d\n",nump);
						nump++;
					}
				
					else{
					char buff[255];
					memset(buff,0,255);
					int tmp;
					tmp = recv(fds[i].data.fd,buff,255,0);
					if(tmp <= 0){
						fds_del(epfd,fds[i].data.fd);
						close(fds[i].data.fd);
						printf("one client over\n");
					}
					send(p->fd,buff,tmp,0);
			     	} 
				}
			}
		}
	}
}
void *dowork_2(void *args){
	my_args *p = (struct my_args*)args;
	int epfd = epoll_create(MAXFD);
	assert(epfd != -1);
	struct epoll_event fds[MAXFD];

	struct sockaddr_in saddr,caddr;

	listen(p->sockfd,5);
	
	fds_add(epfd,p->sockfd);
	while(1){
		int n = epoll_wait(epfd,fds,MAXFD,-1);
		if( n == -1){perror("epoll  wait error\n");}
		else if( n == 0){perror("time our\n");}
		else {
			int i = 0;
			for(;i < n;++i){
				if(fds[i].events & EPOLLIN){
					if(fds[i].data.fd == p->sockfd){
					    socklen_t  len = sizeof(caddr);
						int c = accept(p->sockfd,(struct sockaddr*)&caddr,&len);
						if(c < 0){
							perror("accept error\n");
							printf("num~~~~~~~~~~~~~~~~~~~~~~~~~~~%d\n",nump+1);
							continue;
						}
						fds_add(epfd,c);
						printf("new client accept     already connet number is %d\n",nump);
						nump++;
					}
				
					else{
					char buff[255];
					memset(buff,0,255);
					int tmp;
					tmp = recv(fds[i].data.fd,buff,255,0);
					if(tmp <= 0){
						fds_del(epfd,fds[i].data.fd);
						close(fds[i].data.fd);
						printf("one client over\n");
					}
					send(p->fd,buff,tmp,0);
			     	} 
				}
			}
		}
	}
}
int main(){
	pthread_t thread1_id;
	pthread_t thread2_id;
	pthread_t thread3_id;
	my_args args1;
	my_args args2;
	my_args	args3;

	int tmp = my_bind();

	int fd1 = conser1();
	int fd2 = conser2();
	int fd3 = conser3();
	if(fd1 != 0){
		args1.fd = fd1;
		args1.sockfd = tmp;
		pthread_create(&thread1_id,NULL,dowork,&args1);
//		pthread_exit(NULL);
//		pthread_join(thread1_id,NULL);
	}
	if(fd2 != 0){
		args2.fd = fd2;
		args2.sockfd = tmp;
		pthread_create(&thread2_id,NULL,dowork_1,&args2);
//		pthread_exit(NULL);
//		pthread_join(thread2_id,NULL);
	}
	if(fd3 != 0){
		args3.fd = fd3;
		args3.sockfd = tmp;
		pthread_create(&thread3_id,NULL,dowork_2,&args3);
//		pthread_exit(NULL);
//		pthread_join(thread3_id,NULL);
	}

	printf("***************************\n");
		pthread_join(thread1_id,NULL);		
		pthread_join(thread2_id,NULL);
		pthread_join(thread3_id,NULL);
	printf("***************************\n");
	return 0;
}
