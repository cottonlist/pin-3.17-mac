import pickle

# variable settings for learning process
learning_rate = 1
discount_factor = 1

class Instruction:
    def __init__(self, thread_id, instruction_offset, accessed_offset, is_read, is_write):
        self.thread_id = thread_id
        self.instruction_offset = instruction_offset
        self.accessed_offset = accessed_offset
        self.is_read = is_read
        self.is_write = is_write

instructions = []
with open("profiler_rl.log", "r") as file:
    lines = file.readlines()
    for line in lines:
        temp1, temp2, temp3, temp4, temp5 = line.split()
        instruction = Instruction(temp2[:-1], temp1[:-1], temp3[:-1], temp4[:-1], temp5)
        instructions.append(instruction)


trace = []
with open("trace", "r") as file:
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
            if instructions[index_1].is_write == "1" and instructions[index_2].is_read == "1":
                reward = 1
    
    # print("Reward of ", instruction_offset_1, instruction_offset_2, reward)
    return reward


class State:
    def __init__(self, offset):
        self.offset = offset
        self.q_value = {}

states = []
with open("states_binary", "rb") as pickle_file:
    states = pickle.load(pickle_file)

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
        states[-1].q_value[str(next)] = learning_rate * reward

    elif is_contained_current == 0 and is_contained_next == 1:
        create_state(current)
        states[-1].q_value[str(next)] = learning_rate * (reward + discount_factor * max(states[index_next].q_value.values()))

    elif is_contained_current == 1 and is_contained_next == 1:
        if str(next) in states[index_current].q_value:
            states[index_current].q_value[str(next)] = states[index_current].q_value[str(next)] + learning_rate * (reward + discount_factor * max(states[index_next].q_value.values()) - states[index_current].q_value[str(next)])
        else:
            states[index_current].q_value[str(next)] = learning_rate * (reward + discount_factor * max(states[index_next].q_value.values()))

        
trace_length = len(trace)
for i in range(trace_length-1):
    update_q_value(trace[i], trace[i+1])


for state in states:
    print(state.offset, state.q_value)


with open("states_binary", "wb") as pickle_file:
    pickle.dump(states, pickle_file)