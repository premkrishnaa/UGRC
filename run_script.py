import os

folders = ['d1','d2','d3']
files = range(1,11)
for folder in folders:
	for f in files:
		s1 = './graphmatching -A -s -t -i  ../datasets/' + folder + '/' + folder + '_/1000_1000_5_1_'+ str(f) + '.txt -o out1.txt'
		s2 = './graphmatching -B -s -t -i  ../datasets/' + folder + '/' + folder + '_/1000_1000_5_1_'+ str(f) + '.txt -o out2.txt'
		print(s1)
		os.system(s1)
		print(s2)
		os.system(s2)
		print("diff: " + str(f))
		os.system('diff out1.txt out2.txt')
os.system("rm out1.txt out2.txt")