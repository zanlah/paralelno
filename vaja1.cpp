#include <iostream>
#include <vector>
#include <omp.h>

// Izračun zmnožka vsot po vrsticah
double calculate_row_product(const std::vector<std::vector<double>>& matrix) {
    double result = 1.0;
    int n = matrix.size();
    std::vector<double> row_sums(n, 0.0);

    // Izračunaj vsote vrstic
    #pragma omp parallel for
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            row_sums[i] += matrix[i][j];
        }
    }

    // Zmnoži vsote
    for(int i = 0; i < n; i++) {
        result *= row_sums[i];
    }
    return result;
}

// Izračun zmnožka vsot po stolpcih
double calculate_column_product(const std::vector<std::vector<double>>& matrix) {
    double result = 1.0;
    int n = matrix.size();
    std::vector<double> col_sums(n, 0.0);

    // Izračunaj vsote stolpcev
    #pragma omp parallel for collapse(2)
    for(int j = 0; j < n; j++) {
        for(int i = 0; i < n; i++) {
            #pragma omp atomic
            col_sums[j] += matrix[i][j];
        }
    }

    // Zmnoži vsote
    for(int j = 0; j < n; j++) {
        result *= col_sums[j];
    }
    return result;
}

int main() {
    // Branje velikosti matrike
    int n;
    std::cout << "Vnesite velikost matrike (N x N): ";
    std::cin >> n;

    // Inicializacija matrike
    std::vector<std::vector<double>> matrix(n, std::vector<double>(n));
    
    // Vnos elementov matrike
    std::cout << "Vnesite elemente matrike:\n";
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            std::cin >> matrix[i][j];
        }
    }

    // Izpis vhodne matrike
    std::cout << "\nVHOD:\n";
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }

    // Začni merjenje časa
    double start_time = omp_get_wtime();

    // Paralelno izračunaj obe funkciji
    double f1, f2;
    #pragma omp parallel sections
    {
        #pragma omp section
        f1 = calculate_row_product(matrix);

        #pragma omp section
        f2 = calculate_column_product(matrix);
    }

    // Končni rezultat
    double result = f1 + f2;

    // Končaj merjenje časa
    double end_time = omp_get_wtime();

    // Izpis rezultatov
    std::cout << "\nIZHOD:\n";
    std::cout << "f1 = " << f1 << "\n";
    std::cout << "f2 = " << f2 << "\n";
    std::cout << "Rezultat = " << result << "\n";
    std::cout << "Čas izvajanja: " << (end_time - start_time) << " sekund\n";

    return 0;
}