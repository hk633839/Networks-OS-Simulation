#include<bits/stdc++.h>
#include<stdio.h>

#define memory 1000         // Total memory size
#define partition_size 10   // Size of each fixed partition

// Structure to define the linked allocation node
struct linked_file
{
    int file_no;
    int size;
    int block_no;
    struct linked_file* next;
};

using namespace std;

// Function to find vacant space for contiguous allocation
int find(vector<pair<int, vector<int>>>& vect, int size)
{
    for(int i=0; i<vect.size(); i++)
    {
        if(vect[i].first >= size)
        {
            return i;
        }
    }
    return -1;
}

// Function to find the vacant space for the linked allocation
int linked_find(vector<int>& space, int size)
{
    for(int i=0; i<space.size(); i++)
    {
        if(space[i] >= size) return i;
    }
    return -1;
}

// Function to find whether the file is already present or not in indexed allocation
int indexed_find(vector<pair<int, int>>& temp, int file_no, int* access_times)
{
    for(int i=0; i<temp.size(); i++)
    {
        (*access_times)++;
        if(temp[i].first == file_no) return i;
    }
    return -1;
}

// Function to insert the node into the linked list in linked allocation
void node_insert(struct linked_file** head, int file_no, int size, int block_no)
{
    struct linked_file* temp = *head;
    struct linked_file* dummy = new linked_file;
    dummy->file_no = file_no;
    dummy->size = size;
    dummy->block_no = block_no;
    dummy->next = NULL;
    if(*head == NULL) 
    {
        *head = dummy;
        return;
    }
    while(temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = dummy;
}

// Function to find a specific node in the linked list in linked allocation
struct linked_file* node_finder(struct linked_file** head, int file_no, int* access_times)
{
    struct linked_file* temp = *head;
    while (temp != NULL)
    {
        (*access_times)++;
        if(temp->file_no == file_no) return temp;
        temp = temp->next;
    }
    return NULL;
}

// Function to delete a node in the linked list in linked allocation
void node_delete(struct linked_file** head, int file_no)
{
    struct linked_file* temp = *head;
    struct linked_file* after = *head;
    if(temp->file_no == file_no)
    {
        *head = (*head)->next;
        free(temp);
    }
    else
    {
        while(temp->next->file_no != file_no)
        {
            temp = temp->next;
        }
        after = temp->next;
        if(after->next == NULL)
        {
            temp->next = NULL;
            free(after);
        }
        else
        {
            temp->next = after->next;
            after->next = NULL;
            free(after);
        }
    }
    
}

// Function to find the location of a specific file in the contiguous allocation
pair<int, int> location(vector<pair<int, vector<int>>>& vect, int file_no)
{
    for(int i=0; i<vect.size(); i++)
    {
        for(int j=0; j<vect[i].second.size(); j++)
        {
            if(vect[i].second[j] == file_no) return make_pair(i,j);
        }
    }
    return make_pair(-1,-1);
}


//<-------------------------------------------------------------------------Contiguous allocation--------------------------------------------------------------------------->// 


pair<int, double> contiguous(vector<pair<int, int>>& requests, vector<int>& operation)
{
    vector<pair<int, vector<int>>> space(memory / partition_size);
    int access_time=0;
    double used_space=0;

    // Initializing the space vector
    for(int i=0; i<memory / partition_size; i++)
    {
        space[i].first = partition_size;
    }

    // Satisfying the file requests
    for(int i=0; i<operation.size(); i++)
    {
        // Create file operation
        if(operation[i] == 1)
        {
            if(requests[i].second > partition_size) cout << "The file is too large to fit into any of the blocks" << endl;
            else
            {
                int index = find(space, requests[i].second);
                if(index == -1)
                {
                    access_time += space.size();
                    cout << "No space is present to accomodate the file" << endl;
                }
                else
                {
                    for(int k=0; k<index; k++)
                    {
                        access_time += space[k].second.size();
                    }
                    access_time += space[index].second.size()+1;
                    space[index].second.push_back(requests[i].first);
                    space[index].first -= requests[i].second;
                    cout << "The file is allocated a space" << endl;
                }
            }
        }

        // Read file operation
        else if(operation[i] == 2)
        {
            pair<int, int> temp;

            // Finding the location of the file
            temp = location(space, requests[i].first);
            for(int k=0; k<temp.first; k++)
            {
                access_time += space[k].second.size();
            }
            access_time += temp.second;
            if(temp.first == -1) cout << "The requested file is not present" << endl;
            else 
            {
                cout << "The file has been read from the partition " << temp.first << endl;
            }
        }

        // Update file operation
        else if(operation[i] == 3)
        {
            pair<int, int> temp;
            temp = location(space, requests[i].first);
            for(int k=0; k<temp.first; k++)
            {
                access_time += space[k].second.size();
            }
            access_time += temp.second;
            if(temp.first == -1) cout << "The requested file is not present" << endl;
            else 
            {
                cout << "The file has been updated from the partition " << temp.first << endl;
            }
        }

        // Delete file operation
        else if(operation[i] == 4)
        {
            pair<int, int> temp;
            temp = location(space, requests[i].first);

            // Handling access times
            for(int k=0; k<temp.first; k++)
            {
                access_time += space[k].second.size();
            }
            access_time += temp.second;
            if(temp.first == -1) cout << "The requested file is not present" << endl;
            else
            {
                space[temp.first].second[temp.second] = -1;
                space[temp.first].first += requests[i].second;
                cout << "The file has been deleted from the memory" << endl;
            }
        }

        // Handling space used
        double temp=0;
        for(int j=0; j<space.size(); j++)
        {
            temp += partition_size - space[j].first;
        }
        used_space += temp / memory;
    }
    used_space /= operation.size();
    used_space *= 100;
    return make_pair(access_time, used_space);
}


//<-------------------------------------------------------------------------Linked allocation------------------------------------------------------------------------------->//


pair<int, double> linked(vector<pair<int, int>>& requests, vector<int>& operation)
{
    struct linked_file* head = NULL;

    // Initializing the space vector
    vector<int> space(memory / partition_size, partition_size);
    int access_times=0;
    double used_space=0;

    // Iterating through the file requests
    for(int i=0; i<operation.size(); i++)
    {
        // File create operation
        if(operation[i] == 1)
        {
            int free_space = -1;

            // Finding free space
            free_space = linked_find(space, requests[i].second);
            if(free_space == -1) 
            {
                access_times += space.size();
                cout << "No free space present to allocate" << endl;
            }
            else 
            {
                access_times += free_space;
                node_insert(&head, requests[i].first, requests[i].second, free_space);
                space[free_space] -= requests[i].second;
                cout << "The file is allocated a space" << endl;
            }
        }

        // Read file operation
        else if(operation[i] == 2)
        {
            struct linked_file* node_pointer = NULL;

            // Finding the node of that particular file
            node_pointer = node_finder(&head, requests[i].first, &access_times);
            if(node_pointer == NULL) cout << "No such file exists in 2" << endl;
            else 
            {
                cout << "The file has been read" << endl;
                access_times++;
            }
        }

        // Update file operation
        else if(operation[i] == 3)
        {
            struct linked_file* node_pointer = NULL;

            // Finding the node of that particular file
            node_pointer = node_finder(&head, requests[i].first, &access_times);
            if(node_pointer == NULL) cout << "No such file exists in 3" << endl;
            else 
            {
                cout << "The file has been updated" << endl;
                access_times++;
            }
        }

        // Delete file operation
        else if(operation[i] == 4)
        {
            struct linked_file* node_pointer = NULL;

            // Finding the node of that particular file
            node_pointer = node_finder(&head, requests[i].first, &access_times);
            if(node_pointer == NULL) cout << "No such file exists in 4" << endl;
            else
            {
                space[node_pointer->block_no] += node_pointer->size;
                node_delete(&head, requests[i].first);
                access_times++;
            }
        }

        // Handling space used
        double temp=0;
        for(int j=0; j<space.size(); j++)
        {
            temp += partition_size - space[j];
        }
        used_space += temp / memory;
    }
    used_space /= operation.size();
    used_space *= 100;
    return make_pair(access_times, used_space);
}


//<-------------------------------------------------------------------------Indexed allocation------------------------------------------------------------------------------>//


pair<int, double> indexed(vector<pair<int, int>>& requests, vector<int>& operation)
{
    // Initializing the space vector
    vector<int> space(memory / partition_size, partition_size);

    // Initializing the index vector
    vector<pair<int, int>> index_block;
    int access_times=0;
    double used_space=0;

    // Iterating through every file request
    for(int i=0; i<operation.size(); i++)
    {

        // File create operation
        if(operation[i] == 1)
        {
            int free_space = -1;

            // Finding free space
            free_space = linked_find(space, requests[i].second);
            if(free_space == -1) 
            {
                access_times += space.size();
                cout << "No free space present to allocate" << endl;
            }
            else 
            {
                access_times += free_space;
                index_block.push_back(make_pair(requests[i].first, free_space));
                space[free_space] -= requests[i].second;
                cout << "The file is allocated a space" << endl;
            }
        }

        // Read file operation
        else if(operation[i] == 2)
        {
            // Finding the file through index block
            int index = indexed_find(index_block, requests[i].first, &access_times);
            if(index == -1) cout << "No such file exists in 2" << endl;
            else 
            {
                cout << "The file has been read" << endl;
                access_times += 2;
            }
        }

        // Updating file operation
        else if(operation[i] == 3)
        {
            // Finding the file through index block
            int index = indexed_find(index_block, requests[i].first, &access_times);
            if(index == -1) cout << "No such file exists in 3" << endl;
            else 
            {
                cout << "The file has been updated" << endl;
                access_times += 2;
            }
        }

        // Deleting file operation
        else if(operation[i] == 4)
        {
            // Finding the file through index block
            int index = indexed_find(index_block, requests[i].first, &access_times);
            if(index == -1) cout << "No such file exists in 4" << endl;
            else 
            {
                space[index_block[index].second] += requests[i].second;
                index_block.erase(index_block.begin()+index);
                access_times += 2;
            }
        }

        // Handling used space
        double temp=0;
        for(int j=0; j<space.size(); j++)
        {
            temp += partition_size - space[j];
        }
        used_space += temp / memory;
    }
    used_space /= operation.size();
    used_space *= 100;
    return make_pair(access_times, used_space);
}


//<----------------------------------------------------------------------Modified Contiguous allocation--------------------------------------------------------------------->//


pair<int, double> modified_contiguous(vector<pair<int, int>>& requests, vector<int>& operation)
{
    // Initializing the space vector
    vector<pair<int, vector<int>>> space(memory / partition_size);
    vector<pair<int, int>> additional;
    int access_time=0;
    double used_space=0;
    for(int i=0; i<memory / partition_size; i++)
    {
        space[i].first = partition_size;
    }

    // Iterating through every file request
    for(int i=0; i<operation.size(); i++)
    {
        // File create operation
        if(operation[i] == 1)
        {
            if(requests[i].second > partition_size) cout << "The file is too large to fit into any of the blocks" << endl;
            else
            {
                // Finding empty space
                int index = find(space, requests[i].second);
                if(index == -1)
                {
                    access_time += space.size();
                    cout << "No space is present to accomodate the file" << endl;
                }
                else
                {
                    for(int k=0; k<index; k++)
                    {
                        access_time += space[k].second.size();
                    }
                    access_time += space[index].second.size()+1;
                    space[index].second.push_back(requests[i].first);
                    space[index].first -= requests[i].second;
                    cout << "The file is allocated a space" << endl;
                }
            }
        }

        // Read file operation
        else if(operation[i] == 2)
        {
            pair<int, int> temp;

            // Finding the location of that particular file
            temp = location(space, requests[i].first);
            for(int k=0; k<temp.first; k++)
            {
                access_time += space[k].second.size();
            }
            access_time += temp.second;
            if(temp.first == -1) cout << "The requested file is not present" << endl;
            else 
            {
                // Reading the additional space also if present
                int place = indexed_find(additional, requests[i].first,&access_time);
                if(place == -1) access_time++;
                cout << "The file has been read from the partition " << temp.first << endl;
            }
        }

        // Update file operation and increase the space of the file
        else if(operation[i] == 3)
        {
            pair<int, int> temp;

            // Finding location of that particular file
            temp = location(space, requests[i].first);
            for(int k=0; k<temp.first; k++)
            {
                access_time += space[k].second.size();
            }
            access_time += temp.second;
            if(temp.first == -1) cout << "The requested file is not present" << endl;
            else 
            {
                // Adding some additional space to already present file
                access_time++;
                int place = indexed_find(additional, requests[i].first,&access_time);
                if(place == -1) additional.push_back(make_pair(requests[i].first, requests[i].second));
                else 
                {
                    additional[place].second += requests[i].second;
                }
                cout << "The file has been updated from the partition " << temp.first << endl;
            }
        }

        // Delete the file operation
        else if(operation[i] == 4)
        {
            pair<int, int> temp;
            temp = location(space, requests[i].first);
            for(int k=0; k<temp.first; k++)
            {
                access_time += space[k].second.size();
            }
            access_time += temp.second;
            if(temp.first == -1) cout << "The requested file is not present" << endl;
            else
            {
                // Deleting the additional space if present
                int place = indexed_find(additional, requests[i].first,&access_time);
                if(place != -1) 
                {
                    additional.erase(additional.begin()+place);
                    access_time++;
                }
                space[temp.first].second[temp.second] = -1;
                space[temp.first].first += requests[i].second;
                cout << "The file has been deleted from the memory" << endl;
            }
        }

        // Handling the used space
        double temp=0;
        for(int j=0; j<space.size(); j++)
        {
            temp += partition_size - space[j].first;
        }
        used_space += temp / memory;
    }
    used_space /= operation.size();
    used_space *= 100;
    return make_pair(access_time, used_space);
}

int main()
{
    srand(time(NULL));
    vector<pair<int, int>> requests;
    vector<int> operations;
    vector<pair<int, double>> contiguous_res;
    vector<pair<int, double>> linked_res;
    vector<pair<int, double>> indexed_res;
    vector<pair<int, double>> mod_cont_res;
    for(int j=0; j<5; j++)
    {
        for(int i=0; i<500; i++)
        {
            requests.push_back(make_pair(rand() % 100, (rand() % 15)+1));
            operations.push_back((rand()%4)+1);
        }
        contiguous_res.push_back(contiguous(requests, operations));
        linked_res.push_back(linked(requests, operations));
        indexed_res.push_back(indexed(requests, operations));
        mod_cont_res.push_back(modified_contiguous(requests, operations));
    }
    for(int j=0; j<5; j++)
    {
        printf("\n\n Iteration %d :\n", j+1);
        cout<<setw(15)<<"Type"<<setw(15)<<"Contiguous"<<setw(15)<<"Linked"<<setw(15)<<"Indexed"<<setw(15)<<"Modified"<<endl;
        cout<<setw(15)<<"Accesses"<<setw(15)<<contiguous_res[j].first<<setw(15)<<linked_res[j].first<<setw(15)<<indexed_res[j].first<<setw(15)<<mod_cont_res[j].first<<endl;
        cout<<setw(15)<<"Space(%)"<<setw(15)<<contiguous_res[j].second<<setw(15)<<linked_res[j].second<<setw(15)<<indexed_res[j].second<<setw(15)<<mod_cont_res[j].second<<endl;    
    }

    return 0;
}