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

