import pickle

filename1 = "profiler_rl_part1.log"
filename2 = "profiler_rl_part2.log"
filename3 = "trace_v2"
filename4 = "states_idiom2_ex1"

# variable settings for learning process
learning_rate = 0.1
discount_factor = 1
round_digit = 2

class MemoryAccess:
    def __init__(self, thread_id, ins_offset, accessed_offset, is_read, is_write, disassemble):
        self.thread_id = thread_id
        self.ins_offset = ins_offset
        self.accessed_offset = accessed_offset
        self.is_read = is_read
        self.is_write = is_write
        self.disassemble = disassemble

class MemoryAccessPart1:
    def __init__(self, ins_offset, thread_id, accessed_offset):
        self.ins_offset = ins_offset
        self.thread_id = thread_id
        self.accessed_offset = accessed_offset

class MemoryAccessPart2:
    def __init__(self, ins_offset, is_memory_read, is_memory_write, is_stack_read, is_stack_write, disassemble):
        self.ins_offset = ins_offset
        self.is_memory_read = is_memory_read
        self.is_memory_write = is_memory_write
        self.is_stack_read = is_stack_read
        self.is_stack_write = is_stack_write
        self.disassemble = disassemble

accesses = []
accesses_part1 = []
accesses_part2 = []

with open(filename1, "r") as file1, open(filename2, "r") as file2:
    lines_file1 = file1.readlines()
    lines_file2 = file2.readlines()
    for line in lines_file1:
        temp1, temp2, temp3 = line.split()
        access_part1 = MemoryAccessPart1(temp1[:-1], temp2[:-1], temp3)
        accesses_part1.append(access_part1)
    for line in lines_file2:
        temp1, temp2, temp3, temp4, temp5, temp6 = line.split("; ")
        access_part2 = MemoryAccessPart2(temp1, temp2, temp3, temp4, temp5, temp6.strip())
        accesses_part2.append(access_part2)

for i in accesses_part1:
    ins_offset = i.ins_offset
    thread_id = i.thread_id
    accessed_offset = i.accessed_offset
    for j in accesses_part2:
        if j.ins_offset == ins_offset:
            is_memory_read = j.is_memory_read
            is_memory_write = j.is_memory_write
            disassemble = j.disassemble
            access = MemoryAccess(thread_id, ins_offset, accessed_offset, is_memory_read, is_memory_write, disassemble)
            accesses.append(access)

interleave = []
with open(filename3, "r") as file:
    lines = file.readlines()
    for line in lines:
        for access in accesses:
            if access.ins_offset == str(line.strip()):
                interleave.append(access)
for access in interleave:
    print(access.ins_offset)
print()

def calculate_reward(state, action):
    reward = 0
    first = -1
    second = -1
    list_length = len(state)
    for i in range(list_length):
        if state[i].ins_offset == str(15971):
            first = i
        elif state[i].ins_offset == str(16043):
            second = i
    if first != -1 and second != -1:
        if first < second and action.ins_offset == str(15988):
            reward = 100
    return reward


# class State:
#     def __init__(self, offset):
#         self.offset = offset
#         self.q_value = {}

class State:
    def __init__(self, interleave):
        self.interleave = interleave
        self.q_value = {}

states = []

try:
    with open(filename4, "rb") as pickle_file:
        states = pickle.load(pickle_file)
except IOError:
    print("File not exists")



def create_state(interleave):
    new_state = State(interleave)
    states.append(new_state)

def update_q_value(executed, next):
    for i in executed:
        print(i.ins_offset)
    print()
    print(next.ins_offset)
    print("section1")
    print(executed)
    print("section2")
    next_list = [next]
    temp = executed + next_list
    complete = []
    for i in temp:
        complete.append(i.ins_offset)
    is_contained_executed = 0
    is_contained_complete = 0
    states_length = len(states)
    reward = calculate_reward(executed, next)
    for i in range(states_length):
        state_list=[]
        executed_list=[]
        for j in states[i].interleave:
            state_list.append(j.ins_offset)
        for j in executed:
            executed_list.append(j.ins_offset)
        # print("list1:")
        # print(list1)
        # print("list2:")
        # print(list2)
        # print("section3")
        # print((states[i].interleave))
        # print("section4")
        # print(len(states[i].interleave))

        print((executed))
        print("section5")
        print(len(executed))
        if state_list == executed_list:
            is_contained_executed = 1
            index_executed = i
        if state_list == complete:
            is_contained_complete = 1
            index_complete = i
        # if states[i].interleave == executed:
        #     print("section6")
        #     is_contained_executed = 1
        #     index_executed = i
        # if states[i].interleave == complete:
        #     print("section7")
        #     is_contained_complete = 1
        #     index_complete = i

    if is_contained_executed == 0:
        print("section8")
        create_state(executed)
        if is_contained_complete == 0:
            states[-1].q_value[str(next.ins_offset)] = round(reward, round_digit)
        elif is_contained_complete == 1:
            states[-1].q_value[str(next.ins_offset)] = round(reward+discount_factor*max(states[index_complete].q_value.values()), round_digit)
    elif is_contained_executed == 1:
        print("section9")
        if is_contained_complete == 0:
            print("section10")
            states[index_executed].q_value[str(next.ins_offset)]=round(reward, round_digit)
        elif is_contained_complete == 1:
            print("section11")
            states[index_executed].q_value[str(next.ins_offset)]=round(states[index_executed].q_value[str(next.ins_offset)]+learning_rate*(reward+discount_factor*max(states[index_complete].q_value.values())-states[index_executed].q_value[str(next.ins_offset)]), round_digit)

    print()
    print()

    # if is_contained_current == 0 and is_contained_next == 0:
    #     create_state(current)
    #     states[-1].q_value[str(next)] = round(reward, round_digit)

    # elif is_contained_current == 0 and is_contained_next == 1:
    #     create_state(current)
    #     states[-1].q_value[str(next)] = round(reward + discount_factor * max(states[index_next].q_value.values()), round_digit)

    # elif is_contained_current == 1 and is_contained_next == 1:
    #     if str(next) in states[index_current].q_value:
    #         states[index_current].q_value[str(next)] = round(states[index_current].q_value[str(next)] + learning_rate * (reward + discount_factor * max(states[index_next].q_value.values()) - states[index_current].q_value[str(next)]), round_digit)
    #     else:
    #         states[index_current].q_value[str(next)] = round(learning_rate * (reward + discount_factor * max(states[index_next].q_value.values())), round_digit)

        
interleave_length = len(interleave)
for i in range(interleave_length-1):
    update_q_value(interleave[:i+1], interleave[i+1])


for state in states:
    for access in state.interleave:
        print(access.ins_offset)
        # print(access.thread_id)
        # print(access.is_read)
        # print(access.is_write)
        # print(access.disassemble)
    print(state.q_value)
    print()
    print()
    # for access in state:
    #     print(state.ins_offset)
    # print(state.q_value)
print(len(states))


with open(filename4, "wb") as pickle_file:
    pickle.dump(states, pickle_file)