#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
using namespace std;
#define MAX_LINE_LENGTH 1000
#define MAX_LINES 1000

// Funkcija f1: dolžina najdaljše besede v nizu
int f1(const string &str)
{
    // Returns length of longest word
    int max = 0, current = 0;
    for (char c : str)
    {
        if (c != ' ')
            current++;
        else
        {
            if (current > max)
                max = current;
            current = 0;
        }
    }
    if (current > max)
        max = current;
    return max;
}

// Funkcija f2: število samoglasnikov
int f2(const string &str)
{
    // Returns number of vowels
    int count = 0;
    for (char c : str)
    {
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U')
        {
            count++;
        }
    }
    return count;
}

// Funkcija f3: število presledkov
int f3(const string &str)
{
    // Returns number of spaces
    int count = 0;
    for (char c : str)
    {
        if (c == ' ')
            count++;
    }
    return count;
}

int main(int argc, char **argv)
{
    int rank, size;
    double start_time, end_time;

    int num_lines = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<string> lines;
    int numLines;

    // Process 0 reads input
    if (rank == 0)
    {
        cout << "Enter number of lines: ";
        cin >> numLines;
        cin.ignore(); // Clear the newline

        cout << "Enter " << numLines << " lines of text:\n";
        for (int i = 0; i < numLines; i++)
        {
            string line;
            getline(cin, line);
            lines.push_back(line);
        }
    }

    start_time = MPI_Wtime();
    MPI_Bcast(&numLines, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        lines.resize(numLines);
    }

    for (int i = 0; i < numLines; i++)
    {
        if (rank == 0)
        {
            int len = lines[i].length() + 1;
            MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast((void *)lines[i].c_str(), len, MPI_CHAR, 0, MPI_COMM_WORLD);
        }
        else
        {
            int len;
            MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
            char *buffer = new char[len];
            MPI_Bcast(buffer, len, MPI_CHAR, 0, MPI_COMM_WORLD);
            lines[i] = string(buffer);
            delete[] buffer;
        }
    }

    // Each process processes all lines with its assigned function
    int local_sum = 0;
    for (const auto &line : lines)
    {
        int result = 0;
        if (rank == 0)
            result = f1(line);
        else if (rank == 1)
            result = f2(line);
        else if (rank == 2)
            result = f3(line);
        local_sum += result;
    }

    // Combine results
    int total_sum;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "Total sum of all functions: " << total_sum << endl;
        cout << "Time: " << MPI_Wtime() - start_time << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}