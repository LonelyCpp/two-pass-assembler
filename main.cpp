/*
    An implementation of a two pass assembler

    input   :   input.txt with source code
                opcode.txt with the opcodes used in the source

    output  :   listing_file.txt with the object code.
*/

#include "tables.cpp"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

int strTOhex(string s);
int strTOdec(string s);

string expandlen(int num, int len);
string expandlen(string s, int len);

int main()
{
    table optab, symtab;
    int length = 0;

    optab.load("opcode.txt");       //loads opcodes from file

    // ***************** First Pass *****************

    int locctr = 0, st_addr = 0;
    fstream asmfile, intermfile;
    asmfile.open("input.txt", ios::in);
    intermfile.open("intermediate.txt", ios::out);

    if(asmfile.is_open())
    {
        string lbl, opc, opd;
        asmfile >> lbl >> opc >> opd;
        if(opc == "START")
        {
            st_addr = strTOhex(opd);
            locctr = st_addr;
            intermfile  << hex << setw(10) << locctr << " "
                        << setw(10) << lbl << " "
                        << setw(10) << opc << " "
                        << setw(10) << opd << " "
                        << endl;

            asmfile >> lbl >> opc >> opd;
        }

        while ( opc != "END" )
        {
            if (lbl != "-")
            {
                if(lbl.length() >2 )
                    if(lbl.substr(lbl.length()-2, 2) == ",X")       // checks for indexed addressing
                            lbl = lbl.substr(0, lbl.length()-2);

                if( symtab.search(lbl) == -1)
                    symtab.insert(lbl, locctr);
                else
                    cout<<"error duplicate label";
            }

            intermfile  << hex << setw(10) << locctr << " "
                        << setw(10) << lbl << " "
                        << setw(10) << opc << " "
                        << setw(10) << opd << " "
                        << endl;

            if( optab.search(opc) != -1 )   //found
                locctr += 3;
            else if (opc == "WORD")
                locctr += 3;
            else if (opc == "RESW")
                locctr += 3*strTOdec(opd);
            else if (opc == "RESB")
                locctr += strTOdec(opd);
            else if (opc == "BYTE")
            {
                if(opd[0] == 'X')
                    locctr += (opd.length()-3)/2;

                else if (opd[0] == 'C')
                    locctr += (opd.length()-3);
            }
            else
                cout<<"error invalid code";
            asmfile >> lbl >> opc >> opd;
        }

        intermfile  << hex << setw(10) << locctr << " "
                    << setw(10) << lbl << " "
                    << setw(10) << opc << " "
                    << setw(10) << opd << " "
                    << endl;
        length = locctr - st_addr;
        intermfile << "length = " << length;
        asmfile.close();
        intermfile.close();

        // ***************** Second Pass *****************

        intermfile.open("intermediate.txt", ios::in);
        ofstream list_f("listing_file.txt");
        int temp1, temp2, loc, st_tr, ct = 0, s_ptr, new_t = 0; // ct -> tracks text record length
        int indir = 0;                                          // new_t -> new text record flag. indir -> indexed addressing flag

        intermfile >> loc >> lbl >> opc >> opd;

        if(opc == "START")
        {
            list_f << "H^" << expandlen(lbl, 6) << "^" << expandlen(st_addr, 6)  << "^" << expandlen(length, 6) << endl; // write header record
            intermfile >> loc >> lbl >> opc >> opd;
        }

        list_f << "T^" << expandlen(loc, 6) << "^";

        s_ptr = list_f.tellp();                      // saves location to write size of text record

        list_f <<"**";
        st_tr = loc;
        while ( opc != "END" )
        {
            if (opc != "RESW" && opc != "RESB" )
            {
                if(new_t)                           // if new text record flag is set, writes T^ in newline
                {
                    list_f << "T^" << expandlen(loc, 6) << "^";
                    s_ptr = list_f.tellp();
                    list_f <<"**";
                    ct = 0;
                    new_t = 0;
                }
                if(opc == "WORD")
                {
                    list_f << "^" << expandlen(strTOhex(opd), 6);
                    ct += 6;
                }
                else if(opc == "BYTE")
                {
                    if(opd[0] == 'X')                                   // processes stuff like X'F1'
                    {
                        list_f << "^" << opd.substr(2, opd.length()-3);
                        ct += (opd.length()-3);
                    }
                    else if(opd[0] == 'C')                              // processes stuff like C'TESTING'
                    {
                        list_f << "^";
                        for(int i=2; i<opd.length()-1; i++)
                            list_f << hex <<(int)opd[i];
                            ct += (opd.length()-3)*2;
                            cout<<"adding"<<opd.length()-3<<endl;
                    }
                }
                else
                {

                    if(opd.length() > 2 )
                        if( opd.substr(opd.length()-2, 2) == ",X" )       // checks for indexed addressing
                        {
                            opd = opd.substr(0, opd.length()-2);
                            indir = 1;
                        }

                    if ( (temp1 = optab.search(opc)) == -1 )
                    {
                        cerr<< "error in optab"<< opc <<"\n";
                        temp1 = 0;
                    }

                    if ( (temp2 = symtab.search(opd)) == -1 )
                    {
                            cerr<<"error in symtab "<< opd <<"\n";
                            temp2 = 0;
                    }

                    if(indir == 1 && temp2 < 32768)
                    {
                        temp2 += 32768;
                        indir = 0;
                    }
                    list_f << "^" << expandlen(temp1, 2) << expandlen(temp2, 4);
                    ct += 6;
                }
            }
            else // initialize new text record.
            {
                list_f << endl;
                list_f.seekp(s_ptr);
                list_f << expandlen(ct/2, 2);
                list_f.seekp(0, ios_base::end);

                new_t = 1;
            }
            intermfile >> loc >> lbl >> opc >> opd;
        }

        list_f << endl << "E^" << expandlen(st_addr, 6);
        list_f.seekp(s_ptr);
        list_f << expandlen(ct/2, 2);
    }
    else
    {
        cout<<" Cannot locate input file";
        exit(0);
    }

    cout << "Records written to listing_file.txt ";
    return 0;
}

//takes a string with hex value and returns int equivalent
int strTOhex(string s)
{
    stringstream str;
    str << s;
    int value;
    str >> std::hex >> value;
    return value;
}

//takes a string with decimal value and returns int equivalent
int strTOdec(string s)
{
    stringstream str;
    str << s;
    int value;
    str >> value;
    return value;
}

//expands the length of input. eg: 4 -> 000004
string expandlen(int num, int len)
{
    char n[10];
    itoa(num, n, 16);
    string s = n;
    while(s.length() < len)
        s = "0" + s;

    return s;
}

string expandlen(string s, int len)
{
    while(s.length() < len)
        s = "0" + s;
    return s;
}
