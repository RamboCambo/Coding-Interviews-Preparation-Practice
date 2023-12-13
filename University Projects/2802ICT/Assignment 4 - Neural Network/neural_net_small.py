import time
import numpy as np
import pickle
import matplotlib.pyplot as plt

# Define the sigmoid activation function
def sigmoid(x):
        return 1 / (1 + np.exp(-x))

# Convert labels into one-hot encoded format
def one_hot(y, num_classes):
    return np.eye(num_classes)[y]

class NeuralNetwork:
    # Initialize the neural network with random weights and biases
    def __init__(self, input_size, hidden_size, output_size, random_state=None):
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.output_size = output_size
        self.random_state = random_state

        if random_state:
            np.random.seed(random_state)

        self.weights_hidden = np.random.randn(hidden_size, input_size)
        self.bias_hidden = np.random.randn(hidden_size, 1)
        self.weights_output = np.random.randn(output_size, hidden_size)
        self.bias_output = np.random.randn(output_size, 1)

    # Compute the forward pass through the network
    def forward(self, x):
        hidden_layer_output = np.dot(self.weights_hidden, x.T) + self.bias_hidden
        hidden_layer_output_sigmoid = sigmoid(hidden_layer_output)
        output_layer_output = np.dot(self.weights_output, hidden_layer_output_sigmoid) + self.bias_output
        output_layer_output_sigmoid = sigmoid(output_layer_output)
        return hidden_layer_output_sigmoid, output_layer_output_sigmoid

    # Train the network using batched gradient descent
    def train(self, x, y, learning_rate, epochs, batch_size):
        num_samples = x.shape[0]

        for epoch in range(epochs):
            permutation = np.random.permutation(num_samples)
            shuffled_x = x[permutation]
            shuffled_y = y[permutation]

            for i in range(0, num_samples, batch_size):
                x_batch = shuffled_x[i:i+batch_size, :]
                y_batch = shuffled_y[i:i+batch_size, :]

                hidden_layer_output, output = self.forward(x_batch)

                """
                Calculate the derivative of the cost function with respect to the original input (x) for the sigmoid activation function
                Using the chain rule:
                
                First, we find the derivative of the sigmoid function with respect to its input (dy/dx or sigmoid'(x))
                The sigmoid function is defined as sigmoid(x) = 1 / (1 + exp(-x))
                The derivative of the sigmoid function is given by sigmoid'(x) = sigmoid(x) * (1 - sigmoid(x))
                
                Next, we multiply the derivative of the sigmoid function by the derivative of the cost function with respect to the output of the sigmoid function
                The cost function is typically defined as the quadratic cost function, which can be written as C = 1/2n * (output - y)^2, where output represents the output of the sigmoid function and y represents the target value
                The derivative of the quadratic cost function with respect to the output is (output - y)
                
                Multiplying these two derivatives together gives us the derivative of the cost function with respect to the original input
                This derivative represents the impact of changes in the original input on the overall cost and is denoted as dC/dx
                
                By calculating dC/dx, we can update the weights and biases in the neural network during training, optimizing its performance
                """

                # Calculate the error for backpropagation
                error = output - y_batch.T
                d_output = error * (output * (1 - output)) # derivative w.r.t. output layer
                d_hidden = np.dot(self.weights_output.T, d_output) * (hidden_layer_output * (1 - hidden_layer_output)) # derivative w.r.t. hidden layer

                # Adjust weights and biases using the calculated derivatives
                self.weights_output -= learning_rate * np.dot(d_output, hidden_layer_output.T) / batch_size
                self.bias_output -= learning_rate * np.sum(d_output, axis=1, keepdims=True) / batch_size
                self.weights_hidden -= learning_rate * np.dot(d_hidden, x_batch) / batch_size
                self.bias_hidden -= learning_rate * np.sum(d_hidden, axis=1, keepdims=True) / batch_size

# Define the network parameters
input_size = 2
hidden_size = 2
output_size = 2
random_state = 42
print("Random State:", random_state)
network = NeuralNetwork(input_size, hidden_size, output_size, random_state)

network.weights_hidden = np.array([[0.1, 0.1], [0.2, 0.1]])
network.bias_hidden = np.array([[0.1], [0.1]])
network.weights_output = np.array([[0.1, 0.1], [0.1, 0.2]])
network.bias_output = np.array([[0.1], [0.1]])

batch_size = 2
learning_rate = 0.1
epochs = 1

combined_data = np.array([[0.1, 0.1], [0.1, 0.2]])
combined_labels = np.array([[1.0, 0.0], [0.0, 1.0]])

# Train the neural network
network.train(combined_data, combined_labels, learning_rate, epochs, batch_size)

# Print the weights and biases
print("Weights (Hidden):\n", network.weights_hidden)
print("Bias (Hidden):\n", network.bias_hidden)
print("Weights (Output):\n", network.weights_output)
print("Bias (Output):\n", network.bias_output)