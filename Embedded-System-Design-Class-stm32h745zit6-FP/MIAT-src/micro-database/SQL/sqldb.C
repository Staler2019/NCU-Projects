
#include <graphics.h>
#include<stdio.h>
#include<conio.h>
#include<dos.h>
#include<math.h>
#include <stdlib.h>
#include<time.h>
#include <ctype.h>
#include <bios.h>
#define TRUE 1
#define LEFT 1
#define FALSE 0
#define buff 30
#define null 0
/******************************************************************/

/************************************************************************************************/
typedef struct
{
  char name[buff];

  int typenum;
  int signnum;
  struct ntype *right;
  struct nsign *down;
}file;

typedef struct ntype
{
  char name[buff];
  char style;
  int num;
  struct ntype *right;
}type;

typedef struct nsign
{
  int i;
  struct nrecord *right;
  struct nsign *down;
}sign;

typedef struct nrecord
{
  void *data;
  struct nrecord *right;
}record;
file head;
file head1,head2;
int mend;
int stack[1024][2],sr=0;
/***********************************************************/
modify()
{
  type *t1,*s1;
  sign *t2,*s2;
  record *t3,*s3;
  char ch,*t;
  int i,j,k;
   clrscr();
  cleardevice();
  t=malloc(buff);
  t1=head.right;
  for (i=0;i<head.typenum;i++)
  {
    s1=t1;
    t1=t1->right;
  }
  t1=malloc(sizeof(type));	/*申请一个新的空间，以存放新的结点*/
  t1->right=NULL;
  s1->right=t1;			/*将新的接点接在s1后面*/
  printf("\nInput the type's name:  ");
  gets(t);  strcpy(t1->name,t);/*给新接点负值*/
  printf("\nSelect type:\n1   int.\n2   char.\nInput:  ");
  do
  {
    ch=bioskey(0);   printf("%c",ch);/*ch=bioskey(0),读取键盘值*/
    switch(ch)
    {
      case'1':
	t1->style='i';
	break;
      case'2':
	t1->style='c';
	break;
      default:
	printf("\nInput error!\nInput another:  ");
	break;
    }
  }while(ch!='1' && ch!='2');
  if (ch=='1')
    t1->num=2;
  else
  {
    printf("\nInput the length:  ");
    scanf("%d",&(t1->num));getchar();
  }
  head.typenum++;

  t2=head.down;
  for (i=0;i<head.signnum;i++)
  {
    t3=t2->right;
    s3=t3;
    for (j=0;j<head.typenum-1;j++)
    {
      s3=t3;
      t3=t3->right;
    }
    t3=malloc(sizeof(record));
    t3->right=NULL;
    s3->right=t3;
    if (t1->style=='i')
    {
      t3->data=malloc(t1->num);
      printf("\n< %d >%-10s:  ",t2->i,t1->name);
      scanf("%d",t3->data);getchar();
    }
    if (t1->style=='c')
    {
      do
      {
	printf("\n< %d >%-10s:  ",t2->i,t1->name);
	gets(t);
	printf("%d",strlen(t));
	printf("--%d",t1->num);
      }while(strlen(t)>t1->num || strlen(t)<=0);
      t3->data=malloc(t1->num);
      strcpy(t3->data,t);
    }
    t2=t2->down;
  }
  mend=1;
}
/*************************************************************/
void insert()
{
  int i,j,tnum;
  sign *t2,*s2;
  type *t1,*s1;
  record *t3,*s3;
  char *t,ch;
  t=malloc(buff);
  do
  {
    list();
    printf("\nInput the sign's number you want to insert:  ");
    scanf("%d",&tnum);getchar();
    if (tnum>=head.signnum)
    {
      printf("\Error!");
      sleep(1);
      return;
    }
    t1=head.right;
    s2=malloc(sizeof(sign));
    s2->right=NULL;
    s2->down=NULL;
    t2=head.down;
    for (i=0;i<tnum-1;i++)
      t2=t2->down;
    if (tnum==0)
    {
      s2->down=head.down;
      head.down=s2;
      s2->i=tnum;
    }
    else
    {
      s2->down=t2->down;
      t2->down=s2;
      s2->i=tnum;
    }
    t2=s2;
    head.signnum++;
    for (i=tnum;i<head.signnum;i++,t2=t2->down)
      t2->i=i;
    for (j=0;j<head.typenum;j++)
    {
      s3=malloc(sizeof(record));
      s3->right=NULL;
      if (t1==head.right)
      {
	s2->right=s3;
	t3=s3;
      }
      else
      {
	t3->right=s3;
	t3=s3;
      }
      if (t1->style=='i')
      {
	s3->data=malloc(t1->num);
        printf("\n%-10s:  ",t1->name);
	scanf("%d",s3->data);getchar();
      }
      if (t1->style=='c')
      {
	do
	{
          printf("\n%-10s:  ",t1->name);
	  gets(t);
	  printf("%d",strlen(t));
	  printf("--%d",t1->num);
	}while(strlen(t)>t1->num || strlen(t)<=0);
	s3->data=malloc(t1->num);
	strcpy(s3->data,t);
      }
      t1=t1->right;
    }
    printf("\nDo you want to insert another one?\nanswer:  ");
    ch=bioskey(0);   printf("%c",ch);
  }while(ch!='n' && ch!='N');
  mend=1;
  printf("\n\nPress any key back to the MENU.\n");
  bioskey(0);
}
/************************************************************************************/
void use()
{
  int i,j,count;
  FILE *fp;
  type *t1,*s1;
  sign *t2,*s2;
  record *t3,*s3;
  char ch,*t;
  clrscr();
  cleardevice();
  if (mend==1)
  {
    printf("\nDo you want to save the last table?\nAnswer:  ");
    ch=bioskey(0);   printf("%c",ch);
    if (ch=='y' ||  ch=='Y')
      save();
  }
  printf("\nInput the table's name:  ");
  t=malloc(buff);gets(t);
  strcpy(head.name,t);
  if ((fp=fopen(head.name,"rb"))==NULL)
  {
    printf("\ncannot create this file.");
    sleep(1);
    return;
  }
  fread(&head.name[buff],sizeof(head.name[buff]+1),1,fp);

  fread(&head.typenum,sizeof(head.typenum),1,fp);
  fread(&head.signnum,sizeof(head.signnum),1,fp);
  head.right=NULL;  head.down=NULL;
  for (i=0;i<head.typenum;i++)
  {
    s1=malloc(sizeof(type));
    s1->right=NULL;
    fread(&s1->name,sizeof(t1->name)+1,1,fp);
    s1->style=fgetc(fp);
    fread(&s1->num,sizeof(t1->num),1,fp);
    if (head.right==NULL)
    {
      head.right=s1;
      t1=s1;
    }
    else
    {
      t1->right=s1;
      t1=s1;
    }
  }
  for (i=0;i<head.signnum;i++)
  {
    t1=head.right;
    s2=malloc(sizeof(sign));
    s2->right=NULL;
    s2->down=NULL;
    if (head.down==NULL)
    {
      head.down=s2;
      t2=s2;
      count=0;
      s2->i=count;
    }
    else
    {
      t2->down=s2;
      t2=s2;
      count++;
      s2->i=count;
    }
    for (j=0;j<head.typenum;j++)
    {
      s3=malloc(sizeof(record));
      s3->right=NULL;
      if (t1==head.right)
      {
	s2->right=s3;
	t3=s3;
      }
      else
      {
	t3->right=s3;
	t3=s3;
      }
      if (t1->style=='i')
      {
	s3->data=malloc(t1->num);
	fread(s3->data,t1->num,1,fp);
      }
      if (t1->style=='c')
      {
	s3->data=malloc(t1->num);
	fread(s3->data,t1->num,1,fp);
      }
      t1=t1->right;
    }
  }
  mend=0;
  fclose(fp);
}
/***********************************************************/
void delete()
{
  int i,tnum,count;
  sign *t2,*s2;
  type *t3,*s3;
clrscr();
  cleardevice();
  printf("\nInput the number you want to delete:  ");
  scanf("%d",&tnum);getchar();
  if (tnum>=head.signnum)
  {
    printf("\n< %d > is not exist.",tnum);
    return;
  }
  if (tnum==0)
    head.down=head.down->down;
  else
  {
    t2=head.down;
    s2=t2->down;
    while(s2->i!=tnum)
    {
      t2=s2;
      s2=s2->down;
    }
    t2->down=s2->down;
  }
  head.signnum--;
  
  t2=head.down;
  for (i=0;i<head.signnum;i++,t2=t2->down)
    t2->i=i;
  mend=1;
}
/***********************************************************/
 save()
{
  FILE *fp;
  type *t1;
  sign *t2;
  record *t3;
  int i,j;

  remove(head.name);
  if ((fp=fopen(head.name,"wb+"))==NULL)
  {
    printf("\ncannot open this file.\n");
    sleep(1);
    return;
  }
  fwrite(&head.name[buff],sizeof(head.name[buff]+1),1,fp);
 
  fwrite(&head.typenum,sizeof(head.typenum),1,fp);
  fwrite(&head.signnum,sizeof(head.signnum),1,fp);
  t1=head.right;
  for (i=0;i<head.typenum;i++)
  {
    fwrite(t1->name,sizeof(t1->name)+1,1,fp);
    fputc(t1->style,fp);
    fwrite(&t1->num,sizeof(t1->num),1,fp);
    t1=t1->right;
  }
  t2=head.down;
  for (i=0;i<head.signnum;i++)
  {
    t1=head.right;
    t3=t2->right;
    for (j=0;j<head.typenum;j++)
    {
      if (t1->style=='i')
	fwrite(t3->data,t1->num,1,fp);
      if (t1->style=='c')
	fwrite(t3->data,t1->num,1,fp);
      t1=t1->right;
      t3=t3->right;
    }
    t2=t2->down;
  }
  mend=0;
  fclose(fp);
}
/***********************************************************/
void liststru()
{
  int i;
  type *t1;
  clrscr();
cleardevice();
  printf("\nfile name				");
  printf("\n%-40ls",head.name);
  printf("\nname			type		length");
  t1=head.right;
  for (i=0;i<head.typenum;i++)
  {
    printf("\n%-25s%3c%18d",t1->name,t1->style,t1->num);
    t1=t1->right;
  }
  printf("\n\nPress any key back to the MENU.\n");
  bioskey(0);
}
/************************************************************/
void indata()
{
  int i,j;
  sign *t2,*s2;
  type *t1,*s1;
  record *t3,*s3;
  char *t,ch;
  clrscr();
 cleardevice();
  if (head.typenum==0)
  {
    printf("\nThere is no type in the new table.");
    sleep(1);
    return;
  }
  t=malloc(buff);
  s2=head.down;
  t2=s2;
  while (s2!=NULL)
  {
    t2=s2;
    s2=s2->down;
  }
  do
  {
    t1=head.right;
    s2=malloc(sizeof(sign));
    s2->right=NULL;
    s2->down=NULL;
    if (head.down==NULL)
    {
      head.down=s2;
      t2=s2;
      head.signnum=1;
      s2->i=head.signnum-1;
    }
    else
    {
      t2->down=s2;
      t2=s2;
      head.signnum++;
      s2->i=head.signnum-1;
    }

    for (j=0;j<head.typenum;j++)
    {
      s3=malloc(sizeof(record));
      s3->right=null;
      if (t1==head.right)
      {
	s2->right=s3;
	t3=s3;
      }
      else
      {
	t3->right=s3;
	t3=s3;
      }
      if (t1->style=='i')
      {
	s3->data=malloc(t1->num);
        printf("\n%-10s:  ",t1->name);
	scanf("%d",s3->data);getchar();
      }
      if (t1->style=='c')
      {
	do
	{
          printf("\n%-10s:  ",t1->name);
	  gets(t);
	  printf("%d",strlen(t));
	  printf("--%d",t1->num);
	}while(strlen(t)>t1->num || strlen(t)<=0);
	s3->data=malloc(t1->num);
	strcpy(s3->data,t);
      }
      t1=t1->right;
    }
    printf("\nDo you want to input another one?\nanswer:  ");
    ch=bioskey(0);   printf("%c",ch);
  }while(ch!='n' && ch!='N');
  mend=1;
  printf("\n\nPress any key back to the MENU.\n");
  bioskey(0);
}



/************************************************************/
void init()
{

  strcpy(head.name,"new.rec");
 
  mend=0;
  head.typenum=0;
  head.signnum=0;
  head.right=NULL;
  head.down=NULL;
}

/***********************************************************/
void create()
{
  FILE *fp;
  type *t1,*s1;
  char ch,*t;
  clrscr();
 cleardevice();
  if (mend==1)
  {
    printf("\nDo you want to save the last table?\nAnswer:  ");
    ch=bioskey(0);   printf("%c",ch);
    if (ch=='y' ||  ch=='Y')
      save();
  }
  printf("\nInput the table's name:  ");
  t=malloc(buff);gets(t);
  strcpy(head.name,t);
  if ((fp=fopen(head.name,"wb+"))==NULL)
  {
    printf("\ncannot create this file.");

    sleep(1);
    return;
  }

  head.typenum=0;  head.signnum=0;
  head.right=NULL;  head.down=NULL;
  do
  {
    s1=malloc(sizeof(type));
    s1->right=NULL;
    if (head.right==NULL)
    {
      head.right=s1;
      t1=s1;
    }
    else
    {
      t1->right=s1;
      t1=s1;
    }
    printf("\nInput the type's name:  ");
    gets(t);
    strcpy(s1->name,t);
    printf("\nSelect type:\n1   int.\n2   char.\nInput:  ");
    do				
    {
      ch=bioskey(0);   printf("%c",ch);
      switch(ch)
      {
	case'1':
	  s1->style='i';
	  break;
	case'2':
	  s1->style='c';
	  break;
	default:
	  printf("\nInput error!\nInput another:  ");
	  break;
      }
    }while(ch!='1' && ch!='2');
    if (ch=='1')
      s1->num=2;
    else
    {
      printf("\nInput the length:  ");
      scanf("%d",&(s1->num));getchar();
    }
    head.typenum++;
    printf("\nDo you want input another:  ");
    ch=bioskey(0);   printf("%c",ch);
  }while(ch!='n' && ch!='N');           

  fwrite(&head.name[buff],sizeof(head.name[buff]+1),1,fp);
 
  fwrite(&head.typenum,sizeof(head.typenum),1,fp);
  fwrite(&head.signnum,sizeof(head.signnum),1,fp);

  t1=head.right;
  while(t1!=NULL)
  {
    fwrite(t1->name,sizeof(t1->name)+1,1,fp);
    fputc(t1->style,fp);
    fwrite(&t1->num,sizeof(t1->num),1,fp);
    t1=t1->right;
  }
  fclose(fp);
  printf("\n\nPress any key back to the MENU.\n");
  bioskey(0);
}


/**************************************************************/


mainbulider2()
{
int key2;
char ch;
newplay2();
key2=corss2();
while(key2!=11)
	{
	switch(key2)
	{
case 1:
create();
newplay2();
key2=corss2();

break;
	case 2:
indata();
newplay2();

key2=corss2();
break;
	case 3:
list();
	newplay2();
key2=corss2();
break;
	case 4:
liststru();
	newplay2();
key2=corss2();
break;
	case 5:
 delete();
	newplay2();
key2=corss2();
break;
	case 6:
		modify();
	newplay2();
key2=corss2();

break;
	case 7:
insert();
	newplay2();
key2=corss2();
break;
	case 8:
create();
	newplay2();
key2=corss2();
break;
	case 9:
create();
	newplay2();
key2=corss2();
break;
	case 10:
use();
	newplay2();
key2=corss2();
break;
	case 11:


if (mend==1)
  {
  clrscr();
  cleardevice();
    printf("\nDo you want to save 111 last table?\nAnswer:  ");
    ch=bioskey(0);   printf("%c",ch);
    if (ch=='y' ||  ch=='Y')
      save();
  }
getch();


	break;

	case 12:
if (mend==1)
  {
  clrscr();
  cleardevice();
    printf("\nDo you want to save the last table?\nAnswer:  ");
    ch=bioskey(0);   printf("%c",ch);
    if (ch=='y' ||  ch=='Y')
      save();
  }
getch();


exit(0);
break;

	}
}
}
/*****************************************************************/
 list()
{
  int i,j,*k1,count=0;
  char *k2;
  type *t1;
  sign *t2,*s2;
  record *t3,*s3;
   clrscr();
 cleardevice();
  t1=head.right;
  printf("\nnum");
  for (i=0;i<head.typenum;i++)
  {
    printf("%10s",t1->name);
    t1=t1->right;
  }
  t2=head.down;
  for (i=0;i<head.signnum;i++)
  {
    t1=head.right;
    t3=t2->right;
    printf("\n%3d",count++);
    for (j=0;j<head.typenum;j++)
    {
      if (t1->style=='i')
      {
	k1=t3->data;
	printf("%10d",*k1);
      }
      if (t1->style=='c')
      {
	k2=t3->data;
	printf("%10s",k2);
      }
      t1=t1->right;
      t3=t3->right;
    }
    t2=t2->down;
  }
  printf("\n\nPress any key back to the MENU.\n");
  bioskey(0);
}

/*****************************************************************/
corss2()
{
int button,oldx,oldy;
int key;
int x ,y,First=TRUE;
time_t ww;
button=oldx=oldy=0;
 key=0;

setwritemode(XOR_PUT);

setcolor(15);
while(key!=1&&key!=2&&key!=3&&key!=4&&key!=5&&key!=6&&key!=7&&key!=8
&&key!=9&&key!=10&&key!=11&&key!=12)
{
readmouse(&button,&x,&y);

if(oldx!=x||oldy!=y||button==LEFT)
{
if(!First)
	{line(oldx,oldy,oldx,oldy+5);
	line(oldx,oldy,oldx+5,oldy);
	line(oldx,oldy,oldx+10,oldy+10);
	}
line(x,y,x,y+5);
line(x,y,x+5,y);
line(x,y,x+10,y+10);
oldx=x;
oldy=y;
First=FALSE;
if(button==LEFT&&x>220&&x<300&&y>80&&y<120)
	key=1;
else if(button==LEFT&&x>220&&x<300&&y>140&&y<180)
	key=3;
else if(button==LEFT&&x>220&&x<300&&y>200&&y<240)
	key=5;
else if(button==LEFT&&x>220&&x<300&&y>260&&y<300)
	key=7;
else if(button==LEFT&&x>220&&x<300&&y>320&&y<360)
	key=9;
else if(button==LEFT&&x>220&&x<300&&y>380&&y<420)
	key=11;
else if(button==LEFT&&x>320&&x<400&&y>80&&y<120)
	key=2;
else if(button==LEFT&&x>320&&x<400&&y>140&&y<180)
	key=4;
else if(button==LEFT&&x>320&&x<400&&y>200&&y<240)
	key=6;
else if(button==LEFT&&x>320&&x<400&&y>260&&y<300)
	key=8;
else if(button==LEFT&&x>320&&x<400&&y>320&&y<360)
	key=10;
else if(button==LEFT&&x>320&&x<400&&y>380&&y<420)
	key=12;
else key=0;
}

}

line(x,y,x,y+5);
line(x,y,x+5,y);
line(x,y,x+10,y+10);
setwritemode(COPY_PUT);
return key;
}
/*****************************************************************/

corss1()
{
int button,oldx,oldy;
int key;
int x ,y,First=TRUE;
time_t ww;
button=oldx=oldy=0;
 key=0;

setwritemode(XOR_PUT);
/*hidemouse();*/
setcolor(15);
while(key!=1&&key!=2&&key!=3&&key!=4&&key!=5&&key!=6&&key!=7)
{
readmouse(&button,&x,&y);

if(oldx!=x||oldy!=y||button==LEFT)
{
if(!First)
	{line(oldx,oldy,oldx,oldy+5);
	line(oldx,oldy,oldx+5,oldy);
	line(oldx,oldy,oldx+10,oldy+10);
	}
line(x,y,x,y+5);
line(x,y,x+5,y);
line(x,y,x+10,y+10);
oldx=x;
oldy=y;
First=FALSE;
if(button==LEFT&&x>250&&x<355&&y>80&&y<120)

	key=1;

else if(button==LEFT&&x>250&&x<355&&y>140&&y<180)

		key=2;


else if(button==LEFT&&x>250&&x<355&&y>200&&y<240)

	key=3;


else if(button==LEFT&&x>250&&x<355&&y>260&&y<300)

	key=4;


else if(button==LEFT&&x>250&&x<355&&y>320&&y<360)

	key=5;



else key=0;
}

}

line(x,y,x,y+5);
line(x,y,x+5,y);
line(x,y,x+10,y+10);
setwritemode(COPY_PUT);

return key;



}
/****************************************************************/




/**************************************************************/
main()
{
/*welcome(); */
init();
mainbulider();



}
welcome()
{
int ballx=220,bally=300,i;
  int dr=DETECT,md;
  initgraph(&dr,&md,"D:\TC");
  cleardevice();
  setbkcolor(BLUE);
  setcolor(RED);
  settextstyle(1,0,4);
  outtextxy(150,150,"WELCOME MY DBMS");
  setcolor(WHITE);
  settextstyle(3,0,1);
  outtextxy(300,400,"DESIGE BY YUGAOCHAO!");
  outtextxy(300,420,"PRESS ANY KEY TO RUN!");
   while(ballx<450)
   { setcolor(GREEN);
    outtextxy(ballx-12,bally,">");

      ballx+=3;
		     }
    ballx=220;
  while(!kbhit())
  {
	  if(ballx==421)
      {
      setcolor(GREEN);
      outtextxy(ballx+12,bally,">");
	   outtextxy(ballx+6,bally,">");
	    outtextxy(ballx,bally,">");
	   outtextxy(ballx-6,bally,">");
}
   if (ballx<420)
      ballx+=3;
    else
      ballx=220;
	setcolor(WHITE);
    outtextxy(ballx-6,bally,">");
	 setcolor(GREEN);
    outtextxy(ballx-9,bally,">");
	/********************************************************/
    setcolor(0);
    outtextxy(ballx,bally,">");
	 setcolor(GREEN);
    outtextxy(ballx-3,bally,">");

	/********************************************************/
	setcolor(RED);
    outtextxy(ballx+6,bally,">");
	 setcolor(GREEN);
    outtextxy(ballx+3,bally,">");

	/********************************************************/
	setcolor(0);
    outtextxy(ballx+12,bally,">");
	 setcolor(GREEN);
    outtextxy(ballx+9,bally,">");

    for (i=0;i<100;i++)
      delay(500);
  }
  bioskey(0);
  closegraph();
   }
 quit()
{
closegraph();
exit(0);
}

/*************************************************************/
int initmouse()
{
union REGS inr,outr;
inr.x.ax=0;
int86(0x33,&inr,&outr);
return outr.x.ax;

}
/**********************************************************************/
 showmouse()
{
	union REGS inr,outr;
	inr.x.ax=1;
	int86(0x33,&inr,&inr);

}
/*********************************************************************/
ErrMsg()
{
	printf("\rNO Mouse Error!");
	getch();
	quit();
}
/************************************************************************/
/****************************************************************/
readmouse(int *f,int *x,int *y)
{
union REGS inr,outr;
inr.x.ax=3;
int86(0x33,&inr,&outr);
*f=outr.x.bx;
*x=outr.x.cx;
*y=outr.x.dx;
}
/******************************************************************/
/*******************************************************************/
newplay1()
{
int i;
cleardevice();

setcolor(3);
for(i=0;i<5;i++)
{
rectangle(250,80+i*60,355,120+i*60);

}
for(i=0;i<5;i++)
	{
		setfillstyle(6,SOLID_FILL)  ;
		floodfill(300,100+i*60,3);
	}
settextstyle(1,0,4);
setcolor(4);
outtextxy(265,85,"DDL");
outtextxy(265,145,"DML");
outtextxy(265,205,"DCL");
outtextxy(265,265,"HELP");
outtextxy(255,325,"QDBMS");
}
/*******************************************************************/
newplay2()
{
int i;
setcolor(3);
cleardevice();
for(i=0;i<6;i++)
{
rectangle(220,80+i*60,300,120+i*60);

}
for(i=0;i<6;i++)
	{
		setfillstyle(1,3)  ;
		floodfill(250,100+i*60,3);
	}
for(i=0;i<6;i++)
{
rectangle(320,80+i*60,400,120+i*60);

}
for(i=0;i<6;i++)
	{
		setfillstyle(1,3)  ;
		floodfill(350,100+i*60,3);
	}
settextstyle(1,0,2);
setcolor(4);
outtextxy(230,90,"CREAT");
outtextxy(330,90,"INDATA");
outtextxy(230,150,"LIST");
outtextxy(230,210,"DETE");

outtextxy(230,270,"INSERT");
setcolor(8);
outtextxy(230,330,"HLPE");
setcolor(4);
outtextxy(330,150,"STUR");
setcolor(8);
outtextxy(325,210,"MODIFY");
setcolor(4);
setcolor(8);
outtextxy(330,270,"8");
setcolor(4);
outtextxy(320,330,"OLDDATA");

outtextxy(230,390,"BACK");
outtextxy(330,390,"QUIT");
/*settextstyle(1,0,4);
setcolor(5);
outtextxy(265,85,"DDL");
outtextxy(265,145,"DML");
outtextxy(265,205,"DCL");
outtextxy(265,265,"HELP");
outtextxy(255,325,"QDBMS");*/
}
/*******************************************************************/
/******************************************************************/
mainbulider()
{
int key1;


int dr=DETECT,md;
  initgraph(&dr,&md,"D:\TC");
  cleardevice();
newplay1();

if(!initmouse())
	ErrMsg();

showmouse();


	key1=corss1();
	delay(300);
while(key1!=5)
	{
	switch(key1)
	{
case 1:mainbulider2();
newplay1();
key1=corss1();
break;
	case 2:mainbulider2();

key1=corss1();
break;
	case 3:
	mainbulider2();

key1=corss1();break;
	case 4:mainbulider2();

key1=corss1();break;
	case 5:
	exit(0);

break;

	}
	}

getchar();
quit();
}