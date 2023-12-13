from collections import deque
import time
from io import StringIO

# main function to find the optimal path
def find_path(alg, size, board):
    graph = {}

    # build up the graph
    # i represents the row
    # j represents the column
    for i in range(len(board)):
        for j in range(len(board[i])):
            node_label = (i,j)
            graph[node_label] = []

            # predicates which determine which positions around the current node are available
            # note we only include the G and R types as we don't want to loop back onto the start node
            top = i > 0 and board[i-1][j] in ["R", "G"]
            bottom = i < len(board)-1 and board[i+1][j] in ["R", "G"]
            left = j > 0 and board[i][j-1] in ["R", "G"]
            right = j < len(board[i])-1 and board[i][j+1] in ["R", "G"]

            # prioritise the diagonal pieces in the order as they have a lower cost

            # top left piece
            if top and left and board[i-1][j-1] in ["R", "G"]:
                neighbour_label = (i-1,j-1)
                graph[node_label].append((neighbour_label, 1))
            # top right piece
            if top and right and board[i-1][j+1] in ["R", "G"]:
                neighbour_label = (i-1,j+1)
                graph[node_label].append((neighbour_label, 1))
            # bottom left piece
            if bottom and left and board[i+1][j-1] in ["R", "G"]:
                neighbour_label = (i+1,j-1)
                graph[node_label].append((neighbour_label, 1))
            # bottom right piece
            if bottom and right and board[i+1][j+1] in ["R", "G"]:
                neighbour_label = (i+1,j+1)
                graph[node_label].append((neighbour_label, 1))

            # testing if the adjacent nodes can be added to the adjacency list
            if top:
                neighbour_label = (i-1,j)
                graph[node_label].append((neighbour_label, 2))
            if bottom:
                neighbour_label = (i+1,j)
                graph[node_label].append((neighbour_label, 2))
            if left:
                neighbour_label = (i,j-1)
                graph[node_label].append((neighbour_label, 2))
            if right:
                neighbour_label = (i,j+1)
                graph[node_label].append((neighbour_label, 2))

            # by combining the two predicates which would both have to be true for each diagonal piece
            # we can then infer if the diagonal piece can be added

    # setting the start and end node based on the test input
    # it does assume any board will have a start piece at 0,0 and an end piece at board len,board len
    start_node = (0,0)
    end_node = (len(board)-1,len(board[0])-1)

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
        if current_node == (0,0):
            break
        current_node = parent[current_node]

    # get the length of the path, not the cost
    length = len(shortest_path)-1

    # reverse the path as we want to know from start to finish, not finish to start
    shortest_path.reverse()

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

    # copy the board array
    modified_board = board[:]
    # loop through the shortest_path nodes, except the start and finish as we don't want to replace them
    for node in shortest_path[1:-1]:
        # get the current board row
        row = modified_board[node[0]]
        # string manipulation to replace the position the node with an o
        modified_board[node[0]] = row[:node[1]] + "o" + row[node[1]+1:]

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
        output.write(f"no path\n")

    if type(result) == tuple:
        direction_steps, cost, length, explored, board = result

        output.write(f"{direction_steps}\n\n")
        output.write(f"Path cost: {cost}\n")
        output.write(f"Path length: {length}\n")
        output.write(f"Explored states: {explored}\n\n")

        # print out the board
        for line in board:
            output.write(f"{line}\n")

        output.write("\nExtra Stats:\n")
        output.write(f"Completion Time: {end_time-start_time:.10f}s\n")

    # write to the output file
    with open("output.txt", "w+") as f:
        f.write(output.getvalue())
