import random

Module_num = 1000
Size_range = 100

upper_bound = 0.6
lower_bound = 1.75

with open('case_{}.in'.format(Module_num), 'w') as file:
    file.write("{} {}\n". format(upper_bound, lower_bound))
    
    for count in range(Module_num):
        file.write("m{} {} {}".format(count, random.randint(10, Size_range), random.randint(10, Size_range)))
        if (count != Module_num - 1):
            file.write("\n")

