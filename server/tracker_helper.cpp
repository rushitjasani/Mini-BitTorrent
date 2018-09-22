/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/

#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "server_header.h"
#endif

#ifndef CL_GLOBAL_H
#define CL_GLOBAL_H
#include "server_global.h"
#endif

/*
 * give protected fstream object of seeder 
 * file to write and read from it.
 */
fstream getSeederListFile(int mode)
{
    seedfile_mutex.lock();
    fstream my_file;
    if (mode == 0)
        my_file.open(seeder_list, ios::in);
    if (mode == 1)
        my_file.open(seeder_list, ios::out);
    if (mode == 2)
        my_file.open(seeder_list, ios::app);
    return my_file;
}

/*
 * give protected fstream object of logfile to 
 * write log in logfile.
 */
fstream getLogFile()
{
    logfile_mutex.lock();
    fstream my_file;
    my_file.open(log_file, ios::app);
    return my_file;
}

void writeLog(string message)
{
    fstream logfile_fd;
    logfile_fd = getLogFile();
    time_t cur = time(NULL);
    logfile_fd << ctime(&cur) << "" << message << endl;
    logfile_mutex.unlock();
    return;
}

/*
 * APPEND data to seeder file 
 */
void append_to_seederlist(string data)
{
    fstream seeder_file;
    seeder_file = getSeederListFile(2);
    seeder_file << data << endl;
    seeder_file.close();
    seedfile_mutex.unlock();
    writeLog("SeederList file updated.");
    return;
}

/*
 * take file stream object of seeder file and overwrite whole map to 
 * that file into it. 
 */
void write_to_seederlist()
{
    fstream seed_file;
    seed_file = getSeederListFile(1);
    string s;
    for (auto i : seeder_map)
    {
        for (auto j : i.second)
            seed_file << i.first << SEP << j.first << SEP << j.second << endl;
    }
    seed_file.close();
    seedfile_mutex.unlock();
    writeLog("SeederList file updated.");
    return;
}

/*
 * At Wakeup time, read seederlist file and 
 * dump that content to map
 */
void read_seederlist()
{
    fstream seed_file;
    seed_file = getSeederListFile(0);
    string s;
    while (getline(seed_file, s))
    {
        char *token = strtok((char *)s.c_str(), SEP.c_str());
        string key_hash = token;
        token = strtok(NULL, SEP.c_str());
        string cl_socket = token;
        token = strtok(NULL, SEP.c_str());
        string file_path = token;
        seeder_map[key_hash][cl_socket] = file_path;
    }
    seed_file.close();
    seedfile_mutex.unlock();
    // print_map();
    return;
}

/*
 * Process command line arguments and stores
 * all ip/port in apropriate fields in global
 * variables.
 */
void process_args(char *argv[])
{
    char *token = strtok(argv[1], ":");
    tr1_ip = token;
    token = strtok(NULL, ":");
    tr1_port = stoi(token);

    token = strtok(argv[2], ":");
    tr2_ip = token;
    token = strtok(NULL, ":");
    tr2_port = stoi(token);

    seeder_list = argv[3];
    log_file = argv[4];
}

/*
 * prints whole content of map and print on screen
 */
void print_map()
{
    string print_str;
    print_str = "******* CURRENT MAP DATA *******\n";
    print_str += "================================\n";
    for (auto i : seeder_map)
    {
        print_str += i.first + SEP;
        for (auto j : i.second)
            print_str += j.first + SEP + j.second +"\n";
        print_str += "\n";
    }
    print_str = "================================\n";
    writeLog(print_str);

    // cout << "================================" << endl;
    // for (auto i : seeder_map)
    // {
    //     cout << i.first << endl;
    //     for (auto j : i.second)
    //         cout << j.first << endl
    //              << j.second << endl;
    //     cout << endl;
    // }
    // cout << "================================" << endl;
}