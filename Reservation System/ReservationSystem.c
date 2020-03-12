#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct flight
{
	int number;
	char dep_city[15];
	char ar_city[15];
	char dep_day[15];
	char dep_time[15];
	char ar_day[15];
	char ar_time[15];
} *pf;

struct passenger
{
	char name[15];
	char surname[15];
	char dep_city[15];
	char ar_city[15];
	char dep_day[15];
	char dep_time[15];
	int number;
} *pp;

void search_fNum(int number, int N, struct flight *pf)
{
	int i=0;
	int flag=1;

			for(i=0; i<N; i++)
			{
				if(number==pf[i].number)
				{
					printf("%d %s %s %s %s %s %s\n",pf[i].number, pf[i].dep_city, pf[i].ar_city,
										 			  pf[i].dep_day, pf[i].dep_time, pf[i].ar_day, pf[i].ar_time);
					flag=0;
				}
			}
			if(flag)
				printf("No flight with number %d\n",number);
}

void search_dep(char dep[15], int N, struct flight *pf)
{
	int i=0;
	int flag=1;
	
	for(;i<N;i++)
	{
		if(!strcmp(dep,pf[i].dep_city))
		{
			printf("%d %s %s %s %s %s %s\n",pf[i].number, pf[i].dep_city, pf[i].ar_city,
										 pf[i].dep_day, pf[i].dep_time, pf[i].ar_day, pf[i].ar_time);
			flag=0;
		}
	}
	if(flag)
		printf("No departure city %s\n",dep);
}

void search_ar(char ar[15], int N, struct flight *pf)
{
	int i=0;
	int flag=1;
	
	for(;i<N;i++)
	{
		if(!strcmp(ar,pf[i].ar_city))
		{
			printf("%d %s %s %s %s %s %s\n",pf[i].number, pf[i].dep_city, pf[i].ar_city,
										 pf[i].dep_day, pf[i].dep_time, pf[i].ar_day, pf[i].ar_time);
			flag=0;
		}
	}
	if(flag)
			printf("No arrival city %s\n",ar);
}

void print_fNum(int number, struct flight *pf, struct passenger *pp, int N, int M)
{
	int i=0,count=0;
	int j=0;
	
	for(i=0; i<N; i++)
	{
		if(number==pf[i].number)
			break;
	}
	
	for(j=0; j<M; j++)
	{
		if(number==pp[j].number)
			count++;
	}
	
	printf("%d %s %s %s %s %s %s %d\n",number, pf[i].dep_city, pf[i].ar_city, pf[i].dep_day, pf[i].dep_time,
										pf[i].ar_day, pf[i].ar_time, count);	
}

void print_fNumwP(int number, struct flight *pf, struct passenger *pp, int N, int M)
{
	int i=0,count=0;
	int j=0;
	
	for(i=0; i<N; i++)
	{
		if(number==pf[i].number)
			break;
	}
	
	for(j=0; j<M; j++)
	{
		if(number==pp[j].number)
			count++;
	}
	
	printf("%d %s %s %s %s %s %s %d\n",number, pf[i].dep_city, pf[i].ar_city, pf[i].dep_day, pf[i].dep_time,
										pf[i].ar_day, pf[i].ar_time, count);
	for(j=0; j<M; j++)
	{
		if(number==pp[j].number)
			printf("%s %s\n",pp[j].name, pp[j].surname);
	}
}

void print_R(char name[15], char surname[15],int M, int N, struct flight *pf, struct passenger *pp)
{
	int i=0, j=0, temp;
	
	for(; i<M; i++)
	{
		if(!strcmp(name, pp[i].name))
			if(!strcmp(surname, pp[i].surname))
			{
				temp = pp[i].number;
				break;
			}
	}
	
	for(j=0; j<N; j++)
	{
		if(temp == pf[j].number)
			printf("%s %s %d %s %s %s %s %s %s\n", name, surname, temp, pf[j].dep_city, pf[j].ar_city,
													pf[j].dep_day, pf[j].dep_time, pf[j].ar_day, pf[j].ar_time);
	}
}

int main()
{
	int N,M;
	scanf("%d",&N);
	int count=0;
	int flag,i;
	
	pf = (struct flight *) malloc (sizeof(struct flight)*N);
	while(count<N)
	{
		scanf("%d %s %s %s %s %s %s",&pf[count].number, pf[count].dep_city, pf[count].ar_city,
								pf[count].dep_day, pf[count].dep_time, pf[count].ar_day,
								pf[count].ar_time);
		count++;
	}
	
	scanf("%d",&M);

	pp = (struct passenger *) malloc (sizeof(struct passenger)*M);
	count=0;
	while(count<M)
	{
		scanf("%s%s%s%s%s",pp[count].name, pp[count].surname, pp[count].dep_city, pp[count].ar_city, pp[count].dep_day);

		flag=1;
		for(i=0; i<N; i++)
		{
			if(!strcmp(pp[count].dep_city,pf[i].dep_city))
			{
				if(!strcmp(pp[count].ar_city, pf[i].ar_city))
				{
					if(!strcmp(pp[count].dep_day, pf[i].dep_day))
					{
						if(!flag)
							if(strcmp(pp[count].dep_time,pf[i].dep_time)==-1)
								continue;
						
						pp[count].number = pf[i].number;
						strcpy(pp[count].dep_time, pf[i].dep_time);
						flag=0;
					}
				}
			}
		}
		if(flag)
			printf("%s %s %s %s %s No reservation\n",pp[count].name, pp[count].surname, pp[count].dep_city, pp[count].ar_city,
													pp[count].dep_day);
		count++;
	}
	
	
	int t;
	char arr[15];
	char temp_ch[15];
	while(1)
	{
		scanf("%s",arr);
		if(!strcmp(arr, "END"))
			break;
		printf("\n");
		if(!strcmp(arr,"search"))
		{
			scanf("%s",arr);
			if(!strcmp("flightNumber",arr))
			{
				scanf("%d",&t);
				search_fNum(t, N, pf);
			}
			else if(!strcmp("departure",arr))
			{
				scanf("%s",arr);
				search_dep(arr, N, pf);
			}
			else if(!strcmp("arrival",arr))
			{
				scanf("%s",arr);
				search_ar(arr, N, pf);
			}
		}
		else if(!strcmp(arr, "print"))
		{
			scanf("%d",&t);
			print_fNum(t,pf,pp,N,M);
		}
		else if(!strcmp(arr, "printwP"))
		{
			scanf("%d",&t);
			print_fNumwP(t,pf,pp,N,M);
		}
		else if(!strcmp(arr, "printR"))
		{
			scanf("%s",arr);
			scanf("%s",temp_ch);
			print_R(arr, temp_ch, M, N, pf, pp);
		}
	}
	
return 0;
}
