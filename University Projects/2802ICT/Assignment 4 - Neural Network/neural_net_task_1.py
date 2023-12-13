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
        accuracies = []
        epochs_list = []

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

            # Compute the accuracy after each epoch
            _, predictions = self.forward(x_test)
            accuracy = np.mean(np.argmax(predictions, axis=0) == y_test)
            accuracies.append(accuracy)
            epochs_list.append(epoch)

        # Plot the test accuracy vs epoch graph
        plt.plot(epochs_list, accuracies)
        plt.xlabel('Epoch')
        plt.ylabel('Test Accuracy')
        plt.title('Test Accuracy vs Epoch')
        plt.show()

        # Print the maximum accuracy achieved
        max_accuracy = np.max(accuracies)
        print(f"Maximum Accuracy: {max_accuracy*100}%")

# Define the network parameters
input_size = 3072
hidden_size = 30
output_size = 10
random_state = 42
print("Random State:", random_state)
network = NeuralNetwork(input_size, hidden_size, output_size, random_state)
batch_size = 100
learning_rate = 1
epochs = 20

# The data is divided into different batches for training
batch_files = ['data_batch_1', 'data_batch_2', 'data_batch_3', 'data_batch_4', 'data_batch_5']

combined_data = []
combined_labels = []

# Load and combine training data batches
for batch_file in batch_files:
    with open(batch_file, 'rb') as file:
        batch_data = pickle.load(file, encoding='bytes')

    x_batch = batch_data[b'data']
    y_batch = np.array(batch_data[b'labels'])

    combined_data.append(x_batch)
    combined_labels.append(y_batch)

# Normalize and prepare the combined data
combined_data = np.concatenate(combined_data, axis=0)
combined_data = combined_data / 255
combined_labels = np.concatenate(combined_labels, axis=0)
combined_labels = one_hot(combined_labels, output_size)

# Load and normalize test data
with open("test_batch", 'rb') as file:
    test_data = pickle.load(file, encoding='bytes')

x_test = test_data[b'data']
x_test = x_test / 255
y_test = np.array(test_data[b'labels'])

# Train the neural network
network.train(combined_data, combined_labels, learning_rate, epochs, batch_size)
