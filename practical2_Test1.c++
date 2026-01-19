#include<iostream>
#include<string>
using namespace std;



int main(){
    string input;
    cout<<"enter a string: ";
    cin>>input;

    int currentstate=0;
  
    for(char c:input){
        switch (currentstate)
        {
        //first state
        case 0:
             if(c=='0')
             {
                currentstate=1;
             }
             else if(c=='1')
             {
                currentstate=0; 
             }
             else
             {
                currentstate=-1;
             }
            break;
        // second state
        case 1:
              if(c=='1')
              {
                currentstate=2;
              }
              else
              {
                currentstate=-1;
              }
              break;
        //third state
        case 2:
           if(c=='1')
           {
            currentstate=0;
           }
           else
           {
            currentstate=-1;
           }
            break;

        }
        if(currentstate==-1)
        {
            break;
        }
    }
    if(currentstate==0)
    {
        cout<<"accepted"<<endl;
    }
    else
    {
        cout<<"not accepted"<<endl;
    }
}