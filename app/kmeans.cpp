#include <vector>
#include <random>
#include <limits>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

//g++ -O2 -Wall -shared -std=c++20 -fPIC `python3.12 -m pybind11 --includes` kmeans.cpp -o kmeans`python3.12-config --extension-suffix`
//g++ -O3 -Wall -shared -std=c++11 -fPIC $(python3.12 -m pybind11 --includes) kmeans.cpp -o kmeans$(python3.12-config --extension-suffix)

namespace py = pybind11;

class KMeans {
private:
    int n_clusters;
    int max_iter;
    double tolerance;
    std::vector<std::vector<double>> centroids;
    
    double squared_euclidean_distance(const std::vector<double>& a, const std::vector<double>& b) {
        double dist = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            double diff = a[i] - b[i];
            dist += diff * diff;
        }
        return dist;
    }

public:
    KMeans(int n_clusters = 3, int max_iter = 100, double tolerance = 1e-4)
        : n_clusters(n_clusters), max_iter(max_iter), tolerance(tolerance) {}
    
    void fit(const std::vector<std::vector<double>>& data) {
        // Inicialización aleatoria de centroides
        std::vector<size_t> indices(data.size());
        for (size_t i = 0; i < data.size(); ++i) indices[i] = i;
        std::random_shuffle(indices.begin(), indices.end());
        
        centroids.clear();
        for (int i = 0; i < n_clusters; ++i) {
            centroids.push_back(data[indices[i]]);
        }
        
        for (int iter = 0; iter < max_iter; ++iter) {
            // Asignación de clusters
            std::vector<std::vector<double>> new_centroids(n_clusters, std::vector<double>(data[0].size(), 0.0));
            std::vector<int> counts(n_clusters, 0);
            
            for (const auto& point : data) {
                double min_dist = std::numeric_limits<double>::max();
                int cluster = 0;
                
                for (int i = 0; i < n_clusters; ++i) {
                    double dist = squared_euclidean_distance(point, centroids[i]);
                    if (dist < min_dist) {
                        min_dist = dist;
                        cluster = i;
                    }
                }
                
                for (size_t j = 0; j < point.size(); ++j) {
                    new_centroids[cluster][j] += point[j];
                }
                counts[cluster]++;
            }
            
            // Actualización de centroides
            double max_shift = 0.0;
            for (int i = 0; i < n_clusters; ++i) {
                if (counts[i] == 0) continue;
                
                std::vector<double> old = centroids[i];
                for (size_t j = 0; j < new_centroids[i].size(); ++j) {
                    centroids[i][j] = new_centroids[i][j] / counts[i];
                }
                
                double shift = squared_euclidean_distance(old, centroids[i]);
                if (shift > max_shift) max_shift = shift;
            }
            
            if (max_shift < tolerance) break;
        }
    }
    
    std::vector<int> predict(const std::vector<std::vector<double>>& data) {
        std::vector<int> labels;
        for (const auto& point : data) {
            double min_dist = std::numeric_limits<double>::max();
            int cluster = 0;
            
            for (int i = 0; i < n_clusters; ++i) {
                double dist = squared_euclidean_distance(point, centroids[i]);
                if (dist < min_dist) {
                    min_dist = dist;
                    cluster = i;
                }
            }
            labels.push_back(cluster);
        }
        return labels;
    }
    
    std::vector<std::vector<double>> get_centroids() const {
        return centroids;
    }
};

PYBIND11_MODULE(kmeans, m) {
    py::class_<KMeans>(m, "KMeans")
        .def(py::init<int, int, double>(), 
             py::arg("n_clusters") = 3, 
             py::arg("max_iter") = 100, 
             py::arg("tolerance") = 1e-4)
        .def("fit", &KMeans::fit)
        .def("predict", &KMeans::predict)
        .def("get_centroids", &KMeans::get_centroids);
}