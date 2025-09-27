#define _USE_MATH_DEFINES
#include <cmath>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <windows.h>

using namespace cv;
using namespace std;

// Configurar consola para colores
void setupConsole() {
    SetConsoleOutputCP(65001); // UTF-8
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

// Códigos de colores ANSI
namespace Colors {
    const string RESET = "\033[0m";
    const string BOLD = "\033[1m";
    const string DIM = "\033[2m";
    
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";
    const string WHITE = "\033[37m";
    
    const string BRIGHT_BLACK = "\033[90m";
    const string BRIGHT_RED = "\033[91m";
    const string BRIGHT_GREEN = "\033[92m";
    const string BRIGHT_YELLOW = "\033[93m";
    const string BRIGHT_BLUE = "\033[94m";
    const string BRIGHT_MAGENTA = "\033[95m";
    const string BRIGHT_CYAN = "\033[96m";
    const string BRIGHT_WHITE = "\033[97m";
}

// Funciones de interfaz
class CyberInterface {
public:
    static void clearScreen() {
        system("cls");
    }
    
    static void printHeader() {
        cout << Colors::BRIGHT_CYAN << Colors::BOLD;
        cout << "================================================================================" << endl;
        cout << "                   ANALIZADOR DE PIEL DE CARNERO                              " << endl;
        cout << "            ALGORITMOS GENETICOS + VISION ARTIFICIAL                          " << endl;
        cout << "================================================================================" << endl;
        cout << "  >>> Sistema de Optimizacion Inteligente para Cortes Circulares <<<        " << endl;
        cout << "================================================================================" << Colors::RESET << endl;
        cout << endl;
    }
    
    static void printSection(const string& title) {
        cout << Colors::BRIGHT_GREEN << Colors::BOLD;
        cout << ">>> " << title << " <<<" << Colors::RESET << endl;
        cout << Colors::CYAN;
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << Colors::RESET << endl;
    }
    
    static void printProgressBar(int percentage, const string& task, int width = 50) {
        // Solo imprimir el título la primera vez o cuando cambie
        static string lastTask = "";
        static bool taskShown = false;
        
        if (task != lastTask) {
            cout << Colors::BRIGHT_YELLOW << ">> " << task << Colors::RESET << endl;
            lastTask = task;
            taskShown = true;
        }
        
        // Limpiar la línea actual completamente
        cout << "\r" << string(80, ' ') << "\r";
        
        // Crear barra más estética con caracteres ASCII compatibles
        cout << Colors::CYAN << "[";
        int filled = (percentage * width) / 100;
        
        for (int i = 0; i < width; i++) {
            if (i < filled) {
                cout << Colors::BRIGHT_GREEN << "="; // Carácter sólido ASCII
            } else {
                cout << Colors::BRIGHT_BLACK << "."; // Carácter ligero ASCII
            }
        }
        
        cout << Colors::CYAN << "] " << Colors::BRIGHT_WHITE << Colors::BOLD;
        cout << setw(3) << percentage << "%" << Colors::RESET;
        
        // Agregar indicador de velocidad con caracteres ASCII
        if (percentage < 100) {
            cout << Colors::BRIGHT_CYAN << " >>>";
        } else {
            cout << Colors::BRIGHT_GREEN << " OK";
            cout << endl;
            taskShown = false; // Reset para la próxima tarea
        }
        
        cout.flush(); // Forzar actualización inmediata
    }
    
    static void printStatus(const string& message, const string& color = Colors::BRIGHT_WHITE) {
        cout << color << "* " << message << Colors::RESET << endl;
    }
    
    static void printSuccess(const string& message) {
        cout << Colors::BRIGHT_GREEN << "[OK] " << message << Colors::RESET << endl;
    }
    
    static void printError(const string& message) {
        cout << Colors::BRIGHT_RED << "[ERROR] " << message << Colors::RESET << endl;
    }
    
    static void printWarning(const string& message) {
        cout << Colors::BRIGHT_YELLOW << "[WARNING] " << message << Colors::RESET << endl;
    }
    
    static void printInfo(const string& message) {
        cout << Colors::BRIGHT_CYAN << "[INFO] " << message << Colors::RESET << endl;
    }
    
    static void printDataTable(const vector<vector<string>>& data, const vector<string>& headers) {
        if (data.empty() || headers.empty()) return;
        
        // Calcular anchos de columna
        vector<int> widths(headers.size(), 0);
        for (size_t i = 0; i < headers.size(); i++) {
            widths[i] = static_cast<int>(headers[i].length());
        }
        
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size() && i < widths.size(); i++) {
                if (row[i].length() > static_cast<size_t>(widths[i])) {
                    widths[i] = static_cast<int>(row[i].length());
                }
            }
        }
        
        // Imprimir tabla
        cout << Colors::BRIGHT_CYAN;
        cout << "+";
        for (size_t i = 0; i < widths.size(); i++) {
            cout << string(widths[i] + 2, '-');
            if (i < widths.size() - 1) cout << "+";
        }
        cout << "+" << endl;
        
        // Headers
        cout << "|";
        for (size_t i = 0; i < headers.size(); i++) {
            cout << " " << Colors::BRIGHT_WHITE << Colors::BOLD << left << setw(widths[i]) << headers[i] << Colors::BRIGHT_CYAN << " |";
        }
        cout << endl;
        
        // Separador
        cout << "+";
        for (size_t i = 0; i < widths.size(); i++) {
            cout << string(widths[i] + 2, '-');
            if (i < widths.size() - 1) cout << "+";
        }
        cout << "+" << endl;
        
        // Datos
        for (const auto& row : data) {
            cout << "|";
            for (size_t i = 0; i < headers.size(); i++) {
                string cell = (i < row.size()) ? row[i] : "";
                cout << " " << Colors::BRIGHT_WHITE << left << setw(widths[i]) << cell << Colors::BRIGHT_CYAN << " |";
            }
            cout << endl;
        }
        
        // Footer
        cout << "+";
        for (size_t i = 0; i < widths.size(); i++) {
            cout << string(widths[i] + 2, '-');
            if (i < widths.size() - 1) cout << "+";
        }
        cout << "+" << Colors::RESET << endl;
    }
    
    static void animateText(const string& text, int delayMs = 30) {
        for (char c : text) {
            cout << c;
            cout.flush();
            this_thread::sleep_for(chrono::milliseconds(delayMs));
        }
        cout << endl;
    }
    
    static void showLoadingAnimation(const string& message, int durationMs = 2000) {
        cout << Colors::BRIGHT_YELLOW << "> " << message;
        
        const string chars = "|/-\\";
        int steps = durationMs / 100;
        
        for (int i = 0; i < steps; i++) {
            // Usar \r para volver al inicio de la línea sin nueva línea
            cout << " " << Colors::BRIGHT_CYAN << chars[i % chars.length()] << "\r";
            cout << Colors::BRIGHT_YELLOW << "> " << message;
            cout.flush();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        
        // Limpiar la línea y mostrar resultado final
        cout << "\r" << Colors::BRIGHT_YELLOW << "> " << message;
        cout << Colors::BRIGHT_GREEN << " [DONE]" << Colors::RESET << endl;
    }
    
    // Función especial para progreso del algoritmo genético - más rápida y estética
    static void printGeneticProgressBar(int percentage, int generation, int maxGenerations, double fitness) {
        // Limpiar línea completamente
        cout << "\r" << string(100, ' ') << "\r";
        
        // Barra de progreso compacta y elegante con caracteres ASCII
        int width = 30; // Más compacta para mejor rendimiento
        int filled = (percentage * width) / 100;
        
        cout << Colors::BRIGHT_MAGENTA << "<" << Colors::BRIGHT_CYAN;
        for (int i = 0; i < width; i++) {
            if (i < filled) {
                cout << Colors::BRIGHT_GREEN << "#"; // Barra ASCII compatible
            } else {
                cout << Colors::DIM << "-";
            }
        }
        cout << Colors::BRIGHT_MAGENTA << "> ";
        
        // Información compacta en una sola línea
        cout << Colors::BRIGHT_WHITE << setw(3) << percentage << "% " 
             << Colors::YELLOW << "Gen:" << Colors::WHITE << generation 
             << Colors::CYAN << "/" << Colors::WHITE << maxGenerations
             << Colors::GREEN << " F:" << Colors::BRIGHT_WHITE << fixed << setprecision(1) << fitness;
        
        if (percentage >= 100) {
            cout << Colors::BRIGHT_GREEN << " [COMPLETE]" << endl;
        }
        
        cout.flush();
    }
};

// Constantes del problema
const double PIXELS_PER_CM = 5.0;  // 5 pixeles = 1 cm

// Variables globales para permitir cambios durante la ejecución
double CIRCLE_RADIUS_CM = 10.0;  // Radio de 10 cm (diametro 20 cm)
int CIRCLE_RADIUS_PIXELS = static_cast<int>(CIRCLE_RADIUS_CM * PIXELS_PER_CM);  // 50 pixeles

// Parametros del algoritmo genetico
const int POPULATION_SIZE = 100;
const int MAX_GENERATIONS = 500;
const double MUTATION_RATE = 0.05;
const double CROSSOVER_RATE = 0.8;

// Estructura para representar un círculo
struct Circle {
    int x, y;  // Centro del círculo
    double fitness;
    
    Circle() : x(0), y(0), fitness(0.0) {}
    Circle(int _x, int _y) : x(_x), y(_y), fitness(0.0) {}
};

class SkinAnalyzer {
private:
    Mat originalImage;
    Mat binaryImage;
    Mat resultImage;
    vector<Circle> bestSolution;
    int imageWidth, imageHeight;
    
public:
    // Constructor
    SkinAnalyzer() : imageWidth(0), imageHeight(0) {}
    
    // Cargar y procesar la imagen
    bool loadImage(const string& imagePath) {
        CyberInterface::printSection("CARGA Y ANALISIS DE IMAGEN");
        
        CyberInterface::showLoadingAnimation("Cargando imagen desde disco", 800); // Reducido de 1500ms
        
        originalImage = imread(imagePath, IMREAD_COLOR);
        if (originalImage.empty()) {
            CyberInterface::printError("Error: No se pudo cargar la imagen desde: " + imagePath);
            return false;
        }
        
        imageWidth = originalImage.cols;
        imageHeight = originalImage.rows;
        
        CyberInterface::printSuccess("Imagen cargada exitosamente");
        CyberInterface::printInfo("Dimensiones: " + to_string(imageWidth) + "x" + to_string(imageHeight) + " pixeles");
        CyberInterface::printInfo("Tamaño real: " + to_string(imageWidth / PIXELS_PER_CM) + "x" + to_string(imageHeight / PIXELS_PER_CM) + " cm");
        
        cout << endl;
        return true;
    }
    
    // Realizar umbralización para obtener imagen binaria
    void performThresholding() {
        CyberInterface::printSection("PROCESAMIENTO DE IMAGEN");
        
        Mat grayImage;
        
        // Procesamiento ultra-rápido con delays mínimos
        CyberInterface::printProgressBar(20, "Convirtiendo a escala de grises");
        cvtColor(originalImage, grayImage, COLOR_BGR2GRAY);
        this_thread::sleep_for(chrono::milliseconds(50));
        
        CyberInterface::printProgressBar(40, "Aplicando umbralizacion binaria");
        threshold(grayImage, binaryImage, 200, 255, THRESH_BINARY_INV);
        this_thread::sleep_for(chrono::milliseconds(50));
        
        CyberInterface::printProgressBar(70, "Aplicando operaciones morfologicas");
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
        morphologyEx(binaryImage, binaryImage, MORPH_CLOSE, kernel);
        this_thread::sleep_for(chrono::milliseconds(50));
        
        CyberInterface::printProgressBar(90, "Finalizando procesamiento");
        morphologyEx(binaryImage, binaryImage, MORPH_OPEN, kernel);
        this_thread::sleep_for(chrono::milliseconds(50));
        
        CyberInterface::printProgressBar(100, "Procesamiento completado");
        CyberInterface::printSuccess("Umbralizacion completada exitosamente");
        
        cout << endl;
    }
    
    // Guardar imagen binaria como PNG y CSV
    void saveBinaryResults(const string& basePath) {
        CyberInterface::printSection("EXPORTACION DE RESULTADOS");
        
        // Guardar imagen PNG
        string pngPath = basePath + "_bin.png";
        CyberInterface::showLoadingAnimation("Guardando imagen binaria (PNG)", 300);
        imwrite(pngPath, binaryImage);
        CyberInterface::printSuccess("Imagen binaria guardada: " + pngPath);
        
        // Guardar como CSV con buffer optimizado
        string csvPath = basePath + "_bin.csv";
        CyberInterface::showLoadingAnimation("Preparando exportacion CSV", 300);
        
        ofstream csvFile(csvPath);
        if (csvFile.is_open()) {
            // Buffer para escribir de forma más eficiente
            csvFile.sync_with_stdio(false);
            
            int totalPixels = binaryImage.rows * binaryImage.cols;
            int processedPixels = 0;
            int updateInterval = totalPixels / 5; // Solo 5 actualizaciones total
            
            cout << Colors::BRIGHT_YELLOW << ">> Escribiendo datos CSV" << Colors::RESET << endl;
            
            for (int i = 0; i < binaryImage.rows; i++) {
                for (int j = 0; j < binaryImage.cols; j++) {
                    csvFile << static_cast<int>(binaryImage.at<uchar>(i, j));
                    if (j < binaryImage.cols - 1) csvFile << ",";
                    
                    processedPixels++;
                    // Actualizar muy poco frecuentemente para máxima velocidad
                    if (processedPixels % updateInterval == 0) {
                        int progress = (processedPixels * 100) / totalPixels;
                        CyberInterface::printProgressBar(progress, "Escribiendo CSV");
                    }
                }
                csvFile << "\n";
            }
            csvFile.close();
            
            CyberInterface::printProgressBar(100, "Escribiendo CSV");
            CyberInterface::printSuccess("Datos CSV guardados: " + csvPath);
        }
        
        cout << endl;
    }
    
    // Verificar si un círculo está completamente dentro del área de piel
    bool isCircleValid(const Circle& circle) {
        int radius = CIRCLE_RADIUS_PIXELS;
        
        // Verificar límites de la imagen
        if (circle.x - radius < 0 || circle.x + radius >= imageWidth ||
            circle.y - radius < 0 || circle.y + radius >= imageHeight) {
            return false;
        }
        
        // Verificar si todos los píxeles del círculo están en área de piel
        for (int y = circle.y - radius; y <= circle.y + radius; y++) {
            for (int x = circle.x - radius; x <= circle.x + radius; x++) {
                int distance = static_cast<int>(sqrt((x - circle.x) * (x - circle.x) + (y - circle.y) * (y - circle.y)));
                if (distance <= radius) {
                    if (binaryImage.at<uchar>(y, x) == 0) {  // Píxel blanco (fondo)
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    // Verificar si dos círculos se superponen
    bool circlesOverlap(const Circle& c1, const Circle& c2) {
        double distance = sqrt((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
        return distance < (2 * CIRCLE_RADIUS_PIXELS);
    }
    
    // Calcular fitness de un individuo
    double calculateFitness(const vector<Circle>& circles) {
        double fitness = 0.0;
        
        for (size_t i = 0; i < circles.size(); i++) {
            if (!isCircleValid(circles[i])) {
                continue;
            }
            
            bool overlaps = false;
            for (size_t j = i + 1; j < circles.size(); j++) {
                if (circlesOverlap(circles[i], circles[j])) {
                    overlaps = true;
                    break;
                }
            }
            
            if (!overlaps) {
                fitness += 1.0;
            } else {
                fitness -= 0.5;
            }
        }
        
        return fitness;
    }
    
    // Generar población inicial
    vector<vector<Circle>> generateInitialPopulation() {
        vector<vector<Circle>> population;
        population.reserve(POPULATION_SIZE); // Reservar memoria
        
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> xDist(CIRCLE_RADIUS_PIXELS, imageWidth - CIRCLE_RADIUS_PIXELS);
        uniform_int_distribution<> yDist(CIRCLE_RADIUS_PIXELS, imageHeight - CIRCLE_RADIUS_PIXELS);
        uniform_int_distribution<> sizeDist(1, 20);
        
        CyberInterface::showLoadingAnimation("Generando poblacion inicial", 600); // Más rápido
        
        cout << Colors::BRIGHT_YELLOW << ">> Creando individuos" << Colors::RESET << endl;
        
        for (int i = 0; i < POPULATION_SIZE; i++) {
            vector<Circle> individual;
            int numCircles = sizeDist(gen);
            individual.reserve(numCircles); // Reservar memoria
            
            for (int j = 0; j < numCircles; j++) {
                individual.emplace_back(xDist(gen), yDist(gen));
            }
            
            population.push_back(move(individual));
            
            // Actualizar cada 10% para mayor velocidad
            if (i % (POPULATION_SIZE / 10) == 0) {
                int progress = (i * 100) / POPULATION_SIZE;
                CyberInterface::printProgressBar(progress, "Creando individuos");
            }
        }
        
        CyberInterface::printProgressBar(100, "Creando individuos");
        return population;
    }
    
    // Cruzamiento de dos individuos
    vector<Circle> crossover(const vector<Circle>& parent1, const vector<Circle>& parent2) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(0.0, 1.0);
        
        vector<Circle> offspring;
        
        for (const auto& circle : parent1) {
            if (dist(gen) < 0.5) {
                offspring.push_back(circle);
            }
        }
        
        for (const auto& circle : parent2) {
            if (dist(gen) < 0.5) {
                offspring.push_back(circle);
            }
        }
        
        return offspring;
    }
    
    // Mutación de un individuo
    void mutate(vector<Circle>& individual) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(0.0, 1.0);
        uniform_int_distribution<> xDist(CIRCLE_RADIUS_PIXELS, imageWidth - CIRCLE_RADIUS_PIXELS);
        uniform_int_distribution<> yDist(CIRCLE_RADIUS_PIXELS, imageHeight - CIRCLE_RADIUS_PIXELS);
        
        for (auto& circle : individual) {
            if (dist(gen) < MUTATION_RATE) {
                circle.x = xDist(gen);
                circle.y = yDist(gen);
            }
        }
        
        if (dist(gen) < MUTATION_RATE && individual.size() < 50) {
            Circle newCircle(xDist(gen), yDist(gen));
            individual.push_back(newCircle);
        }
        
        if (dist(gen) < MUTATION_RATE && individual.size() > 1) {
            uniform_int_distribution<> indexDist(0, static_cast<int>(individual.size()) - 1);
            individual.erase(individual.begin() + indexDist(gen));
        }
    }
    
    // Selección por torneo
    int selectByTournament(const vector<double>& fitness) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, POPULATION_SIZE - 1);
        
        int best = dist(gen);
        for (int i = 0; i < 3; i++) {
            int candidate = dist(gen);
            if (fitness[candidate] > fitness[best]) {
                best = candidate;
            }
        }
        return best;
    }
    
    // Ejecutar algoritmo genético
    void runGeneticAlgorithm() {
        CyberInterface::printSection("ALGORITMO GENETICO - OPTIMIZACION");
        
        CyberInterface::printInfo("Poblacion: " + to_string(POPULATION_SIZE) + " individuos");
        CyberInterface::printInfo("Generaciones maximas: " + to_string(MAX_GENERATIONS));
        CyberInterface::printInfo("Tasa de mutacion: " + to_string(MUTATION_RATE * 100) + "%");
        CyberInterface::printInfo("Tasa de cruzamiento: " + to_string(CROSSOVER_RATE * 100) + "%");
        cout << endl;
        
        vector<vector<Circle>> population = generateInitialPopulation();
        vector<double> fitness(POPULATION_SIZE);
        
        double bestFitness = 0.0;
        double prevBestFitness = 0.0;
        
        CyberInterface::printStatus("Iniciando evolucion...", Colors::BRIGHT_MAGENTA);
        
        // Mostrar progreso inicial
        cout << Colors::BRIGHT_YELLOW << ">> Evolucion en progreso" << Colors::RESET << endl;
        
        for (int generation = 0; generation < MAX_GENERATIONS; generation++) {
            // Evaluar fitness
            for (int i = 0; i < POPULATION_SIZE; i++) {
                fitness[i] = calculateFitness(population[i]);
                if (fitness[i] > bestFitness) {
                    bestFitness = fitness[i];
                    bestSolution = population[i];
                }
            }
            
            // Actualizar progreso cada 5 generaciones para máxima velocidad
            if (generation % 5 == 0 || generation == MAX_GENERATIONS - 1) {
                int progress = (generation * 100) / MAX_GENERATIONS;
                CyberInterface::printGeneticProgressBar(progress, generation, MAX_GENERATIONS, bestFitness);
            }
            
            // Mostrar mejoras significativas solo si hay un salto grande
            if (generation % 100 == 0 && abs(bestFitness - prevBestFitness) > 0.5) {
                cout << endl << Colors::BRIGHT_GREEN << ">>> Mejora significativa: " 
                     << bestFitness << Colors::RESET << endl;
                cout << Colors::BRIGHT_YELLOW << ">> Evolucion en progreso" << Colors::RESET << endl;
                prevBestFitness = bestFitness;
            }
            
            // Reproducción optimizada
            vector<vector<Circle>> newPopulation;
            newPopulation.reserve(POPULATION_SIZE);
            
            // Elitismo optimizado
            vector<pair<double, int>> sortedFitness;
            sortedFitness.reserve(POPULATION_SIZE);
            for (int i = 0; i < POPULATION_SIZE; i++) {
                sortedFitness.emplace_back(fitness[i], i);
            }
            sort(sortedFitness.rbegin(), sortedFitness.rend());
            
            int eliteSize = POPULATION_SIZE / 5;
            for (int i = 0; i < eliteSize; i++) {
                newPopulation.push_back(population[sortedFitness[i].second]);
            }
            
            // Cruzamiento y mutación optimizado
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> dist(0.0, 1.0);
            
            while (newPopulation.size() < static_cast<size_t>(POPULATION_SIZE)) {
                int parent1 = selectByTournament(fitness);
                int parent2 = selectByTournament(fitness);
                
                vector<Circle> offspring;
                if (dist(gen) < CROSSOVER_RATE) {
                    offspring = crossover(population[parent1], population[parent2]);
                } else {
                    offspring = population[parent1];
                }
                
                mutate(offspring);
                newPopulation.push_back(move(offspring));
            }
            
            population = move(newPopulation);
        }
        
        // Progreso final
        CyberInterface::printGeneticProgressBar(100, MAX_GENERATIONS, MAX_GENERATIONS, bestFitness);
        cout << endl;
        CyberInterface::printSuccess("Algoritmo genetico finalizado exitosamente");
        CyberInterface::printStatus("Fitness final: " + to_string(bestFitness), Colors::BRIGHT_GREEN);
        cout << endl;
    }
    
    // Mostrar resultados finales
    void showResults() {
        CyberInterface::printSection("RESULTADOS DEL ANALISIS");
        
        // Filtrar círculos válidos
        vector<Circle> validCircles;
        for (const auto& circle : bestSolution) {
            if (isCircleValid(circle)) {
                bool overlaps = false;
                for (const auto& validCircle : validCircles) {
                    if (circlesOverlap(circle, validCircle)) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    validCircles.push_back(circle);
                }
            }
        }
        
        // Mostrar estadísticas principales
        CyberInterface::printSuccess("Analisis completado exitosamente");
        CyberInterface::printInfo("Circulos optimos encontrados: " + to_string(validCircles.size()));
        CyberInterface::printInfo("Radio de cada circulo: " + to_string(CIRCLE_RADIUS_CM) + " cm (" + to_string(CIRCLE_RADIUS_PIXELS) + " pixeles)");
        
        double areaPerCircle = M_PI * CIRCLE_RADIUS_CM * CIRCLE_RADIUS_CM;
        double totalArea = validCircles.size() * areaPerCircle;
        
        CyberInterface::printInfo("Area por circulo: " + to_string(areaPerCircle) + " cm²");
        CyberInterface::printInfo("Area total aprovechada: " + to_string(totalArea) + " cm²");
        
        cout << endl;
        
        // Crear tabla de coordenadas
        if (!validCircles.empty()) {
            vector<string> headers = {"ID", "X(px)", "Y(px)", "X(cm)", "Y(cm)"};
            vector<vector<string>> data;
            
            for (size_t i = 0; i < validCircles.size(); i++) {
                double x_cm = validCircles[i].x / PIXELS_PER_CM;
                double y_cm = validCircles[i].y / PIXELS_PER_CM;
                
                vector<string> row = {
                    to_string(i + 1),
                    to_string(validCircles[i].x),
                    to_string(validCircles[i].y),
                    to_string(x_cm).substr(0, 5),
                    to_string(y_cm).substr(0, 5)
                };
                data.push_back(row);
            }
            
            cout << Colors::BRIGHT_WHITE << Colors::BOLD << "TABLA DE COORDENADAS DE CENTROS" << Colors::RESET << endl;
            CyberInterface::printDataTable(data, headers);
        }
        
        // Crear imagen resultado
        createResultImage(validCircles);
        cout << endl;
    }
    
    // Crear imagen con los círculos marcados
    void createResultImage(const vector<Circle>& circles) {
        CyberInterface::showLoadingAnimation("Generando imagen de resultados", 1500);
        
        resultImage = originalImage.clone();
        
        for (size_t i = 0; i < circles.size(); i++) {
            // Dibujar círculo
            circle(resultImage, Point(circles[i].x, circles[i].y), CIRCLE_RADIUS_PIXELS, 
                   Scalar(0, 0, 255), 3);
            
            // Dibujar centro
            circle(resultImage, Point(circles[i].x, circles[i].y), 5, 
                   Scalar(0, 255, 0), -1);
            
            // Agregar número
            putText(resultImage, to_string(i + 1), 
                    Point(circles[i].x - 10, circles[i].y + 5),
                    FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        }
        
        imwrite("piel_carnero_resultado.png", resultImage);
        CyberInterface::printSuccess("Imagen con circulos guardada: piel_carnero_resultado.png");
    }
    
    // Analizar con diferente diámetro
    void analyzeWithDifferentRadius(double newRadiusCm) {
        cout << endl << endl;
        CyberInterface::printSection("ANALISIS COMPARATIVO - DIAMETRO " + to_string(newRadiusCm * 2) + " CM");
        
        double originalRadius = CIRCLE_RADIUS_CM;
        int originalPixels = CIRCLE_RADIUS_PIXELS;
        
        CIRCLE_RADIUS_CM = newRadiusCm;
        CIRCLE_RADIUS_PIXELS = static_cast<int>(newRadiusCm * PIXELS_PER_CM);
        
        CyberInterface::printInfo("Cambiando parametros de analisis...");
        CyberInterface::printInfo("Nuevo radio: " + to_string(newRadiusCm) + " cm");
        CyberInterface::printInfo("Nuevo diametro: " + to_string(newRadiusCm * 2) + " cm");
        cout << endl;
        
        runGeneticAlgorithm();
        showResults();
        
        CIRCLE_RADIUS_CM = originalRadius;
        CIRCLE_RADIUS_PIXELS = originalPixels;
    }
    
    // Mostrar ventanas de visualización
    void displayImages() {
        CyberInterface::printSection("VISUALIZACION DE RESULTADOS");
        
        if (!originalImage.empty()) {
            CyberInterface::printStatus("Mostrando imagen original...");
            namedWindow("Imagen Original", WINDOW_AUTOSIZE);
            imshow("Imagen Original", originalImage);
        }
        
        if (!binaryImage.empty()) {
            CyberInterface::printStatus("Mostrando imagen binaria...");
            namedWindow("Imagen Binaria", WINDOW_AUTOSIZE);
            imshow("Imagen Binaria", binaryImage);
        }
        
        if (!resultImage.empty()) {
            CyberInterface::printStatus("Mostrando resultado con circulos...");
            namedWindow("Resultado con Circulos", WINDOW_AUTOSIZE);
            imshow("Resultado con Circulos", resultImage);
        }
        
        CyberInterface::printWarning("Presiona cualquier tecla para cerrar las ventanas...");
        waitKey(0);
        destroyAllWindows();
        CyberInterface::printSuccess("Ventanas cerradas");
    }
};

int main() {
    // Configurar consola para UTF-8
    setupConsole();
    
    CyberInterface::clearScreen();
    CyberInterface::printHeader();
    
    CyberInterface::animateText("Iniciando sistema de analisis inteligente...", 25); // Reducido de 50ms a 25ms
    cout << endl;
    
    SkinAnalyzer analyzer;
    
    // 1. Cargar la imagen
    string imagePath = "piel_carnero.png";
    if (!analyzer.loadImage(imagePath)) {
        CyberInterface::printError("Asegurate de que el archivo 'piel_carnero.png' este en el directorio del proyecto.");
        CyberInterface::printInfo("Presiona Enter para salir...");
        cin.get();
        return -1;
    }
    
    // 2. Realizar umbralización
    analyzer.performThresholding();
    
    // 3. Guardar resultados de umbralización
    analyzer.saveBinaryResults("piel_carnero");
    
    // 4. Ejecutar algoritmo genético para círculos de 20 cm de diámetro
    analyzer.runGeneticAlgorithm();
    analyzer.showResults();
    
    // 5. Análisis adicional con círculos de 25 cm de diámetro
    analyzer.analyzeWithDifferentRadius(12.5);
    
    // 6. Mostrar imágenes
    analyzer.displayImages();
    
    // Resumen final
    CyberInterface::printSection("PROCESO COMPLETADO");
    CyberInterface::printSuccess("Analisis completado exitosamente");
    CyberInterface::printInfo("Archivos generados:");
    CyberInterface::printStatus("  - piel_carnero_bin.png (imagen binaria)", Colors::BRIGHT_CYAN);
    CyberInterface::printStatus("  - piel_carnero_bin.csv (datos en CSV)", Colors::BRIGHT_CYAN);
    CyberInterface::printStatus("  - piel_carnero_resultado.png (imagen con circulos)", Colors::BRIGHT_CYAN);
    
    cout << endl;
    CyberInterface::printStatus("Sistema finalizado. Gracias por usar el analizador!", Colors::BRIGHT_MAGENTA);
    
    cout << Colors::BRIGHT_WHITE << "\nPresiona Enter para salir..." << Colors::RESET;
    cin.get();
    
    return 0;
}