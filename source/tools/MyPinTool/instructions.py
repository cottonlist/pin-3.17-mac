import json
import pickle

learning_rate = 1
discount_factor = 1

class Instruction(object):
	"""docstring for Instruction"""
	def __init__(self, ins_offset, threadId, accessed_offset, isRead, isWrite):
		super(Instruction, self).__init__()
		self.ins_offset = ins_offset
		self.threadId = threadId
		self.accessed_offset = accessed_offset
		self.isRead = isRead
		self.isWrite = isWrite

class State(object):
	"""docstring for State"""
	def __init__(self, trace, reward1=0, reward2=0, value1=0, value2=0, ):
		super(State, self).__init__()
		self.trace = trace
		self.reward1 = reward1
		self.reward2 = reward2
		self.value1 = value1
		self.value2 = value2

def calculate_reward(previous, next):
	for ins in instructions:
		if int(ins.ins_offset[:-1]) == previous: 
			old = ins
		elif int(ins.ins_offset[:-1]) == next:
			new = ins

	if old.accessed_offset == new.accessed_offset:
		if old.threadId != new.threadId:
			if old.isRead[0:1] == "1" and new.isWrite[0:1]=="1":
				return 1
			if old.isWrite[0:1] == "1" and new.isRead[0:1] == "1":
				return 1
			else:
				return 0

def calculate_value(trace, next):
	reward = calculate_reward(trace[-5:], next)
	print(reward)

instructions = []
file2 = open("profiler_rl.log", "r")
lines = file2.readlines()
for line in lines:
	temp1, temp2, temp3, temp4, temp5 = line.split()
	ins = Instruction(temp1, temp2, temp3, temp4, temp5)
	instructions.append(ins)

states = []
InFile2 = open("state", "r")
lines = InFile2.readlines()
for line in lines:
	temp1, temp2, temp3, temp4, temp5 = line.split()
	state = State(temp1, temp2, temp3, temp4, temp5)
	states.append(state)
InFile2.close()

with open("ins.pickle", "rb") as file4:
	states = pickle.load(file4)

with open("ins.pickle", "wb") as file3:
	pickle.dump(instructions, file3)


trace = ""
InFile1 = open("trace", "r")
InOutFile1 = open("state", "a+")
lines = InFile1.readlines()
for line in lines:
	trace = trace + line.rstrip()
	print(trace)

calculate_reward(15844, 15793)
calculate_value(str(12415115844), 15793)

		
		
		


		