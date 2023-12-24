import random

constraint_rate = 0.5
Connect_num = 500
Module_num = 300
connect_size_1 = 2
connect_size_2 = 20

# Function to generate a list of random connections for a block
def generate_connections(block_id, max_connections, total_blocks):
    connections = set()
    while len(connections) < max_connections:
        connection = random.randint(0, total_blocks - 1)
        if connection != block_id:
            connections.add(connection)
    return connections

with open('connect.in', 'w') as file:
    file.write("{}\n".format(constraint_rate))
    
    for count in range(Connect_num):
        # Random number of connections (e.g., up to 5)
        num_connections = random.randint(connect_size_1, connect_size_2)
        connections = generate_connections(count, num_connections, Module_num)

        # Format the connections as block names
        connections_str = ' '.join('m{}'.format(c) for c in connections)

        file.write("NET n{} {} ;\n".format(count, connections_str))
