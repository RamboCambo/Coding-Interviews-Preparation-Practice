# Import necessary libraries
import math
from collections import Counter
import pandas as pd
import random
import numpy as np
import matplotlib.pyplot as plt

# Define Decision Tree Node class
class DecisionTreeNode:
    def __init__(self, attribute=None, value=None, class_value=None):
        self.attribute = attribute
        self.value = value
        self.class_value = class_value
        self.children = {}

    def add_child(self, value, child):
        self.children[value] = child

# Calculate entropy of a set of labels
def calculate_entropy(labels):
    # Initialize entropy to 0
    entropy = 0
    
    # Get the total number of instances
    total_instances = len(labels)
    
    # Count the number of occurrences of each label
    label_counts = Counter(labels)

    # Loop through each label count
    for count in label_counts.values():
        # Calculate the probability of the label
        probability = count / total_instances
        
        # Calculate the contribution of the label to the entropy
        entropy -= probability * math.log2(probability)

    # Return the entropy
    return entropy

# Calculate entropy of an attribute
def calculate_attribute_entropy(data, attribute):
    # Get the set of unique values for the attribute
    attribute_values = set(data[attribute])
    
    # Initialize the attribute entropy to 0
    attribute_entropy = 0

    for value in attribute_values:
        # Subset the data to only include instances with the current attribute value
        subset_labels = data[data[attribute] == value]["party"]

        # Calculate the probability of the current attribute value
        probability = len(subset_labels) / len(data)
        
        # Calculate the entropy of the subset of labels for the current attribute value
        attribute_entropy += probability * calculate_entropy(subset_labels)

    # Return the attribute entropy
    return attribute_entropy

# Get majority class label
def majority_class(labels):
    label_counts = Counter(labels)
    return label_counts.most_common(1)[0][0]

# Generate decision tree
def generate_decision_tree(data, attributes):
    class_labels = data["party"]

    # Base case: If all instances have the same class label, create a leaf node
    if len(set(class_labels)) == 1:
        return DecisionTreeNode(class_value=class_labels.iloc[0])

    # Base case: If no attributes remaining, create a leaf node with the majority class label
    if len(attributes) == 0:
        majority_class_value = majority_class(class_labels)
        return DecisionTreeNode(class_value=majority_class_value)

    # Calculate the entropy for each attribute
    attribute_entropies = {}
    for attribute in attributes:
        attribute_entropy = calculate_attribute_entropy(data, attribute)
        attribute_entropies[attribute] = attribute_entropy

    # Find the attribute with the lowest entropy
    splitting_attribute = min(attribute_entropies, key=attribute_entropies.get)

    # Create a new decision tree node with the splitting attribute
    decision_tree = DecisionTreeNode(attribute=splitting_attribute)

    attribute_values = set(data[splitting_attribute])
    new_attributes = attributes.copy()
    new_attributes.remove(splitting_attribute)

    # Loop through each attribute value in the list of attribute values
    for value in attribute_values:
        # Subset the data to only include instances with the current attribute value
        subset_data = data[data[splitting_attribute] == value]
        
        # If there are no instances with the current attribute value, create a leaf node with the majority class label
        if len(subset_data) == 0:
            majority_class_value = majority_class(class_labels)
            decision_tree.add_child(value, DecisionTreeNode(class_value=majority_class_value))
        else:
            # Recursively generate a decision tree for the subset data and remaining attributes
            child_tree = generate_decision_tree(subset_data, new_attributes)
            decision_tree.add_child(value, child_tree)

    # Return the decision tree
    return decision_tree

# Split data into training and testing sets
def split_data(data, test_size):
    total_instances = len(data)
    test_instances = int(total_instances * test_size)
    train_instances = total_instances - test_instances

    data = data.sample(frac=1, random_state=42)  # Shuffle the data randomly

    train_data = data[:train_instances].reset_index(drop=True)
    test_data = data[train_instances:].reset_index(drop=True)

    return train_data, test_data

# Classify an instance using the decision tree
def classify_instance(instance, tree):
    if tree.class_value:
        # Leaf node, return the class value
        return tree.class_value

    attribute_value = instance[tree.attribute]
    if attribute_value in tree.children:
        # Recursively classify the instance
        child = tree.children[attribute_value]
        return classify_instance(instance, child)
    else:
        # No matching attribute value, return the majority class value
        return majority_class(train_data["party"])

# Calculate accuracy of predicted labels
def accuracy(true_labels, predicted_labels):
    correct = sum(1 for true, predicted in zip(true_labels, predicted_labels) if true == predicted)
    return correct / len(true_labels)

# Calculate confusion matrix
def confusion_matrix(true_labels, predicted_labels, classes):
    # Get the number of classes
    num_classes = len(classes)
    
    # Create a matrix to store the number of true positives, false positives, false negatives, and true negatives for each class
    matrix = np.zeros((num_classes, num_classes), dtype=int)

    # Loop through each true and predicted label pair
    for true, predicted in zip(true_labels, predicted_labels):
        # Get the index of the true and predicted labels in the list of classes
        true_index = classes.index(true)
        predicted_index = classes.index(predicted)
        
        # Increment the count in the matrix for the true and predicted label pair
        matrix[true_index][predicted_index] += 1

    # Return the confusion matrix
    return matrix

# Calculate precision, recall, and F1-score
def precision_recall_f1(matrix, classes):
    # Get the number of classes
    num_classes = len(classes)
    
    # Create arrays to store precision, recall, and F1-score for each class
    precision = np.zeros(num_classes)
    recall = np.zeros(num_classes)
    f1_score = np.zeros(num_classes)

    # Loop through each class
    for i in range(num_classes):
        # Calculate the number of true positives, false positives, and false negatives for the current class
        true_positive = matrix[i][i]
        false_positive = sum(matrix[:, i]) - true_positive
        false_negative = sum(matrix[i, :]) - true_positive

        # Calculate precision, recall, and F1-score for the current class
        """
        Precision evaluates how many selected items are relevant. 
        It's the ratio of true positives (correctly identified positive cases) to the sum of 
        true positives and false positives (negative cases incorrectly identified as positive). 
        For example, if an email spam detector flags 100 emails as spam, and 90 of them are actually spam, the precision is 90%.

        Recall assesses how many relevant items are selected. 
        It's the ratio of true positives to the sum of true positives and false negatives (positive cases incorrectly identified as negative). 
        For instance, if out of 100 actual spam emails, the spam detector identifies 80, the recall is 80%.

        F1-score is the harmonic mean of precision and recall, balancing the two metrics. 
        An F1-score reaches its best value at 1 (perfect precision and recall) and worst at 0. 
        For the spam detector, if the precision is 90% and the recall is 80%, the F1-score is approximately 85%.
        """
        precision[i] = true_positive / (true_positive + false_positive)
        recall[i] = true_positive / (true_positive + false_negative)
        f1_score[i] = 2 * precision[i] * recall[i] / (precision[i] + recall[i])

    # Return the precision, recall, and F1-score arrays
    return precision, recall, f1_score

# Calculate accuracy of decision tree on test data
def calculate_accuracy(y_true, y_pred):
    correct_predictions = sum(y_t == y_p for y_t, y_p in zip(y_true, y_pred))
    total_instances = len(y_true)
    accuracy = correct_predictions / total_instances
    return accuracy

# Plot learning curve
def plot_learning_curve(train_data, test_data, attributes):
    train_sizes = np.arange(0.1, 1.1, 0.1)
    accuracies = []

    for train_size in train_sizes:
        train_subset = train_data.sample(frac=train_size, random_state=42)
        decision_tree = generate_decision_tree(train_subset, attributes)
        predictions = test_data.apply(
            classify_instance, args=(decision_tree,), axis=1)
        accuracy = calculate_accuracy(test_data["party"], predictions)
        accuracies.append(accuracy)

    plt.plot(train_sizes, accuracies, marker="o")
    plt.xlabel("Percentage of Training Examples Used to Generate Decision Tree")
    plt.ylabel("Accuracy")
    plt.title("Learning Curve")
    plt.show()

# Import data from CSV using Pandas
data = pd.read_csv("votes.csv")  # Modify the filename to your CSV file

# Get the attribute names from the CSV file (excluding the "party" attribute)
attributes = data.columns.tolist()
attributes.remove("party")

# Split the data into training and testing sets
train_data, test_data = split_data(data, test_size=0.3)

# Generate the decision tree using the training data
decision_tree = generate_decision_tree(train_data, attributes)

# Classify instances from the test set
predicted_labels = [classify_instance(
    instance, decision_tree) for _, instance in test_data.iterrows()]
true_labels = test_data["party"]

# 1. Print the size of the training and testing sets
print("Size of the training set:", len(train_data))
print("Size of the testing set:", len(test_data))
print()

# 2. Calculate and print the total accuracy
total_acc = accuracy(true_labels, predicted_labels)
print("Total Accuracy:", total_acc)
print()

# 3. Calculate and print the confusion matrix
class_list = sorted(set(true_labels))
confusion_mat = confusion_matrix(true_labels, predicted_labels, class_list)
print("Confusion Matrix:")
print(confusion_mat)
print()

# 4. Calculate and print precision, recall, and F1-score values
precision, recall, f1_score = precision_recall_f1(confusion_mat, class_list)
print("Class\tPrecision\tRecall\t\tF1-Score")
for i, class_name in enumerate(class_list):
    print(f"{class_name}\t{precision[i]}\t\t{recall[i]}\t\t{f1_score[i]}")

# Calculate macro and weighted averages for precision, recall, and F1-score
macro_avg_precision = np.mean(precision)  # Calculate the macro average precision
macro_avg_recall = np.mean(recall)  # Calculate the macro average recall
macro_avg_f1_score = np.mean(f1_score)  # Calculate the macro average F1-score

# Calculate the weighted average precision, recall, and F1-score
# The weights are calculated using a list comprehension that sums the number of instances in each class
weighted_avg_precision = np.average(precision, weights=[sum(confusion_mat[i, :]) for i in range(len(class_list))])
weighted_avg_recall = np.average(recall, weights=[sum(confusion_mat[i, :]) for i in range(len(class_list))])
weighted_avg_f1_score = np.average(f1_score, weights=[sum(confusion_mat[i, :]) for i in range(len(class_list))])

print("Macro-Average Precision:", macro_avg_precision)
print("Macro-Average Recall:", macro_avg_recall)
print("Macro-Average F1-Score:", macro_avg_f1_score)
print("Weighted-Average Precision:", weighted_avg_precision)
print("Weighted-Average Recall:", weighted_avg_recall)
print("Weighted-Average F1-Score:", weighted_avg_f1_score)
print()

# 5. Plot the Learning Curve
plot_learning_curve(train_data, test_data, attributes)
