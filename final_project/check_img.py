import matplotlib.pyplot as plt
import matplotlib.patches as patches

# Defining the modules with their dimensions and positions

for index in range(29):
    modules = {}
    module_width = {}
    module_length = {}

    data_in = open("testcase/case.in", "r")
    line_in = data_in.readlines()
    data_out = open("floorplan/partition_{}.out".format(index), "r")
    line_out = data_out.readlines()

    for i in range(len(line_in) - 1):
        list_1 = line_in[i+1].split()
        module_width[list_1[0]] = int(list_1[1])
        module_length[list_1[0]] = int(list_1[2]) 

    for i in range(len(line_out) - 2):
        list_2 = line_out[i+2].split()
        
        if len(list_2) == 4:
            modules[list_2[0]] = {"length": module_width[list_2[0]], "width": module_length[list_2[0]], "x": int(list_2[1]), "y": int(list_2[2])}
        else:
            modules[list_2[0]] = {"length": module_length[list_2[0]], "width": module_width[list_2[0]], "x": int(list_2[1]), "y": int(list_2[2])}
            
    # Setting up the plot
    fig, ax = plt.subplots()
    ax.set_title('Module Positions')
    ax.set_xlabel('X coordinate')
    ax.set_ylabel('Y coordinate')

    # Adding each module as a rectangle to the plot
    for module_id, module in modules.items():
        rect = patches.Rectangle(
            (module["x"], module["y"]), module["width"], module["length"], 
            linewidth=1, edgecolor='black', facecolor='none', label=module_id
        )
        ax.add_patch(rect)
        # Adding the module ID text inside the rectangle
        plt.text(module["x"] + module["width"]/2, module["y"] + module["length"]/2, module_id, 
                ha='center', va='center', color='blue')

    # Adjusting the plot limits and showing the plot
    ax.set_xlim(0, max(module["x"] + module["width"] for module in modules.values()) + 10)
    ax.set_ylim(0, max(module["y"] + module["length"] for module in modules.values()) + 10)
    plt.grid(True)
    plt.savefig("floorplan/partition_{}.jpg".format(index))
