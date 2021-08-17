# Simple FTP Server Using Socket in c and Linux

This Repository contains a simple FTP Server implemented in Linux Operating System Using Socket .

<div align="center" >
<img src="ftp.png" width="350" height="100" >
</div>

The Client can list The Content of the working Directory in The Server or remove a file or download a specific File From The Server .

### Client :

```c
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
    if(argc < 3)
    {
    	fprintf(stderr,"Not Enough Parameters ... \n");
    	exit(1);
    }

    struct sockaddr_in address;
    int socketDes , result,choix,SEND_SIZE=64000,bytesReceived=0,FileLenght;
    char buffer[10],message[2048],*fileTemp;
    FILE *file;
    
    memset(&address,'0',sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);
    address.sin_port = htons(atoi(argv[2]));
    
    if((socketDes = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
    	fprintf(stderr,"Failed to create The Socket ... \n");
    	exit(1);
    }		

    if((result = connect(socketDes,(struct sockaddr*)&address,sizeof(address))) == -1)
    {
    	fprintf(stderr,"Failed to Connect to The Server ... \n");
    	exit(1);
    }	
    
    while(1)
    {
    	printf("	Choose one of the available options :  \n");
    	printf("1 - display The Content of the server  .\n");
    	printf("2 - remove a file in The Server .\n");
    	printf("3 - download a File from The Server . \n");
    	printf("4 - Quit .\n");
    	printf("> ");
    	scanf("%d",&choix);
    	switch(choix)
    	{
    		case 1 : 
    			strcpy(buffer,"1");
    			if(send(socketDes,buffer,sizeof(buffer),0) == -1)
		    	{
			    	fprintf(stderr,"Failed to send The client Choise  ... \n");
			    	exit(1);
		    	}
			printf("the Content of the working folder in the server is : \n");
		    	if((result = read(socketDes,message,sizeof(message))) == -1)
		    	{
		    		fprintf(stderr,"Failed to receive The Result ... \n");
			    	exit(1);
		    	}
		    	printf("%s\n",message);	
		    	  	
    			break;
    		case 2 : 
    			strcpy(buffer,"2");
    			if(send(socketDes,buffer,sizeof(buffer),0) == -1)
		    	{
			    	fprintf(stderr,"Failed to send The client Choise ... \n");
			    	exit(1);
		    	}
    			printf("Enter The Name of the File :  ");
    			scanf("%s",message);
    			if(send(socketDes,message,sizeof(message),0) == -1)
		    	{
			    	fprintf(stderr,"Failed to send The File Name  ... \n");
			    	exit(1);
		    	}
    			break;	
    		case 3 : 
    			strcpy(buffer,"3");
    			if(send(socketDes,buffer,sizeof(buffer),0) == -1)
		    	{
			    	fprintf(stderr,"Failed to send The client Choise ... \n");
			    	exit(1);
		    	}
    			printf("Enter The Name of the File :  ");
    			scanf("%s",message);
    			if(send(socketDes,message,sizeof(message),0) == -1)
		    	{
			    	fprintf(stderr,"Failed to send The File Name  ... \n");
			    	exit(1);
		    	}
		    	
		    	if(read(socketDes,buffer,sizeof(buffer)) == -1)
		    	{
		    		fprintf(stderr,"Failed to received The File Lenght  ... \n");
			    	exit(1);
		    	}
		    	
		    	FileLenght = atoi(buffer);
		    	
		    	fileTemp = (char*)malloc(SEND_SIZE*sizeof(char));
		    	if((file = fopen(message,"w+")) == NULL)
		    	{
		    		fprintf(stderr,"Failed to Open The File  ... \n");
			    	exit(1);
		    	}
		    	
		    	while(1)
		    	{	
		    		if(FileLenght - bytesReceived < SEND_SIZE)
		    		{
		    			SEND_SIZE = FileLenght - bytesReceived;
		    			fileTemp = realloc(fileTemp,SEND_SIZE);
		    		}
		    		    	
		    		if((result = read(socketDes,fileTemp,sizeof(fileTemp))) == -1)
		    		{
		    			fprintf(stderr,"Failed to received The File  ... \n");
			    		exit(1);
		    		}
		    		
		    		bytesReceived += result;
		    		
		    		if((result = fwrite(fileTemp,1,sizeof(fileTemp),file)) == -1)
		    		{
		    			fprintf(stderr,"Failed to write in The File  ... \n");
			    		exit(1);
		    		}
		    		
		    		if(FileLenght == bytesReceived)
		    		{
		    			printf("File Received Successfully ... \n");
		    			printf("%d bytes received , %d File Lenght \n",bytesReceived,FileLenght);
		    			fclose(file);
		    			break;
		    		}
		    		
		    	}
		    	
    			break; 
    		case 4:
    			return 0;
    			break;		   	
    		default : printf("Please Choose One of The available Options ... \n");
    	}
    }

return 0;
}


```


### Server :

```c
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

```



