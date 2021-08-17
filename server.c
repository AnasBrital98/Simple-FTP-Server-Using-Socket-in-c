#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char** argv)
{
    struct sockaddr_in address;
    int socketDes,clientDes , result = 0,socketLenght,FileLenght,bytesSended=0,SEND_SIZE=64000;
    char buffer[10],fileName[20],temp[30],*fileTemp;
    FILE *file;
    
    memset(&address,'0',sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(atoi(argv[1]));
    
    if((socketDes = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
    	fprintf(stderr,"Failed to create The Socket ... \n");
    	exit(1);
    } 	   
    
    if((result = bind(socketDes,(struct sockaddr*)&address,sizeof(address))) == -1)
    {
    	fprintf(stderr,"Failed to bind ... \n");
    	exit(1);
    }
    
    if(listen(socketDes,5) == -1)
    {
    	fprintf(stderr,"Failed to listen ... \n");
    	exit(1);
    }
    socketLenght = sizeof(address);
    if((clientDes = accept(socketDes,(struct sockaddr*)&address,&socketLenght)) == -1)
    {
    	fprintf(stderr,"Failed to accept The new Client ... \n");
    	exit(1);
    }
    
    while(1)
    {
    	if(read(clientDes,buffer,sizeof(buffer)) == -1)
    	{
    		fprintf(stderr,"Failed to Receive the Client Choise ... \n");
    		exit(1);
    	}
    	
    	switch(atoi(buffer))
    	{
    		case 1:
    			dup2(clientDes,STDOUT_FILENO);
    			system("ls .");
    			break;
    		case 2:
    			if(read(clientDes,fileName,sizeof(fileName)) == -1)
    			{
    				fprintf(stderr,"Failed to Receive the File Name ... \n");
    				exit(1);	
    			}
    			sprintf(temp,"rm %s",fileName);
    			system(temp);
    			break;
    		case 3: 
    			if(read(clientDes,fileName,sizeof(fileName)) == -1)
    			{
    				fprintf(stderr,"Failed to Receive the File Name ... \n");
    				exit(1);	
    			}
    			
    			if((file = fopen(fileName,"r")) == NULL)
    			{
    				fprintf(stderr,"File Not Found ... \n");
    				exit(1);
    			}
    			
    			fseek(file,0,SEEK_END);
    			FileLenght = ftell(file);
    			fseek(file,0,SEEK_SET);
    			
    			sprintf(buffer,"%d",FileLenght);
    			
    			
    			if((result = send(clientDes,buffer,sizeof(buffer),0)) == -1)
    			{
    				fprintf(stderr,"Failed to send The File Lenght ... \n");
    				exit(1);
    			}
    			
    			fileTemp = (char*)malloc(SEND_SIZE*sizeof(char));
    			while(1)
    			{
    				if(FileLenght - bytesSended < SEND_SIZE)
    				{
    					SEND_SIZE = FileLenght-bytesSended;
    					fileTemp = realloc(fileTemp,SEND_SIZE);	
    				}
    				
    				if((result = fread(fileTemp,1,SEND_SIZE,file)) == -1)
    				{
    					fprintf(stderr,"Failed to read from The File ... \n");
    					exit(1);
    				}
    				
    				if((result = send(clientDes,fileTemp,SEND_SIZE,0)) == -1)
    				{
    					fprintf(stderr,"Failed to send The File ... \n");
    					exit(1);
    				}
    				
    				bytesSended += result;
    				if(FileLenght == bytesSended)
    				{
    					SEND_SIZE = 64000;
    					printf("bytes sended : %d , File Lenght : %d \n",bytesSended,FileLenght);
    					fclose(file);
    					printf("File Sended successfully ... \n");
    					break;
    				}
    			}
    					
    			break;		    		   	
    	}
    
    }
    
   
    close(clientDes);
    close(socketDes);    
return 0;
}    
