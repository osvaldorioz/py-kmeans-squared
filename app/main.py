from fastapi import FastAPI
from fastapi.responses import FileResponse
from pydantic import BaseModel
import numpy as np
from typing import List
import matplotlib
import matplotlib.pyplot as plt
from kmeans import KMeans
import json
import seaborn as sbn

matplotlib.use('Agg')  # Usar backend no interactivo
app = FastAPI()

# Definir el modelo para el vector
class VectorF(BaseModel):
    vector: List[float]
    

@app.post("/kmeans-squared")
def calculo(seed: int, n_clusters: int, max_iters: int):
    np.random.seed(seed)
    data = np.vstack([
        np.random.normal(loc=[2, 2], scale=1, size=(500, 2)),
        np.random.normal(loc=[8, 8], scale=1, size=(1000, 2)),
        np.random.normal(loc=[2, 8], scale=1, size=(1500, 2))
    ])

    # Crear y entrenar el modelo K-means

    kmeans = KMeans(n_clusters=3, max_iter=1000)
    #kmeans = KMeans(n_cluster=n_clusters, max_iter=max_iters)
    kmeans.fit(data.tolist())

    # Obtener resultados
    labels = kmeans.predict(data.tolist())
    centroids = kmeans.get_centroids()

    # Convertir a numpy array para facilitar el manejo
    centroids = np.array(centroids)

    # Graficar los puntos de datos y los centroides
    plt.figure(figsize=(8, 6))

    # Colores para cada cluster
    colors = ['r', 'g', 'b', 'c', 'm', 'y', 'k']

    # Graficar los puntos de datos, coloreados por cluster
    for i, point in enumerate(data):
        cluster = labels[i]
        plt.scatter(point[0], point[1], c=colors[cluster], label=f'Cluster {cluster}' if i == 0 else "")

    # Graficar los centroides
    plt.scatter(centroids[:, 0], centroids[:, 1], c='black', marker='x', s=200, label='Centroides')

    # Añadir etiquetas y leyenda
    plt.title('Resultados del K-means')
    plt.xlabel('Característica 1')
    plt.ylabel('Característica 2')
    plt.legend()
    plt.grid(True)

    output_file = 'squared_euclidean_distance.png'
    plt.savefig(output_file)
    plt.close()


    j1 = {
        "Grafica": output_file
    }
    jj = json.dumps(str(j1))

    return jj

@app.get("/kmeans-squared-graph")
def getGraph(output_file: str):
    return FileResponse(output_file, media_type="image/png", filename=output_file)