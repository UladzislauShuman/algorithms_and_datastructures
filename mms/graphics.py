import matplotlib.pyplot as plt
import numpy as np
import os
import time

def plot_data_from_file(filename):
    x_values = []
    y_values = []
    
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split()
                
            if len(parts) == 2:
                x = float(parts[0])
                y = float(parts[1])
                x_values.append(x)
                y_values.append(y)
    
    plt.figure(figsize=(10, 6))
    plt.plot(x_values, y_values, marker='o')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.grid(True)
    plt.show()

# Компиляция и запуск Java-программы
os.system("javac IonSimulation.java")  # Компиляция
os.system("java IonSimulation")       # Запуск

plot_data_from_file('out.txt')