#include <iostream>
#include <string>
#include <vector>
#include <fstream> 
#include <sstream> 
#include <unistd.h>
#include <sys/wait.h>

using namespace std;



int main()
{
    string d1 = "d1";
    string d2 = "d1";

    // Create the directories and files
    system(("mkdir " + d1).c_str());
    system(("echo 'hari_krishna' > " + d1 + "/file1").c_str());
    system(("echo 'pavan_kumar' > " + d1 + "/file2").c_str());

    system(("mkdir " + d2).c_str());
    system(("echo 'jaya_teja' > " + d2 + "/file3").c_str());
    system(("echo 'sri_ram' > " + d2 + "/file4").c_str());


    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();

    if (pid1 < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    {
        close(pipe1[0]); // Close read end of pipe1
        close(pipe2[1]); // Close write end of pipe2

        // sendFileFromDir1ToDir2
        vector<string> files1;
        files1.push_back("file1");
        files1.push_back("file2");

        string msg1 = "";
        for (const auto &file : files1)
        {
            string filename = d1 + "/" + file;
            ifstream f(filename.c_str());
            if (!f.good())
            {
                msg1 = "Error: failed to read " + filename;
                write(pipe1[1], msg1.c_str(), msg1.size() + 1);
                exit(EXIT_FAILURE);
            }
            string contents((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            msg1 += file + " " + contents + "\n";
        }
        write(pipe1[1], msg1.c_str(), msg1.size() + 1);

        // readFileFromDir2ToDir1
        string rmsg1 = "";
        char buf1[BUFSIZ];

        if (read(pipe2[0], buf1, BUFSIZ) > 0)
        {
            rmsg1 += buf1;
        }

        cout << "message recieved is : " << rmsg1 << "\n";

        // vector<string> files2;
        istringstream iss(rmsg1);
        string filePath, file, contents;
        while (iss >> file >> contents)
        {
            // files2.push_back(file);
            string filename = d1 + "/" + file;
            ofstream f(filename.c_str());
            if (!f.good())
            {
                rmsg1 = "Error: failed to create " + filename;
                write(pipe1[1], rmsg1.c_str(), rmsg1.size() + 1);
                exit(EXIT_FAILURE);
            }
            f << contents << "\n";
            f.close();
        }

        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();

    if (pid2 < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid2 == 0)
    {
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

        // readFileFromDir1ToDir2(pipe1, pipe2, d2);
        string rmsg2 = "";
        char buf[BUFSIZ];
        int r;

        cout << "in child 2\n";

        if (read(pipe1[0], buf, BUFSIZ) > 0)
        {
            rmsg2 += buf;
        }

        cout << "message recieved in child2 : " << rmsg2 << "\n";

        // vector<string> files2;
        istringstream iss(rmsg2);
        string filePath, file, contents;
        while (iss >> file >> contents)
        {
            // files2.push_back(file);
            string filename = d2 + "/" + file;
            ofstream f(filename.c_str());
            if (!f.good())
            {
                rmsg2 = "Error in create " + filename;
                cout<< rmsg2<<endl;
                write(pipe2[1], rmsg2.c_str(), rmsg2.size() + 1);
                exit(EXIT_FAILURE);
            }
            f << contents << "\n";
            f.close();
        }
        //senddir2todir1
        vector<string> files2;
        files2.push_back("file3");
        files2.push_back("file4");

        string msg2 = "";

        for (const auto &file : files2)
        {
            string filename = d2 + "/" + file;
            // cout << "filename:" << filename << "\n";
            ifstream f(filename.c_str());
            if (!f.good())
            {
                msg2 = "Error in read " + filename;
                write(pipe2[1], msg2.c_str(), msg2.size() + 1);
                exit(EXIT_FAILURE);
            }
            string contents((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            msg2 += file + " " + contents + "\n";
        }
        write(pipe2[1], msg2.c_str(), msg2.size() + 1);
        exit(EXIT_SUCCESS);
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Check that the directories are identical
    system(("diff -r " + d1 + " " + d2).c_str());

    return 0;
}

