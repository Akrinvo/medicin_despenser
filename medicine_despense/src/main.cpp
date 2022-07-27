#include<ros/ros.h>
#include<medicine_despense/medicine.h>
#include<vector>
#include<iostream>

#include <stdio.h>
#include <string.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include<thread>

using namespace std;

vector<string>med_list;
bool moveNext = true;
vector<string> new_list = {"hello","brother","boy", "and","mat","world","desh","iconoclastic"};
int  serial_port = open("/dev/ttyACM1", O_RDWR);
struct termios tty;

char readbuffer[256];
//char writebuffer[256];
char lastbuffer[256];
int lastbuffer_length = 0;






bool serialSetup()
{


 if(tcgetattr(serial_port, &tty) != 0) {
       printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
             return 1;
   }
  //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_cflag |= CREAD | CLOCAL;

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ECHONL;
  tty.c_lflag &= ~ISIG;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
  tty.c_oflag &= ~OPOST;
  tty.c_oflag &= ~ONLCR;
  tty.c_cc[VTIME] = 5;
  tty.c_cc[VMIN] = 0;
  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }

 


 // if (num_bytes < 0) {
   //   printf("Error reading: %s", strerror(errno));
  //    return 1;
 // }

  return 0;

}




// // std::vector<uint8_t> msg = {'H','\n'};
//   unsigned char msg[] = {"Z M 1000"};

//   write(serial_port, &msg[0], sizeof(msg));

//   std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//   char read_buf [256];
//   int num_bytes = 0;

// //close(serial_port);
// //////////////////////////SERIAL OVER ///////////////////////////////
// char buff[256];
// while(true)
// {
//     read(serial_port , &buff, sizeof(buff));
//     for(auto x : buff)
//         cout << buff << endl;



// return 0;
// }


















void callback(const medicine_despense::medicine::ConstPtr &msg)
{
    
       int med_size = msg->medicine_list.size();
       cout << med_size << endl;

       
            for(int i = 0; i < med_size ; i++)
            {
                med_list.push_back(msg->medicine_list[i]);
                cout << "appended" << endl;
                //cout << msg->medicine_list[i] << endl;


            }
            med_list.push_back("H");


}

bool trying = false;

void tryagain()
{
    vector<uint8_t> last_msg(lastbuffer , lastbuffer+lastbuffer_length+1);
    write(serial_port,last_msg.data(),last_msg.size());
    trying = true;
    



}


void readSerial()
{
    while(true)
    {
    read(serial_port,&readbuffer[0],sizeof(readbuffer));
   // cout << readbuffer[0] << endl;

    if(readbuffer[0] == 'R')
    {

        moveNext = true;
        cout << "cleared" << endl;
        trying = false;
        //cout << med_list.size() << endl;


    }

    else if(readbuffer[0] == 'E'  && trying == true)
    {   
        //vector<uint8_t> last_msg(lastbuffer , lastbuffer+lastbuffer_length+1);
        //write(serial_port,last_msg.data(),last_msg.size());
        
        moveNext = true;
        trying = false;
        //moveNext = true;

    }


    else if(readbuffer[0] == 'E'  && trying == false)
    {
        tryagain();


    } 


    }


}








int main(int argc , char** argv)
{
    serialSetup();
    thread read_thread(readSerial);
    read_thread.detach();
    ros::init(argc,argv,"dispense_medicine");
    ros::NodeHandle nh;
    
    ros::Subscriber sub = nh.subscribe<medicine_despense::medicine>("medicine",1,callback);
    // ros::Publisher pub = nh.advertise<medicine_despense::medicine>("medicine",1);

    ros::Rate rate(40);
    medicine_despense::medicine med;
    med.medicine_list = new_list;
    // pub.publish(med);
    while(ros::ok())
    {
        //pub.publish(med);
        

       if(!med_list.empty() && moveNext == true)
       {

            
            moveNext = false;
            for(auto x :med_list)
            {
                cout << x << " ";;
            }
            cout << " " << endl;
            string medString = med_list[0];

            char medChar[medString.length()+1];
            strcpy(medChar , medString.c_str()); 
            
            
            
            //char medChar[medString.length()+2];
            //char medChar[256] = {"H\n"
            vector<uint8_t>  medVec(medChar , medChar + medString.length()+1);
            
            //medChar.push_back(medString);
            //medChar.push_back('\n');
            //strcpy(medChar,medString.c_str());
            //medChar[1] = '\n';
            write(serial_port,medVec.data(),medVec.size());
            //std::this_thread::sleep_for(std::chrono::milliseconds(20000));

            strcpy(lastbuffer,med_list[0].c_str());
            lastbuffer_length = med_list[0].length();    
            med_list.erase(med_list.begin());
            

       }
      // read(serial_port,&readbuffer[0],sizeof(readbuffer));
    //    if(readbuffer[0] == 'R')
    //    {
            

    //    }

    rate.sleep();
    ros::spinOnce();

    }

return 0;
}


  
   
 	
