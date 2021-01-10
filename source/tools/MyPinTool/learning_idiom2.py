import pickle

# variable settings for learning process
learning_rate = 0.9
discount_factor = 1
round_digit = 2

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
    def __init__(self, ins_offset, is_memory_read, is_memory_write, is_stack_read, is_stack_write):
        self.ins_offset = ins_offset
        self.is_memory_read = is_memory_read
        self.is_memory_write = is_memory_write
        self.is_stack_read = is_stack_read
        self.is_stack_write = is_stack_write

instructions = []
instructions_part1 = []
instructions_part2 = []

with open("profiler_rl_part1.log", "r") as file1, open("profiler_rl_part2.log", "r") as file2:
    lines_file1 = file1.readlines()
    lines_file2 = file2.readlines()
    for line in lines_file1:
        temp1, temp2, temp3 = line.split()
        instruction_part1 = Instruction_Part1(temp1[:-1], temp2[:-1], temp3)
        instructions_part1.append(instruction_part1)
    for line in lines_file2:
        temp1, temp2, temp3, temp4, temp5 = line.split()
        instruction_part2 = Instruction_Part2(temp1[:-1], temp2[:-1], temp3[:-1], temp4[:-1], temp5)
        instructions_part2.append(instruction_part2)

for i in instructions_part1:
    ins_offset = i.ins_offset
    thread_id = i.thread_id
    accessed_offset = i.accessed_offset
    for j in instructions_part2:
        if j.ins_offset == ins_offset:
            is_memory_read = j.is_memory_read
            is_memory_write = j.is_memory_write
            instruction = Instruction(thread_id, ins_offset, accessed_offset, is_memory_read, is_memory_write, "")
            instructions.append(instruction)
for i in instructions:
    if i.instruction_offset == "15971":
        i.disassemble = "cmp"
    else:
        i.disassemble = "non-cmp"


trace = []
with open("trace_v2", "r") as file:
    lines = file.readlines()
    for line in lines:
        trace.append(int(line.rstrip()))

def calculate_reward(instruction_offset_1, instruction_offset_2):
    """
    docstring
    """

    instructions_length = len(instructions)
    for i in range(instructions_length):
        if int(instructions[i].instruction_offset) == instruction_offset_1:
            index_1 = i
        if int(instructions[i].instruction_offset) == instruction_offset_2:
            index_2 = i

    reward = 0

    if instructions[index_1].accessed_offset == instructions[index_2].accessed_offset:
        if instructions[index_1].thread_id != instructions[index_2].thread_id:
            if instructions[index_1].is_write == "1" and instructions[index_2].is_read == "1" and instructions[index_2].disassemble == "non-cmp":
                reward = 1
            elif instructions[index_1].is_write == "1" and instructions[index_2].is_read == "1" and instructions[index_2].disassemble == "cmp":
                reward = -1
            elif instructions[index_1].is_read == "1" and instructions[index_2].is_write == "1" and instructions[index_1].disassemble == "cmp":
                reward = 1
            elif instructions[index_1].is_read == "1" and instructions[index_2].is_write == "1" and instructions[index_1].disassemble == "non-cmp":
                reward = -1
        elif instructions[index_1].thread_id == instructions[index_2].thread_id:
            if instructions[index_1].is_read == "1" and instructions[index_2].is_read == "1":
                reward = -1
    
    # print("Reward of ", instruction_offset_1, instruction_offset_2, reward)
    return reward


class State:
    def __init__(self, offset):
        self.offset = offset
        self.q_value = {}

states = []

try:
    with open("states_binary_idiom2", "rb") as pickle_file:
        states = pickle.load(pickle_file)
except IOError:
    print("File not exists")



def create_state(offset):
    new_state = State(offset)
    states.append(new_state)

def update_q_value(current, next):
    is_contained_current = 0
    is_contained_next = 0
    list_length = len(states)
    reward = calculate_reward(current, next)
    for i in range(list_length):
        if str(states[i].offset) == str(current):
            is_contained_current = 1
            index_current = i
        if str(states[i].offset) == str(next):
            is_contained_next = 1
            index_next = i

    if is_contained_current == 0 and is_contained_next == 0:
        create_state(current)
        states[-1].q_value[str(next)] = round(learning_rate * reward, round_digit)

    elif is_contained_current == 0 and is_contained_next == 1:
        create_state(current)
        states[-1].q_value[str(next)] = round(learning_rate * (reward + discount_factor * max(states[index_next].q_value.values())), round_digit)

    elif is_contained_current == 1 and is_contained_next == 1:
        if str(next) in states[index_current].q_value:
            states[index_current].q_value[str(next)] = round(states[index_current].q_value[str(next)] + learning_rate * (reward + discount_factor * max(states[index_next].q_value.values()) - states[index_current].q_value[str(next)]), round_digit)
        else:
            states[index_current].q_value[str(next)] = round(learning_rate * (reward + discount_factor * max(states[index_next].q_value.values())), round_digit)

        
trace_length = len(trace)
for i in range(trace_length-1):
    update_q_value(trace[i], trace[i+1])


for state in states:
    print(state.offset, state.q_value)


with open("states_binary_idiom2", "wb") as pickle_file:
    pickle.dump(states, pickle_file)