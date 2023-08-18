#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <bits/stdc++.h>

using namespace std;

// Message queue structure
struct msgbuff {
    long msgtype;
    char msgtext[100];
};

// Function to generate a random answer
char generate_answer() {
    return (rand() % 4) + 'A';
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    int num_students, num_questions;
    cout << "Enter the number of students taking the exam: ";
    cin >> num_students;
    cout << "Enter the number of questions on the exam: ";
    cin >> num_questions;

    // Array to store the grades of the students
    int grades[num_students] = {0};
    char final_grades[num_students] = {"F"};

    // Create message queues for sending questions and receiving answers
    int send_qid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    int recv_qid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    // Fork child processes for each student
    for (int i = 0; i < num_students; i++) {
        pid_t pid = fork();
        if (pid == 0) {

            // Child process
            for (int j = 0; j < num_questions; j++) {

                // Receive question from parent process
                msgbuff msg;
                msgrcv(send_qid, &msg, sizeof(msg.msgtext), j + 1, 0);
                cout << "Student " << i + 1 << " received question " << j + 1 << ": " << msg.msgtext << endl;

                // Generate random answer
                char answer = generate_answer();
                cout << "Student " << i + 1 << " answered question " << j + 1 << ": " << answer << endl;

                // Send answer to parent process
                msg.msgtype = i + 1;
                strcpy(msg.msgtext, &answer);
                msgsnd(recv_qid, &msg, sizeof(msg.msgtext), 0);
            }
            exit(0);
        }

        // Handling the error cases 
        else if (pid < 0) {
            cerr << "Error: fork failed" << endl;
            exit(1);
        }
    }

    // Parent process
    for (int i = 0; i < num_questions; i++) {
        // Generate random question
        string question = "Question " + to_string(i + 1) + "?";

        // Send question to all child processes
        msgbuff msg;
        msg.msgtype = i + 1;
        strcpy(msg.msgtext, question.c_str());
        for (int j = 0; j < num_students; j++) {
            msgsnd(send_qid, &msg, sizeof(msg.msgtext), 0);
        }

        // Receive answers from all child processes
        int num_correct = 0;
        int num_incorrect = 0;
        for (int j = 0; j < num_students; j++) {

            // Receive answer from child process
            msgrcv(recv_qid, &msg, sizeof(msg.msgtext), j + 1, 0);
            char answer = msg.msgtext[0];
            cout << "Student " << j + 1 << " answered question " << i + 1 << ": " << answer << endl;

            // Checking whether the answer is correct or not
            if (answer == generate_answer()) {
                grades[j+1]++;
                num_correct++;
            } else {
                num_incorrect++;
            }
        }

    }
    
    for(int i=0; i<num_students; i++)
    {
        double temp = ((double)(grades[i]) / num_questions) * 100;
 
        if (temp >= 50)
        {
            final_grades[i] = 'A';
        }
        else
        {
            if (temp >= 40 && temp <= 49)
            {
                final_grades[i] = 'B';
            }
            else
            {
                if (temp >= 30 && temp <= 39)
                {
                    final_grades[i] = 'C';
                }
                else
                {
                    if (temp >= 20 && temp <= 29)
                    {
                        final_grades[i] = 'D';
                    }
                    else
                    {
                        final_grades[i] = 'F';
                    }
                }
            }
        }
    }

    // Printing the grades of each student
    for(int i=0; i<num_students; i++)
    {
        cout << "Grade of student " << i + 1 << " is " << final_grades[i] << endl;
    }
}