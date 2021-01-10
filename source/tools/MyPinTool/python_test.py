filename1 = "profiler_rl_part1.log"
filename2 = "profiler_rl_part2.log"
filename3 = "trace_v2"

class Instruction:
    def __init__(self, thread_id, instruction_offset, accessed_offset, is_read, is_write, disassemble):
        self.thread_id = thread_id
        self.instruction_offset = instruction_offset
        self.accessed_offset = accessed_offset
        self.is_read = is_read
        self.is_write = is_write
        self.disassemble = disassemble

class Instruction_Part1:
    def __init__(self, ins_offset, thread_id, accessed_offset):
        self.ins_offset = ins_offset
        self.thread_id = thread_id
        self.accessed_offset = accessed_offset

class Instruction_Part2:
    def __init__(self, ins_offset, is_memory_read, is_memory_write, is_stack_read, is_stack_write, disassemble):
        self.ins_offset = ins_offset
        self.is_memory_read = is_memory_read
        self.is_memory_write = is_memory_write
        self.is_stack_read = is_stack_read
        self.is_stack_write = is_stack_write
        self.disassemble = disassemble

instructions = []
instructions_part1 = []
instructions_part2 = []

with open(filename1, "r") as file1, open(filename2, "r") as file2:
    lines_file1 = file1.readlines()
    lines_file2 = file2.readlines()
    for line in lines_file1:
        temp1, temp2, temp3 = line.split()
        instruction_part1 = Instruction_Part1(temp1[:-1], temp2[:-1], temp3)
        instructions_part1.append(instruction_part1)
    for line in lines_file2:
        temp1, temp2, temp3, temp4, temp5, temp6 = line.split("; ")
        instruction_part2 = Instruction_Part2(temp1, temp2, temp3, temp4, temp5, temp6.strip())
        instructions_part2.append(instruction_part2)

for i in instructions_part1:
    ins_offset = i.ins_offset
    thread_id = i.thread_id
    accessed_offset = i.accessed_offset
    for j in instructions_part2:
        if j.ins_offset == ins_offset:
            is_memory_read = j.is_memory_read
            is_memory_write = j.is_memory_write
            disassemble = j.disassemble
            instruction = Instruction(thread_id, ins_offset, accessed_offset, is_memory_read, is_memory_write, disassemble)
            instructions.append(instruction)

access_dict = []
ins_trace = []
with open(filename3, "r") as file:
    lines = file.readlines()
    for line in lines:
    	for access in instructions:
    		if int(access.instruction_offset) == int(line.strip()):
    			access_dict.append(access)
        # trace.append(int(line.rstrip()))
for access in access_dict:
	print(access.instruction_offset)

def calculate_reward(state, action):
	reward = 0
	first = -1
	second = -1
	list_length = len(state)
	for i in range(list_length):
		if state[i].instruction_offset == str(15971):
			first = i
		elif state[i].instruction_offset == str(16043):
			second = i
	if first != -1 and second != -1:
		if first < second and action.instruction_offset == str(15988):
			reward = 10
	return reward

access_dict_length = len(access_dict)
for i in range(access_dict_length-1):
	print(calculate_reward(access_dict[:i], access_dict[i+1]))

list1 = [1, 2, 3]
list2 = [1, 2, 3]
if list1 == list2:
	print("Same")
