#include<bits/stdc++.h>
#include<stdio.h>

int ram = 270;             // Page table size
int swap_space = 450;      // Swap space size
int ref_size = 10000;      // Size of reference string
#define page_size 4         // Page size
using namespace std;


// Function to make the values of a vector to default
void make_default(vector<int>& frames, vector<int>& virtual_space)
{
    for(int i=0; i<frames.size(); i++)
    {
        frames[i] = -1;
    }
    for(int i=0; i<virtual_space.size(); i++)
    {
        virtual_space[i] = i + 1;
    }
}


// Function to find and erase a value in a given vector
void erase(vector<int>& temp, int a)
{
    for(int i=0; i<temp.size(); i++)
    {
        if(temp[i] == a) 
        {
            temp.erase(temp.begin()+i);
            return;
        }
    }
}


// Function to check and insert a value in a given vector 
void insert(vector<int>& temp, int a)
{
    for(int i=0; i<temp.size(); i++)
    {
        if(temp[i] == a) 
        {
            return;
        }
    }    
    temp.push_back(a);

}


// Function to find the index of the minimum element in the given vector
int min(vector<int>& temp)
{
    int res = temp[0], index=0;
    for(int i=0; i<temp.size(); i++)
    {
        if(res > temp[i]) 
        {
            res = temp[i];
            index = i;
        }
    }
    return index;
}


// Function to find the next occurence of a value in a given vector
int find_next_index(vector<int>& ref_string, int index, int value)
{
    for(int i=index; i<ref_string.size(); i++)
    {
        if(ref_string[i] == value) return i;
    }
    return INT_MAX;
}


// Function to implement the Least Recently Used replacement algorithm
pair<int, int> lru(vector<int>& ref_string, vector<int>& frames, vector<int>& virtual_space)
{
    int faults=0, i, j, k, memory=0;
    vector<int> access(ram, 0);
    for(i=0;i<ref_size;i++)
    {
		int present=0;

        // Checking whether the page is present in the page table or not
		for(j=0;j<ram;j++)
        {
			if(frames[j]==ref_string[i])
            {
				present=1;
				access[j]++;
				break;
			}
        }
		if(present==0)
        {
			int empty=0;
			faults++;

            // Check whether there are any empty slots in the page table
			for(k=0;k<ram;k++)
            {
				if(frames[k]==-1)
                {
					empty=1;
					break;
				}
            }

            // If there is an empty slot in the page table
			if(empty==1)
            {
				frames[k]=ref_string[i];
                erase(virtual_space, ref_string[i]);
                memory += 2;
				access[k]++;
			}

            // If no empty slot in the page table
			else
            {
				j=min(access);
                insert(virtual_space, frames[j]);
				frames[j]=ref_string[i];
                erase(virtual_space, ref_string[i]);
                memory += 3;
				access[j]++;
			}
		}
	}
    memory += ref_string.size() + frames.size() + virtual_space.size();
    return make_pair(faults, memory * 4);
}

// Function to implement the First In First Out replacement algorithm
pair<int, int> fifo(vector<int>& ref_string, vector<int>& frames, vector<int>& virtual_space)
{
    int faults=0, index=0, memory = 0;
    for(int i=0;i<ref_size;i++)
    {
		int present=0;

        // Checking whether the page is present in the page table or not
		for(int j=0;j<ram;j++)
        {
			if(frames[j]==ref_string[i])
            {
				present=1;
				break;
			}
        }

        // If the page is not present in the page table
		if(present==0)
        {
			faults++;
            insert(virtual_space, frames[index]);
			frames[index]=ref_string[i];
            erase(virtual_space, ref_string[i]);
			if(index==ram-1)
            {
				index=0;
            }
			else
            {
				index++;
            }
		}
	}
    memory = ref_string.size() + frames.size() + virtual_space.size() + faults * 3;
    return make_pair(faults, memory * 4);
}

// Function to impelement the Optimal page replacement algorithm
pair<int, int> optimal(vector<int>& ref_string, vector<int>& frames, vector<int>& virtual_space)
{
    int faults=0, memory=0, load=0;
    vector<int> upcoming_index(ram, INT_MAX);
    for(int i=0; i<ref_size; i++)
    {
        int present = 0;

        // Checking whether the page is present in the page table or not
        for(int j=0; j<ram; j++)
        {
            if(frames[j] == ref_string[i])
            {
                present = 1;
                break;
            }
        }
        if(present == 0)
        {
            faults++;

            // If there are any empty slots in the page table
            if(load < ram - 1)
            {
                frames[load] = ref_string[i];
                upcoming_index[load] = find_next_index(ref_string, i, ref_string[i]);
                erase(virtual_space, ref_string[i]);
                memory += 2;
                load++;
            }

            // If no empty slots then a page is replaced from the page table
            else 
            {
                int p = min(upcoming_index);
                insert(virtual_space, frames[p]);
                frames[p] = ref_string[i];
                erase(virtual_space, ref_string[i]);
                memory += 3;
            }
        }
    }
    memory += ref_string.size() + frames.size() + virtual_space.size();
    return make_pair(faults, memory * 4);
}

pair<int, int> random(vector<int>& ref_string, vector<int>& frames, vector<int>& virtual_space)
{
    int faults=0, index=0, memory = 0;
    for(int i=0;i<ref_size;i++)
    {
		int present=0;

        // Checking whether the page is present in the page table or not
		for(int j=0;j<ram;j++)
        {
			if(frames[j]==ref_string[i])
            {
				present=1;
				break;
			}
        }

        // If the page is not present in the page table
		if(present==0)
        {
			faults++;
            index = rand() % frames.size();
            insert(virtual_space, frames[index]);
			frames[index]=ref_string[i];
            erase(virtual_space, ref_string[i]);
		}
	}
    memory = ref_string.size() + frames.size() + virtual_space.size() + faults * 3;
    return make_pair(faults, memory * 4);
}

int main()
{
    srand(time(NULL));
    vector<int> ref_string (ref_size, -1);
    vector<pair<int, int>> fifo_res;
    vector<pair<int, int>> lru_res;
    vector<pair<int, int>> optimal_res;
    vector<pair<int, int>> random_res;

    for(int i=400; i<=500; i += 5)
    {
         // Taking the input of reference strings into the vector
        for(int k=0; k<ref_size; k++)
        {
            ref_string[k] = rand() % swap_space;
        }
        for(int j=270; j<=300; j += 3)
        {
            swap_space = i;
            ram = j;
            vector<int> virtual_space(swap_space, -1);
            vector<int> frames (ram, -1);
            make_default(frames, virtual_space);
            fifo_res.push_back(fifo(ref_string, frames, virtual_space));
            make_default(frames, virtual_space);
            lru_res.push_back(lru(ref_string, frames, virtual_space));
            make_default(frames, virtual_space);
            optimal_res.push_back(optimal(ref_string, frames, virtual_space));
            make_default(frames, virtual_space);
            random_res.push_back(random(ref_string, frames, virtual_space));
        }
    }

    int count=0;
    for(int i = 400; i<=500; i += 5)
    {
        for(int j=270; j<=300; j += 3)
        {
            printf("\n\nThe # of frames and swap space used are %d and %dKB\n", j, i*4);
            cout<<setw(10)<<"Type"<<setw(10)<<"FIFO"<<setw(10)<<"LRU"<<setw(10)<<"Optimal"<<setw(10)<<"Random"<<endl;
            cout<<setw(10)<<"Faults"<<setw(10)<<fifo_res[count].first<<setw(10)<<lru_res[count].first<<setw(10)<<optimal_res[count].first<<setw(10)<<random_res[count].first<<endl;
            cout<<setw(10)<<"Memory(KB)"<<setw(10)<<fifo_res[count].second<<setw(10)<<lru_res[count].second<<setw(10)<<optimal_res[count].second<<setw(10)<<random_res[count].second<<endl;
            count++;
        }
    }

    return 0;
}