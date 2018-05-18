#!/usr/bin/python3
import re

LOG_PATH = "fetched/out.txt"
PREF_RPI = "MobRPi"

#-----------------------------------------------------------------------------/
#-----------------------------------------------------------------------------/
#-----------------------------------------------------------------------------/

def prefix_latex():
	return """\\documentclass[]{article}
\\usepackage{pgfplots}
\\usepackage{geometry}
\\pgfplotsset{compat=newest}%
\\begin{document}\n"""

def suffix_latex():
	return "\end{document}\n"

def prefix_graph(graphname):
	return """\\newgeometry{left=0.5in}
\\begin{tikzpicture}
	\\begin{axis}[xlabel=Time (ms),
				ylabel=Packet number,
				cycle list name=exotic,
				width=16cm, height=10cm,
				legend pos=outer north east,
				title=""" + graphname + """,
				mark size=1.5pt]\n"""

def suffix_graph():
	return """\\end{axis}
\\end{tikzpicture}
\\restoregeometry\n"""


def prefix_plot():
	return "\\addplot coordinates {\n"

def suffix_plot(legend):
	return "};\n\\addlegendentry{" + legend + "}\n"

def add_point(x, y):
	return "("+str(x)+","+str(y)+")\n"

#-----------------------------------------------------------------------------/
#-----------------------------------------------------------------------------/
#-----------------------------------------------------------------------------/

def get_rpis():
	rpis = set()
	with open(LOG_PATH) as logs:
		for line in logs:
			fullname = (re.findall(re.escape(PREF_RPI)+r"[0-9]*", line)[0])
			rpis.add(int(fullname[len(PREF_RPI):]))
	rpis = list(rpis)
	rpis.sort()
	return rpis

def print_info(rpis):
	send_count = [0 for x in range(rpis[-1])]
	recv_count = [[0]*rpis[-1]]*rpis[-1]
	len(send_count)
	with open(LOG_PATH) as logs:
		for line in logs:
			tmp = re.search(re.escape(PREF_RPI) + r"([0-9]*).*SEND_BRD", line)
			if(tmp):
				send_count[int(tmp.group(1))-1]+=1
				continue
			tmp = re.search(re.escape(PREF_RPI) + r"([0-9]*).*RECV.*192.168.2.([0-9]*)", line)
			if(tmp):
				recv_count[int(tmp.group(1))-1][int(tmp.group(2))-1]+=1
	print(send_count)
	print(recv_count)
	exit(1)

def get_first_ts():
	with open(LOG_PATH) as logs:
		for line in logs:
			return re.findall(r"[0-9]{10}", line)[0]

def get_first_send_ts(rpi):
	with open(LOG_PATH) as logs:
		for line in logs:
			if(re.search(re.escape(PREF_RPI+str(rpi)), line)):
				return re.search(r"[0-9]{13}", line).group(0)

def get_first_recv_ts(rpi):
	with open(LOG_PATH) as logs:
		for line in logs:
			if(re.search(re.escape(PREF_RPI+str(rpi))+r".*RECV", line)):
				return re.search(r"[0-9]{13}", line).group(0)

def draw_plot_peer_msg_recv(fd, sender, recver):
	first_ts = int(get_first_send_ts(sender))
	cpt = 0
	fd.write(prefix_plot())
	with open(LOG_PATH) as logs:
		for line in logs:
			if(re.search(r".*" + re.escape(PREF_RPI+str(recver)) + 
							r".*from 192.168.2." + 
							re.escape(str(sender)), line)):
				ts = int(re.search(r"[0-9]{13}", line).group(0))
				fd.write(add_point(ts - first_ts, cpt))
				cpt+=1
	fd.write(suffix_plot(str(sender) + "$\\rightarrow$" + str(recver)))

def draw_plot_peer_msg_recv_by_id(fd, sender, recver):
	first_ts = int(get_first_send_ts(sender))
	fd.write(prefix_plot())
	with open(LOG_PATH) as logs:
		for line in logs:
			if(re.search(r".*" + re.escape(PREF_RPI+str(recver)) + 
							r".*from 192.168.2." + 
							re.escape(str(sender)), line)):
				ts = int(re.search(r"[0-9]{13}", line).group(0))
				msg_id = int(re.search(r"RECV ([0-9]*)", line).group(1))
				fd.write(add_point(ts - first_ts, msg_id))
	fd.write(suffix_plot(str(sender) + "$\\rightarrow$" + str(recver)))

def draw_plot_local_msg_recv(fd, i, legend):
	first_ts = int(get_first_recv_ts(i))
	cpt = 0
	fd.write(prefix_plot())
	with open(LOG_PATH) as logs:
		for line in logs:
			if(re.search(r".*" + re.escape(PREF_RPI+str(i)) + 
						r".*RECV", line)):
				ts = int(re.search(r"[0-9]{13}", line).group(0))
				fd.write(add_point(ts - first_ts, cpt))
				cpt+=1
	fd.write(suffix_plot(legend))
			
#-----------------------------------------------------------------------------/
#-----------------------------------------------------------------------------/
#-----------------------------------------------------------------------------/

#First step, find all rpi's ids
rpis = get_rpis()

print_info(rpis)

print(rpis)
print(get_first_ts())

fd = open("foo.tex", "w")
fd.write(prefix_latex())

fd.write(prefix_graph("Global reception of broadcast messages"))
for i in rpis:
	draw_plot_local_msg_recv(fd, i, PREF_RPI + str(i))
fd.write(suffix_graph())

for i in rpis:
	fd.write(prefix_graph(PREF_RPI + str(i) + "'s broadcast received by others"))
	for j in rpis:
		if(i != j):
			draw_plot_peer_msg_recv(fd, i, j)
	fd.write(suffix_graph())
	fd.write(prefix_graph(PREF_RPI + str(i) + "'s broadcast received by others with id"))
	for j in rpis:
		if(i != j):
			draw_plot_peer_msg_recv_by_id(fd, i, j)
	fd.write(suffix_graph())


fd.write(suffix_latex())
fd.close()
