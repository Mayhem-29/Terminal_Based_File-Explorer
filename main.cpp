#include <termios.h>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstddef>
#include <fcntl.h>
#include <stdio.h>
using namespace std;


void disable();
void getsize();
string singleread();
void multikey();
void uparrow();
void downarrow();
void enter();
void leftarrow();
void rightarrow();
void back();
void r_dir();
void to_screen(string);
void thisdir(char[]);
void cmode();
vector<string> parse(string);
int copy_main(vector<string>);
void copy_f(string, string);
void copy_d(string, string);
int move(vector<string>);
int rename(vector<string>);
void go(vector<string>);
int create_file(vector<string>);
int create_dir(vector<string>);
int delete_file(vector<string>);
int delete_dir(vector<string>);
void delete_rec(string);
int search(vector<string>);
string getpath(string);
int search_rec(string, string);

struct termios old;
struct winsize win;

int row;                                       //total number of rows on screen
int col;                                       //total number of columns
int r;                                         //current cursor position on y-axis
int c;                                         //current cursor position on x-axis
int curline = 0;                               //stores the value of first file/folder of current directory on screen
char root_dir[1024];                           //path where the application was launched
char cur_dir[1024];                            //path of current working directory
vector<string> curtable;                       //vector strores names of all the files/folders in current directory
stack<string> backst;                          //stores the path of last visited directories in LIFO manner
stack<string> frontst;                         //stores the path of directories visited after visiting current directory


/* ----------------FUNCTION TO RESTORE TERMINAL WITH OLD SETTINGS,WHILE LEAVING PROGRAM--------------------------------*/

void disable()
{

  r = 0;
  c = 0;
  printf("%c[%d;%dH", 27, r, c);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &old);
}




/*----------------FUNCTION TO GET SIZE OF THE TREMINAL----------------------------------------------------------*/

void getsize()
{
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
  row = win.ws_row;
  col = win.ws_col;
}




/*-----------------READS ONE CHARACTER AT A TIME,USED TO DETECT ESCAPE SEQUENCES----------------------------------*/

string singleread()
{

  int nread;
  char c;
  fflush(0);
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    ;

  if (c == '\x1b')
  {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return "\x1b";
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return "\x1b";
    if (seq[0] == '[')
    {
      switch (seq[1])
      {
      case 'A':
        return "\x1b[A";
      case 'B':
        return "\x1b[B";
      case 'C':
        return "\x1b[C";
      case 'D':
        return "\x1b[D";
      }
    }
    fflush(0);
    return "\x1b";
  }

  else
  {
    int p = c;
    string s = to_string(p);

    //cout<<s;
    //string s(1,c);

    fflush(0);
    return s;
  }
}




/*-------------------DEPENDING ON THE ESCAPE SEQUENCES OR CHARACTERS RECIEVED BY PREVIOUS FUNCTION,ENABLES KEYPRESSES------------*/
void multikey()
{
  string s = singleread();

  if (s == "72" || s == "104")
    r_dir();

  else if (s == "\x1b[A")
    uparrow();
  // printf("%c[%d;%dH", 27, --r, c);

  else if (s == "\x1b[B")
    downarrow();
  //printf("%c[%d;%dH", 27, ++r, c);

  else if (s == "\x1b[C")
    rightarrow();
  //printf("%c[%d;%dH", 27, r, ++c);

  else if (s == "\x1b[D")
    leftarrow();
  //printf("%c[%d;%dH", 27, r, --c);

  else if (s == "127")
  {
    back();
  }

  else if (s == "58")
  {
    cmode();
    thisdir(cur_dir);
  }

  //cout<<"back";

  else if (s == "10")
    enter();

  else if (s == "113" || s == "81")
  {
    disable();
    printf("\033[H\033[J");

    exit(0);
  }

  else
  {
  }
}





/*----------------FUNCTION TO DISSECT THE CWD, STORE ITS CONTENT IN A VECTOR AND LATER PASSING THEM TO DISPLAY-------------------*/
void thisdir(char s[])
{
  curtable.clear();
  DIR *dir;
  struct dirent *dp;
  if ((dir = opendir(s)) == NULL)
  {
    perror("cannot access");
    return;
  }

  else
  {
    r = 1;
    c = 1;
    printf("\033[H\033[J");

    while (1)
    {
      dp = readdir(dir);
      if (dp == NULL)
        break;
      curtable.push_back(dp->d_name);
    }

    getsize();
    int i;
    //cout<<curtable.size();
    //cout<<curline;
    if (curtable.size() <= row - 2)
    {

      for (i = 0; i < curtable.size(); i++)
      {
        to_screen(curtable[i]);
      }
    }

    else
    {
      for (i = 0; i < row - 2; i++)
      {
        to_screen(curtable[i]);
      }
    }

    printf("%c[%d;%dH", 27, row, 1);
    cout << "--NORMAL MODE--";

    r = 1;
    c = 1;

    printf("%c[%d;%dH", 27, 1, c);
  }
}
 




 /*---------------FUNCTION TO NAVIGATE UP AND TAKE CARE OF VERTCAL SCROLLING/OVERFLOW--------------------------------------------*/
void uparrow()
{
  if (r > 1)
  {
    r--;
    printf("%c[%d;%dH", 27, r, c);
  }

  else if (r == 1 && curline == 0)
  {
  }

  else if (r == 1 && curline > 0)

  {

    curline--;
    int i;
    printf("\033[H\033[J");
    for (i = curline; i < curline + row - 2; i++)
    {
      to_screen(curtable[i]);
    }

    printf("%c[%d;%dH", 27, row, 1);
    cout << "--NORMAL MODE--";
    r = 1;
    printf("%c[%d;%dH", 27, r, c);
  }
}





/*------------------FUNCTION TO NAVIGATE UP AND TAKE CARE OF VERTCAL SCROLLING/OVERFLOW-----------------------------------------*/
void downarrow()
{
  if (curtable.size() <= row - 2)
  {
    if (r != curtable.size())
    {
      r++;
      printf("%c[%d;%dH", 27, r, c);
    }
  }
  else

  {
    if (r < row - 2)
    {
      r++;
      printf("%c[%d;%dH", 27, r, c);
    }

    else if (r == row - 2 && curline >= (curtable.size() - row + 2))
    {
    }

    else
    {

      curline++;
      int i;
      printf("\033[H\033[J");
      r = 1;
      for (i = curline; i < curline + row - 2; i++)
      {
        to_screen(curtable[i]);
      }

      printf("%c[%d;%dH", 27, row, 1);
      cout << "--NORMAL MODE--";

      r--;
      printf("%c[%d;%dH", 27, r, c);
    }
  }
}






/*---------------------FUNCTION ENTER A DIRECTORY/FILES,UPON PRESSING ENTER----------------------------------------------*/
void enter()
{

  struct stat st;

  int pos = curline + r - 1;
  if (curtable[pos] == ".")
    return;
  string s = string(cur_dir) + "/" + curtable[pos];
  int len = s.length();
  char temp[len + 1];
  strcpy(temp, s.c_str());
  stat(temp, &st);

  if ((st.st_mode & S_IFMT) == S_IFDIR)
  {
    backst.push(string(cur_dir));

    if (curtable[pos] == "..")
    {

      size_t found;
      string tempc = string(cur_dir);
      found = tempc.find_last_of("/\\");

      tempc = tempc.substr(0, found);
      if (found == 0)
        tempc = "/";
      strcpy(cur_dir, tempc.c_str());
    }

    else
    {
      strcpy(cur_dir, s.c_str());
    }

    while (frontst.size() != 0)
    {
      frontst.pop();
    }
    getsize();
    curline = 0;
    r = 1;
    c = 1;

    printf("%c[%d;%dH", 27, r, c);
    thisdir(cur_dir);
  }

  else

  {

    int pid = fork();
    if (pid == 0)
    {
      execl("/usr/bin/xdg-open", "xdg-open", s.c_str(), NULL);
      exit(1);
    }
  }
}






/*-------------------------FUNCTION TO GO BACK TO PREVIOUSLY VISITED DIRECTORY,UPON PRESSING LEFT-------------------------------*/
void leftarrow()
{

  if (backst.size() == 0)
    return;
  frontst.push(string(cur_dir));
  string go = backst.top();
  backst.pop();
  strcpy(cur_dir, go.c_str());
  getsize();
  curline = 0;
  r = 1;
  c = 1;

  printf("%c[%d;%dH", 27, r, c);
  thisdir(cur_dir);
}






/*-------------------------FUNCTION TO GO TO NEXT DIRECTORY,UPON PRESSING RIGHT------------------------------------------------*/
void rightarrow()
{

  if (frontst.size() == 0)
    return;
  backst.push(string(cur_dir));
  string go = frontst.top();
  frontst.pop();
  strcpy(cur_dir, go.c_str());
  getsize();
  curline = 0;
  r = 1;
  c = 1;

  printf("%c[%d;%dH", 27, r, c);
  thisdir(cur_dir);
}






/*-------------------------FUNCTION TO GO TO PARENT FOLDER OF CWD,UPON PRESSING BACKSPACE---------------------------------------*/
void back()
{ 

  if (strcmp(cur_dir,root_dir) == 0)
   return;
  size_t found;
  string tempc = string(cur_dir);
  found = tempc.find_last_of("/\\");

  tempc = tempc.substr(0, found);
 // if (found == 0)
    //tempc = "/";
  backst.push(string(cur_dir));
  strcpy(cur_dir, tempc.c_str());
  while (frontst.size() != 0)
  {
    frontst.pop();
  }

  getsize();
  curline = 0;
  r = 1;
  c = 1;

  printf("%c[%d;%dH", 27, r, c);
  thisdir(cur_dir);
}






/*--------------------------FUNCTION TO GO TO HOME FOLDER,UPON PRESSING "h"-------------------------------------------------------*/
void r_dir()
{

  while (frontst.size() != 0)
    frontst.pop();

  if (strcmp(root_dir, cur_dir) != 0)
    backst.push(string(cur_dir));

  strcpy(cur_dir, root_dir);
  getsize();
  curline = 0;
  r = 1;
  c = 1;

  printf("%c[%d;%dH", 27, r, c);
  thisdir(cur_dir);
}






/*--------------------------FUNCTION TO DIRECT ALL CONTENTS OF CWD ON THE SCREEN---------------------------------------------------*/
void to_screen(string filename)
{

  string address = string(cur_dir) + "/" + filename;
  //cout<<filename;
  //cout<<"\t\t";
  struct stat info;
  stat(address.c_str(), &info);
  long long len = info.st_size;
  string size = "";
  if (len >= 1073741824)
  {
    size += to_string(len / 1073741824);
    size += " GB";
  }
  else if (len >= 1048576)
  {
    size += to_string(len / 1048576);
    size += " MB";
  }
  else if (len >= 1024)
  {
    size += to_string(len / 1024);
    size += " KB";
  }

  else
  {
    size += to_string(len);
    size += " B";
  }

  string s = "";
  //struct stat info;
  // stat(address.c_str(),&info);

  if (S_ISDIR(info.st_mode))
    s += "d";
  else if (S_ISFIFO(info.st_mode))
    s += "p";
  else if (S_ISLNK(info.st_mode))
    s += "l";
  else if (S_ISBLK(info.st_mode))
    s += "b";
  else if (S_ISSOCK(info.st_mode))
    s += "s";
  else if (S_ISCHR(info.st_mode))
    s += "c";
  else
    s += "-";


  if ((S_IRUSR & info.st_mode))
    s += "r";
  else
    s += "-";

  if ((S_IWUSR & info.st_mode))
    s += "w";
  else
    s += "-";

  if ((S_IXUSR & info.st_mode))
    s += "x";
  else
    s += "-";

  if ((S_IRGRP & info.st_mode))
    s += "r";
  else
    s += "-";

  if ((S_IWGRP & info.st_mode))
    s += "w";
  else
    s += "-";

  if ((S_IXGRP & info.st_mode))
    s += "x";
  else
    s += "-";

  if ((S_IROTH & info.st_mode))
    s += "r";
  else
    s += "-";

  if ((S_IWOTH & info.st_mode))
    s += "w";
  else
    s += "-";

  if ((S_IXOTH & info.st_mode))
    s += "x";
  else
    s += "-";

  string name;
  struct passwd *usr = getpwuid(info.st_uid);
  name += usr->pw_name;
  struct group *grp = getgrgid(info.st_gid);
  name += "\t";
  name += grp->gr_name;

  string time = string(ctime(&info.st_mtime));
  const char *timee = time.substr(4, 12).c_str();

  //cout<<time.length();

  c = 1;
  cout << s;
  cout << "\t";
  //cout<<name;
  //cout<<"\t";
  cout << size;
  //cout<<filename;
  cout << "\t";
  //cout<<time;
  printf(" %-12s ", timee);
  cout << "\t";
  if (S_ISDIR(info.st_mode))
  {
    printf("\x1B[34m");
  }

  cout << filename;
  printf("\x1B[37m");

  //cout<<s;
  r++;
  printf("%c[%d;%dH", 27, r, c);
}








/*----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------COMMAND MODE-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------*/
void cmode()
{
  fflush(0);
  getsize();
  r = row;
  c = 1;
  printf("%c[%d;%dH", 27, r, c);
  cout << "--COMMAND MODE-- ";
  c += 17;
  char ch;
  string com = "";
  while (1)
  {

    ch = cin.get();

    if (ch == 27)
      return;

    if (ch == 113 && com.length() == 0)
    {
      disable();
      printf("\033[H\033[J");

      exit(0);
    }

    else if (ch == 127)
    {
      if (com.length() == 0)
      {
      }

      else
      {
        com.pop_back();
        c--;
        printf("%c[%d;%dH", 27, r, c);

        cout << " ";
        c++;
        c--;
        printf("%c[%d;%dH", 27, r, c);
      }
    }

    else if (ch == '\n')
    {

      // printf("%c[%d;%dH", 27,r, 150);
      //cout<<com.length();
      // printf("%c[%d;%dH", 27,r, c);
      //com.push_back(ch);
      vector<string> res;
      res = parse(com);
      if (res[0] == "copy")
      {
        int ret = copy_main(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " copied!";
        }
        else
        {
          cout << com;
          cout << " ERROR!";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "move")
      {
        int ret = move(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " moved!";
        }
        else
        {
          cout << com;
          cout << " ERROR!";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "rename")
      {
        int ret = rename(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " renamed!";
        }
        else
        {
          cout << com;
          cout << " ERROR!";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "goto")
      {
        go(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        cout << com;
        cout << " sucessful";

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "create_file")
      {
        int ret = create_file(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " created!";
        }
        else
        {
          cout << com;
          cout << " ERROR!";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "create_dir")
      {
        int ret = create_dir(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " created!";
        }
        else
        {
          cout << com;
          cout << " ERROR!";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "delete_file")
      {
        int ret = delete_file(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " deleted!";
        }
        else
        {
          cout << com;
          cout << " ERROR!";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "delete_dir")
      {
        int ret = delete_dir(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " Deleted!";
        }
        else
        {
          cout << com;
          cout << " ERROR";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else if (res[0] == "search")
      {
        int ret = search(res);
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        if (ret == 1)
        {
          cout << com;
          cout << " TRUE!";
        }
        else
        {
          cout << com;
          cout << " FALSE";
        }

        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }

      else
      {
        printf("%c[%d;%dH", 27, row - 1, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- WRONG COMMAND!!";
        printf("%c[%d;%dH", 27, row, 1);
        printf("\x1b[2K");
        cout << "--COMMAND MODE-- ";
        r = row;
        c = 18;
        printf("%c[%d;%dH", 27, r, c);
        com = "";
      }
    }

    else
    {
      cout << ch;
      c++;
      com.push_back(ch);
    }
  }

  return;
}






/*-----------------------------FUNCTION TO PARSE THE COMMAND AND STORE THE CONTENT IN VECTOR--------------------------------------------*/
vector<string> parse(string s)
{
  vector<string> res;
  int len = s.length();
  int i = 0;
  string temp = "";
  while (i != len)
  {
    if (s[i] != ' ')
    {
      temp.push_back(s[i]);
      i++;
    }

    else
    {
      res.push_back(temp);
      temp = "";
      i++;
    }
  }
  res.push_back(temp);

  return res;
}






/*----------------------------------------------------FUNCTION TO GET FULL PATH----------------------------------------------------------*/
string getpath(string s)
{

  string res = "";

  if (s[0] == '~')
  {
    s.replace(0, 1, string(root_dir));
  }

  else if (s[0] == '.')
  {
    s.replace(0, 1, string(cur_dir));
  }

  else if (s[0] == '/')
  {
    s = string(root_dir) + s;
  }
  else
  {
    s = string(cur_dir) + "/" + s;
  }

  return s;
}







/*-------------------------------------------------------RENAME COMMAND------------------------------------------------------------------*/
int rename(vector<string> s)
{

  if (s.size() == 3)
  {
    string a = string(cur_dir) + "/" + s[1];
    string b = string(cur_dir) + "/" + s[2];
    rename(a.c_str(), b.c_str());
    return 1;
  }
  return -1;
}







/*------------------------------------------------------------GOTO COMMAND------------------------------------------------------------------*/
void go(vector<string> s)
{
  string des = getpath(s[1]);
  backst.push(string(cur_dir));
  strcpy(cur_dir, des.c_str());
  thisdir(cur_dir);

  return;
}






/*--------------------------------------------------------------CREATE A FILE----------------------------------------------------------------*/
int create_file(vector<string> s)
{
  int len = s.size();
  string des = getpath(s[len - 1]);
  int status = creat((des + '/' + s[1]).c_str(), O_RDONLY | O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (status == -1)
    return -1;

  return 1;
}






/*-------------------------------------------------------------CREATE A DIRECTORY-------------------------------------------------------------*/
int create_dir(vector<string> s)
{
  int len = s.size();
  string des = getpath(s[len - 1]);
  des += "/";
  des += s[1];
  if (mkdir(des.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1)
    return 1;

  return -1;
}






/*------------------------------------------------------------DELETE A FILE---------------------------------------------------------------------*/
int delete_file(vector<string> s)
{
  string des = getpath(s[1]);
  //printf("%c[%d;%dH", 27,row-3, 1);

  //cout<<des;

  if (remove(des.c_str()) == -1)
    return -1;
  return 1;
}






                                              /*---------DELETE A DIRECTORY---------------*/
void delete_rec(string address)
{
  DIR *d;
  d = opendir(address.c_str());
  struct dirent *dir;
  //printf("%c[%d;%dH", 27,row-3, 1);
  while (1)
  {
    dir = readdir(d);
    if (dir == NULL)
      break;
    if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
    {
      struct stat info;

      string pathextend = address + "/" + dir->d_name;
      //cout<<pathextend;
      if (stat(pathextend.c_str(), &info) == -1)
        return;
      if (S_ISDIR(info.st_mode))
      {
        delete_rec(pathextend);
      }

      else
      {
        remove(pathextend.c_str());
      }
    }
  }
  closedir(d);
  rmdir(address.c_str());

  return;
}



int delete_dir(vector<string> s)
{
  string path = getpath(s[1]);
  delete_rec(path);

  return 1;
}







                                               /*-----------SEARCH A FILE/DIRECTORY-----------*/
int search_rec(string address, string file)
{
  DIR *d;
  if ((d = opendir(address.c_str())) == NULL)
    return -1;
  struct dirent *dir;
  while (1)
  {
    dir = readdir(d);
    if (dir == NULL)
      break;
    if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
    {
      struct stat info;
      string pathextend = address + "/" + dir->d_name;
      if (stat(pathextend.c_str(), &info) == -1)
        return -1;
      if (strcmp(file.c_str(), dir->d_name) == 0)
        return 1;
      if (S_ISDIR(info.st_mode))
      {
        if (strcmp(dir->d_name, file.c_str()) == 0)
          return 1;
        else
        {
          if (search_rec(pathextend, file.c_str()) == 1)

            return 1;
        }
      }

      else
      {
        if (strcmp(file.c_str(), dir->d_name) == 0)
          return 1;
      }
    }
  }
  return -1;
}



int search(vector<string> s)
{
  string address = string(cur_dir);
  int i = search_rec(address, s[1]);

  return i;
}







                                              /*-------------COPY A FILE/FOLDER---------------*/
int copy_main(vector<string> s)
{
  if (s.size() < 3)
  {
    return -1;
  }
  int len = s.size();

  string dest = getpath(s[len - 1]);
  struct stat info;
  int i;
  for (i = 1; i < len - 1; i++)
  {
    string path = string(cur_dir) + "/" + s[i];
    dest = dest + "/" + s[i];
    lstat(path.c_str(), &info);
    if (S_ISDIR(info.st_mode))
    {

      copy_d(path, dest);
    }

    else
    {
      copy_f(path, dest);
    }
  }

  return 1;
}



void copy_f(string filepath, string dest)
{
  FILE *src, *des;
  char c;

  if ((src = fopen(filepath.c_str(), "r")) == NULL)
    return;

  if ((des = fopen(dest.c_str(), "w")) == NULL)
    return;
  
  char ch;
  ch = getc(src);

  while (ch != EOF)
  {
    putc(ch, des);
    ch = getc(src);
  }

  struct stat info;
  stat(filepath.c_str(), &info);
  chown(dest.c_str(), info.st_uid, info.st_gid);
  chmod(dest.c_str(), info.st_mode);
  fclose(src);
  fclose(des);
}

void copy_d(string filepath, string dest)
{
  mkdir(dest.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  DIR *d;
  d = opendir(filepath.c_str());
  struct dirent *dir;
  while (1)
  {
    dir = readdir(d);
    if (dir == NULL)
      break;
    if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
    {
      struct stat info;
      string spath = filepath + "/" + dir->d_name;
      string dpath = dest + "/" + dir->d_name;
      if (stat(spath.c_str(), &info) == -1)
        return;
      if (S_ISDIR(info.st_mode))
      {
        copy_d(spath, dpath);
      }
      else
      {
        copy_f(spath, dpath);
      }
    }
  }
}








                                               /*--------------MOVE A FILE/FOLDER---------------*/
int move(vector<string> s)
{
  if (s.size() < 3)
  {
    return -1;
  }
  int len = s.size();

  string dest = getpath(s[len - 1]);
  struct stat info;
  int i;
  for (i = 1; i < len - 1; i++)
  {
    string path = string(cur_dir) + "/" + s[i];
    dest = dest + "/" + s[i];
    lstat(path.c_str(), &info);
    if (S_ISDIR(info.st_mode))
    {

      copy_d(path, dest);
      delete_rec(path);
    }

    else
    {
      copy_f(path, dest);
      remove(path.c_str());
    }
  }

  return 1;
}







/*----------------------------------------------MAIN FUNCTION------------------------------------------------------------------*/
int main()
{
  printf("\033[H\033[J");
  r = 1;
  c = 1;
  curline = 0;
  //getcwd(root_dir, sizeof(root_dir));
  char* pPath=getenv("HOME");
  strcpy(root_dir,pPath);
  //getenv("HOME");

  getcwd(cur_dir, sizeof(cur_dir));
  thisdir(cur_dir);

  struct termios nw;
  tcgetattr(STDIN_FILENO, &old);
  tcgetattr(STDIN_FILENO, &nw);

  nw.c_lflag &= ~(ECHO);
  nw.c_lflag &= ~(ICANON);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &nw);
  getsize();

  while (1)
  {
    multikey();
  }

  atexit(disable);

  return 0;
}
