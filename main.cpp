#include <iostream>
#include <fstream>
#include <string>
#include <omp.h> // Include the OpenMP library for parallel processing

using namespace std;

int main()
{
    string file_name = "File3.txt"; // Define the name of the file to search
    string word_to_search; // Define a string to store the word to search
    int total_count = 0; // Initialize a variable to store the total count of the word
    //int num_threads = omp_get_max_threads(); // Get the number of threads to use for parallel processing
    int num_threads = 4;
    // Get the word to search from the user
    cout << "Enter the word to search: ";
    cin >> word_to_search;

    // Open the file
    fstream file; // Declare a file stream object
    file.open(file_name.c_str()); // Open the file with the specified name

    if(file.is_open()) // If the file is successfully opened
    {
        // Get the file size
        file.seekg(0, ios::end); // Move from the beginning the end of the file
        int file_size = file.tellg(); // Get the position of the end of the file, which is its size in bytes
        cout << "File size : " << + file_size << endl;
        file.seekg(0, ios::beg); // Move back to the beginning of the file

        // Divide the file into chunks for each thread
        int chunk_size = file_size / num_threads; // Calculate the size of each chunk
        int last_chunk_size = file_size % num_threads; // Calculate the size of the last chunk
        int start = 0, ending = chunk_size - 1; // Initialize the start and end positions of the first chunk

        double startTime = omp_get_wtime();
        #pragma omp parallel num_threads(num_threads) reduction(+:total_count) // Start a parallel region with the specified number of threads and the total_count variable as a reduction
        {
            int thread_id = omp_get_thread_num(); // Get the ID of the current thread
            int local_count = 0; // Initialize a variable to store the local count of the word
            int chunk_start = start + thread_id * chunk_size; // Calculate the start position of the current thread's chunk
            int chunk_end = chunk_start + ending; // Calculate the end position of the current thread's chunk

            if(thread_id == num_threads - 1) // If this is the last thread
            {
                chunk_end += last_chunk_size; // Add the size of the last chunk to its end position
            }

            // Open a new stream for each thread
            fstream file_thread; // Declare a file stream object
            file_thread.open(file_name.c_str()); // Open the file with the specified name

            if(file_thread.is_open()) // If the file is successfully opened
            {
                // Move to the correct position in the file for this thread
                file_thread.seekg(chunk_start, ios::beg); // Move to the start position of the current thread's chunk

                // Read the words in this thread's chunk
                string word_thread; // Declare a string to store the current word
                while(file_thread.tellg() <= chunk_end && file_thread >> word_thread) // While the current position is within the current thread's chunk and there are still words to read
                {
                    if(word_thread == word_to_search) // If the current word is the word to search
                    {
                        local_count++; // Increment the local count of the word
                    }
                    //cout << "Thread id : " << +thread_id << " chunk's word count : " << +local_count <<endl;
                }
                cout << "Thread: " << +thread_id << " Chunk word count: " << +local_count <<endl;
            }
            else
            {
                cout << "Thread " << thread_id << ": File not open" << endl; // If file not open
            }

            total_count += local_count; //Get the summation of the subt-task's word count
        }
        double endTime = omp_get_wtime();
        cout << "Total count: " << total_count << endl;
        cout << "Time is " << (endTime-startTime) << " seconds." << endl; //execution time
    }
    else
    {
        cout << "File not open" << endl;
    }
    return 0;
}

