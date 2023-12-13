from collections import deque
import time
from io import StringIO
from queue import PriorityQueue
import random


def generate_graph(size, board):
    graph = {}

    # build up the graph
    # i represents the row
    # j represents the column
    for i in range(size):
        for j in range(size):
            node_label = (i, j)
            graph[node_label] = []

            # predicates which determine which positions around the current node are available
            # note we only include the G and R types as we don't want to loop back onto the start node
            top = i > 0 and board[i-1][j] in ["R", "G"]
            bottom = i < size-1 and board[i+1][j] in ["R", "G"]
            left = j > 0 and board[i][j-1] in ["R", "G"]
            right = j < size-1 and board[i][j+1] in ["R", "G"]

            # prioritise the diagonal pieces in the order as they have a lower cost

            # top left piece
            if top and left and board[i-1][j-1] in ["R", "G"]:
                neighbour_label = (i-1, j-1)
                graph[node_label].append((neighbour_label, 1))
            # top right piece
            if top and right and board[i-1][j+1] in ["R", "G"]:
                neighbour_label = (i-1, j+1)
                graph[node_label].append((neighbour_label, 1))
            # bottom left piece
            if bottom and left and board[i+1][j-1] in ["R", "G"]:
                neighbour_label = (i+1, j-1)
                graph[node_label].append((neighbour_label, 1))
            # bottom right piece
            if bottom and right and board[i+1][j+1] in ["R", "G"]:
                neighbour_label = (i+1, j+1)
                graph[node_label].append((neighbour_label, 1))

            # testing if the adjacent nodes can be added to the adjacency list
            if top:
                neighbour_label = (i-1, j)
                graph[node_label].append((neighbour_label, 2))
            if bottom:
                neighbour_label = (i+1, j)
                graph[node_label].append((neighbour_label, 2))
            if left:
                neighbour_label = (i, j-1)
                graph[node_label].append((neighbour_label, 2))
            if right:
                neighbour_label = (i, j+1)
                graph[node_label].append((neighbour_label, 2))

            # by combining the two predicates which would both have to be true for each diagonal piece
            # we can then infer if the diagonal piece can be added

    return graph


def generate_direction_steps(shortest_path):
    # now converting the shortest path into directions we can follow from the start
    steps = []
    # loop through pairs of elements
    # we do this as for each node, we want to know the next node, but then go
    # to the next node and know it's next node, and so on
    # so zip is just an easy way to do this
    for current_node, next_node in zip(shortest_path, shortest_path[1:]):
        # dictionary of differences (directions) to the current node from the next node
        directions = {
            (-1, -1): "UL",
            (-1, 0): "U",
            (-1, 1): "UR",
            (0, -1): "L",
            (0, 1): "R",
            (1, -1): "DL",
            (1, 0): "D",
            (1, 1): "DR"
        }

        # creating the difference in nodes by just subtracting the positions of the current node
        # from the next node
        diff = (next_node[0] - current_node[0], next_node[1] - current_node[1])

        # adds to the steps we need to take by adding the value of the difference from
        # directions dictionary
        steps.append(directions[diff])

    direction_steps = "-".join(steps)

    return direction_steps


def generate_modified_board(shortest_path, board):
    # copy the board array
    modified_board = board[:]
    # loop through the shortest_path nodes, except the start and finish as we don't want to replace them
    for node in shortest_path[1:-1]:
        # get the current board row
        row = modified_board[node[0]]
        # string manipulation to replace the position the node with an o
        modified_board[node[0]] = row[:node[1]] + "o" + row[node[1]+1:]

    return modified_board


def find_path_bfs(size, graph, end_node):
    # setting the start and end node based on the test input
    # it does assume any board will have a start piece at 0,0 and an end piece at board len,board len
    start_node = (0, 0)

    # setting up dictionary with the distances to each node
    distance = {start_node: 0}
    # setting up dictionary which provides way to track which node is the parent of which
    parent = {start_node: None}

    # BFS queue so we explore layer by layer of the tree
    queue = deque([start_node])
    # tentatively allowing the start and finish nodes to count as explored nodes
    explored = 0

    while queue:
        # get the first in element and then remove it
        current_node = queue.popleft()

        # if we've reached the goal position
        if current_node == end_node:
            break

        # gets all the children and their distances from the current node
        for neighbour, weight in graph[current_node]:
            # if it isn't in the distance graph, we haven't visited it yet
            if neighbour not in distance:
                explored += 1
                # add entry into distance to show how far the piece is from the start position
                distance[neighbour] = distance[current_node] + weight
                # create link in parent dictionary so we know which piece led to the current one
                parent[neighbour] = current_node
                # add neighbour to be processed later
                queue.append(neighbour)

    # check if the end goal can be reached
    if end_node not in distance:
        return "no path"

    # get the final cost to get to the goal node
    cost = distance[end_node]

    # code block which finds the shortest path in terms of node positions
    # it works by working backwards from the end node, and then finds the parent
    # of each node and then goes to that node and repeats until it finds the start node
    shortest_path = []
    current_node = end_node
    while current_node is not None:
        shortest_path.append(current_node)
        if current_node == start_node:
            break
        current_node = parent[current_node]

    # get the length of the path, not the cost
    length = len(shortest_path)-1

    # reverse the path as we want to know from start to finish, not finish to start
    shortest_path.reverse()

    return cost, length, explored, shortest_path


def find_path_ucs(size, graph, end_node):
    # setting the start and end node based on the test input
    # it does assume any board will have a start piece at 0,0 and an end piece at board len,board len
    start_node = (0, 0)

    # setting up dictionary with the distances to each node
    distance = {start_node: 0}
    # setting up dictionary which provides way to track which node is the parent of which
    parent = {start_node: None}

    queue = PriorityQueue()
    queue.put((0, start_node))

    explored = 0

    while queue:
        # get the first in element and then remove it
        current_cost, current_node = queue.get()

        # if we've reached the goal position
        if current_node == end_node:
            break

        # gets all the children and their distances from the current node
        for neighbour, weight in graph[current_node]:
            # if it isn't in the distance graph, we haven't visited it yet
            if neighbour not in distance:
                explored += 1
                # add entry into distance to show how far the piece is from the start position
                distance[neighbour] = distance[current_node] + weight
                # create link in parent dictionary so we know which piece led to the current one
                parent[neighbour] = current_node
                # add neighbour to be processed later
                queue.put((current_cost + weight, neighbour))
            # check if the distance is smaller and if so, update the neighbour's distance and make its parent the current node
            # elif distance[neighbour] > distance[current_node] + weight:
            #     distance[neighbour] = distance[current_node] + weight
            #     parent[neighbour] = current_node
            #     queue.put((current_cost + weight, neighbour))

    # check if the end goal can be reached
    if end_node not in distance:
        return "no path"

    # get the final cost to get to the goal node
    cost = distance[end_node]

    # code block which finds the shortest path in terms of node positions
    # it works by working backwards from the end node, and then finds the parent
    # of each node and then goes to that node and repeats until it finds the start node
    shortest_path = []
    current_node = end_node
    while current_node is not None:
        shortest_path.append(current_node)
        if current_node == start_node:
            break
        current_node = parent[current_node]

    # get the length of the path, not the cost
    length = len(shortest_path)-1

    # reverse the path as we want to know from start to finish, not finish to start
    shortest_path.reverse()

    return cost, length, explored, shortest_path


def find_path_dfs(size, graph, end_node):
    stack = deque([((0, 0), [(0, 0)], 0)])
    visited = set()

    while stack:
        node, path, cost = stack.popleft()

        for neighbour, weight in graph[node]:
            if neighbour not in visited:
                if neighbour == end_node:
                    return cost + weight, len(path), len(visited), path + [neighbour]
                else:
                    stack.appendleft(
                        (neighbour, path + [neighbour], cost + weight))
                    visited.add(neighbour)

    return "no path"

# define heuristic function which is the manhattan distance between the node and the goal


# heuristic - how to find
# let's remove one of the constraints
# admissable - never overstimates the board cost
def manhattan_distance(node, goal):
    return abs(node[0] - goal[0]) + abs(node[1] - goal[1])


def find_path_a_star(size, graph, end_node):
    # setting the start and end node based on the test input
    # it does assume any board will have a start piece at 0,0 and an end piece at board len,board len
    start_node = (0, 0)

    # setting up dictionary with the distances to each node
    distance = {start_node: 0}
    # setting up dictionary which provides way to track which node is the parent of which
    parent = {start_node: None}

    # priority queue which will be used to find the next node to process
    queue = PriorityQueue()
    queue.put((0, start_node))
    # tentatively allowing the start and finish nodes to count as explored nodes
    explored = 0

    while queue:
        # get the first in element and then remove it
        current_node = queue.get()[1]

        # if we've reached the goal position
        if current_node == end_node:
            break

        # gets all the children and their distances from the current node
        for neighbour, weight in graph[current_node]:
            # if it isn't in the distance graph, we haven't visited it yet
            if neighbour not in distance:
                explored += 1
                # add entry into distance to show how far the piece is from the start position
                distance[neighbour] = distance[current_node] + weight
                # create link in parent dictionary so we know which piece led to the current one
                parent[neighbour] = current_node
                # add neighbour to be processed later
                queue.put(
                    (distance[neighbour] + manhattan_distance(neighbour, end_node), neighbour))
            # check if the distance is smaller and if so, update the neighbour's distance and make its parent the current node
            elif distance[neighbour] > distance[current_node] + weight:
                distance[neighbour] = distance[current_node] + weight
                parent[neighbour] = current_node
                queue.put(
                    (distance[neighbour] + manhattan_distance(neighbour, end_node), neighbour))

    # check if the end goal can be reached
    if end_node not in distance:
        return "no path"

    # get the final cost to get to the goal node
    cost = distance[end_node]

    # code block which finds the shortest path in terms of node positions
    # it works by working backwards from the end node, and then finds the parent

    shortest_path = []
    current_node = end_node
    while current_node is not None:
        shortest_path.append(current_node)
        if current_node == start_node:
            break
        current_node = parent[current_node]

    # get the length of the path, not the cost
    length = len(shortest_path)-1

    # reverse the path as we want to know from start to finish, not finish to start
    shortest_path.reverse()

    return cost, length, explored, shortest_path

# generate a random path


def generate_initial_solution(size, graph, length):
    # generate a random path using the directions L, R, U, D, UL, UR, DL, DR and then convert it to a path
    # the path is generated by randomly choosing a direction and it doesn't matter if it's valid or not
    # as we will check if it's valid later
    random_path = [random.choice(
        ["L", "R", "U", "D", "UL", "UR", "DL", "DR"]) for _ in range(length)]

    # return the random path
    return random_path


def path_penalty(size, board, steps, end_node):
    penalty = 0

    # penalise for the number of steps
    penalty += len(steps)

    path = convert_steps_to_path(steps)

    # penalise if the final node isn't the goal
    if not path_final_node_is_goal(size, path):
        penalty += 1000

    # penalise if the path is invalid
    # invalid paths are ones which go outside the board or go through blocked squares
    # outside the board is penalised twice as much as going through a blocked square
    path_is_valid, invalid_multiplier = check_path_is_valid(size, board, path)
    if not path_is_valid:
        penalty += 1000 * invalid_multiplier

    # reward paths which end close to the goal
    penalty -= path_end_distance_to_goal(size, path, end_node)

    return penalty

# define path_end_distance_to_goal function
# it returns the manhattan distance between the end of the path and the goal


def path_end_distance_to_goal(size, path, end_node):
    return abs(path[-1][0] - end_node[0]) + abs(path[-1][1] - end_node[1])

# function to check if the path is valid
# it checks if the path is within the board and if it doesn't go through any blocked squares
# or go outside the board


def check_path_is_valid(size, board, path):
    multiplier = 0
    is_valid = True
    for node in path:
        # penalise outside the board twice as much as going through a blocked square
        if node[0] < 0 or node[0] >= size or node[1] < 0 or node[1] >= size:
            multiplier += 2
            is_valid = False
        # shouldn't error as we've already checked if the node is within the board
        elif board[node[0]][node[1]] == "X":
            multiplier += 1
            is_valid = False
    return is_valid, multiplier

# function to convert steps to a path
# the first element in the path is always (0,0)
# the first column of the tuple represents the row and the second column represents the column


def convert_steps_to_path(steps):
    path = [(0, 0)]

    for step in steps:
        if step == "L":
            path.append((path[-1][0], path[-1][1]-1))
        elif step == "R":
            path.append((path[-1][0], path[-1][1]+1))
        elif step == "U":
            path.append((path[-1][0]-1, path[-1][1]))
        elif step == "D":
            path.append((path[-1][0]+1, path[-1][1]))
        elif step == "UL":
            path.append((path[-1][0]-1, path[-1][1]-1))
        elif step == "UR":
            path.append((path[-1][0]-1, path[-1][1]+1))
        elif step == "DL":
            path.append((path[-1][0]+1, path[-1][1]-1))
        elif step == "DR":
            path.append((path[-1][0]+1, path[-1][1]+1))

    return path

# function to check if the path reaches the goal node


def path_final_node_is_goal(size, path):
    # if the last node in the path is the goal node, then it reaches the goal node
    return path[-1] == (size-1, size-1)

# define step_cost function
# uses the directions L, R, U, D, UL, UR, DL, DR
# if a direction is L, R, U, D then the cost is 2
# if a direction is UL, UR, DL, DR then the cost is 1


def step_cost(steps):
    cost = 0
    for step in steps:
        if step in ["L", "R", "U", "D"]:
            cost += 2
        elif step in ["UL", "UR", "DL", "DR"]:
            cost += 1
    return cost


counter = 0
counter2 = 0


def find_path_local_search(size, board, initial_steps, end_node, max_iterations=1000):
    current_steps = initial_steps[:]
    current_penalty = path_penalty(size, board, initial_steps, end_node)

    for _ in range(max_iterations):
        # create a copy of the current path
        new_steps = current_steps[:]

        # randomly choose a step to change
        step_to_change = random.randint(0, len(new_steps)-1)

        add_probability = 0.5
        remove_probability = 0.1

        global counter, counter2

        # randomly choose whether to add or remove a step
        if random.random() < add_probability:
            counter += 1
            random_step = random.choice(
                ["L", "R", "U", "D", "UL", "UR", "DL", "DR"])
            # randomly choose a step to add
            new_steps.insert(step_to_change, random_step)
            # print(step_to_change)
            # print(random_step)
            # print(new_steps)
        elif random.random() < remove_probability:
            # remove the step
            del new_steps[step_to_change]
        else:
            # randomly choose a step to change
            new_steps[step_to_change] = random.choice(
                ["L", "R", "U", "D", "UL", "UR", "DL", "DR"])

        # evaluate both the current steps and the new steps
        # if the new steps are better, then replace the current steps with the new steps
        # if the new steps are worse, then keep the current steps
        new_penalty = path_penalty(size, board, new_steps, end_node)

        # some probablity
        # 1/100 chance it would allow worse path to be chosen
        # mess with the probabilities

        if new_penalty < current_penalty:
            counter2 += 1
            print(current_penalty, new_penalty)
            print(current_steps, new_steps)
            current_steps = new_steps
            current_penalty = new_penalty

    path = convert_steps_to_path(current_steps)
    cost = step_cost(current_steps)

    print(counter2)

    return cost, len(current_steps)-1, max_iterations, path, current_steps


def local_search_modified_board(board, path):
    # copy the board array
    modified_board = board[:]
    # loop through the nodes
    for row, col in path:
        replacement_char = None
        # get the current board row
        if board[row][col] == "X":
            replacement_char = "-"
        elif board[row][col] == "R":
            replacement_char = "o"
        elif board[row][col] == ".":
            replacement_char = "!"

        if replacement_char is not None:
            modified_board[row] = modified_board[row][:col] + \
                replacement_char + modified_board[row][col+1:]

    return modified_board


def find_end_node(size, board):
    for row in range(size):
        for col in range(size):
            if board[row][col] == "G":
                return (row, col)

# main function to find the optimal path


def find_path(alg, size, board):
    # create the graph of the board
    graph = generate_graph(size, board)
    end_node = find_end_node(size, board)

    # BFS
    if alg == 1:
        result = find_path_bfs(size, graph, end_node)
    # UCS
    elif alg == 2:
        result = find_path_ucs(size, graph, end_node)
    # IDS / DFS
    elif alg == 3:
        result = find_path_dfs(size, graph, end_node)
    # A*
    elif alg == 4:
        result = find_path_a_star(size, graph, end_node)
    # Local Search
    elif alg == 5:
        initial_steps = generate_initial_solution(size, graph, 10)
        cost, length, explored, path, steps = find_path_local_search(
            size, board, initial_steps, end_node, 1000)

        # if the path contains nodes outside the board, the find out the total size of the board including the nodes outside the board
        # and then generate a new board with the size of the board including the nodes outside the board
        # then generate the modified board with the path marked
        # finally generate the direction steps after everything is done being calculated

        row_vals = [node[0] for node in path]
        col_vals = [node[1] for node in path]

        # get the largest value of the row and column
        # use size or 0 is they're still bigger/smaller than the result paths' values
        min_row = min(0, min(row_vals))
        max_row = max(size, max(row_vals))
        min_col = min(0, min(col_vals))
        max_col = max(size, max(col_vals))

        board_row_len = max_row - min_row
        board_col_len = max_col - min_col

        row_offset = 0
        col_offset = 0

        if min_row < 0:
            row_offset = abs(min_row)
        if min_col < 0:
            col_offset = abs(min_col)

        new_board = ["." * board_col_len for _ in range(board_row_len)]

        # loop through original board and copy the values into the new board
        for row in range(size):
            new_board[row+row_offset] = new_board[row+row_offset][:col_offset] + \
                board[row] + new_board[row+row_offset][size+col_offset:]

        # offset the path so that it matches the new board
        new_path = [(node[0]+row_offset, node[1]+col_offset) for node in path]

        modified_board = local_search_modified_board(new_board, new_path)

        # make the steps pretty by adding a - in between them
        direction_steps = "-".join(steps)

        return direction_steps, cost, length, explored, modified_board
    else:
        return "invalid algorithm"

    if type(result) == str:
        return "no path"

    cost, length, explored, path = result

    # finally generate the modified board and direction steps after everything is done being calculated
    # convert the shortest path into directions we can follow
    direction_steps = generate_direction_steps(path)
    # generate the modified board with the path marked
    modified_board = generate_modified_board(path, board)

    return direction_steps, cost, length, explored, modified_board


# main function which the program loads into
if __name__ == '__main__':
    # default properties
    algorithm = 1
    size = 0
    board = []
    # open the input file and read properties into memory
    with open("input.txt", "r") as f:
        for index, value in enumerate(f):
            if index == 0:
                algorithm = int(value)
                continue
            if index == 1:
                size = int(value)
                continue

            # build up the board line by line
            board.append(value.strip())

    # find the path for the board
    # also record time either side of function to measure time taken to find the path
    start_time = time.time()
    result = find_path(algorithm, size, board)
    end_time = time.time()

    # assume there will be no output
    output = StringIO()

    if type(result) == str:
        output.write(result)

    elif type(result) == tuple:
        direction_steps, cost, length, explored, board = result

        output.write(f"{direction_steps}\n\n")
        output.write(f"Path cost: {cost}\n")
        output.write(f"Path length: {length}\n")
        output.write(f"Explored states: {explored}\n\n")

        # print out the board
        for line in board:
            output.write(f"{line}\n")

        output.write("\nExtra Stats:\n")
        output.write(f"Completion Time: {end_time-start_time:.10f}s\n\n")

        output.write("Local search only!\n")
        output.write(". == invalid position outside original board\n")
        output.write("! == step in solution outside original board\n")
        output.write("- == step in solution that is on cliff\n")

    # write to the output file
    with open("output.txt", "w+") as f:
        f.write(output.getvalue())
