#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

// structure for basic string + integer combination
// can be used in opcode and label
struct element
{
    std::string name;
    int code;
};


// class that maintains table.
class table
{
private:
    element entry[20];
    int num;

public:

    table(){num=0;}

    int insert(std::string s, int cd)
    {
        if(num == 20)
                return -1;                  //overflow
        else
        {
            entry[num].name = s;
            entry[num].code = cd;
            num++;
        }
        return 0;
    }

    int search(std::string n)
    {
        for(int i=0; i < num; i++)
            if( n == entry[i].name)
                return entry[i].code;       //returns code

        return -1;                          //not found
    }

    int load(char *path)
    {

        std::ifstream file(path);
        if(file.is_open())
        {
            int cd;
            std::string nm;
            while(file >> nm >> std::hex >> cd )
                insert(nm, cd);

            file.close();
            return 1;
        }
        else
        {
            file.close();
            return 0;
        }
    }

    void disp()
    {
        for(int i=0; i < num; i++)
                 std::cout << entry[i].name << " : " << std::hex << entry[i].code << std::endl;
    }

};

