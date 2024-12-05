#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
using namespace std;
#define MAX_LINE_LENGTH 1000

// dolzina najdaljše besede v nizu
int f1(const string &str)
{

    int max = 0;
    int current = 0;
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

// stevilo samoglasnikov
int f2(const string &str)
{

    int st = 0;
    for (char c : str)
    {
        c = tolower(c);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
        {
            st++;
        }
    }
    return st;
}

// stevilo presledkov
int f3(const string &str)
{
    int st = 0;
    for (char c : str)
    {
        if (c == ' ')
            st++;
    }
    return st;
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

    if (rank == 0)
    {
        cout << "Vnesi stevilo vrstic: ";
        string temp;
        getline(cin, temp);
        numLines = stoi(temp);

        cout << "Vnesi " << numLines << " vrstic besedila:\n";
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

        char buffer[MAX_LINE_LENGTH];
        if (rank == 0)
        {
            strncpy(buffer, lines[i].c_str(), MAX_LINE_LENGTH - 1); // Kopiramo vrstico v buffer
            buffer[MAX_LINE_LENGTH - 1] = '\0';                     // Znak za konec niza, brez tega je prihajalo do tezav
        }

        MPI_Bcast(buffer, MAX_LINE_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

        if (rank != 0)
        {
            lines[i] = string(buffer); // Pretvorimo nazaj v string
        }
    }

    // vsako vrstico obdelamo z razlicno funkcijo v razlicnem procesu
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

    int total_sum;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "sestevek: " << total_sum << endl;
        cout << "cas: " << MPI_Wtime() - start_time << " s" << endl;
    }

    MPI_Finalize();
    return 0;
}