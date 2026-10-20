print("\n\n---------------------------------------\n")


from d3gl import D3GL, Colors
import sys

d3gl = D3GL(320, 480 - 32)
print("d3gl init success")

d3gl.draw_line(10, 10, 50, 50, Colors.RED)

d3gl.update()

while True:
	result = sys.evtpop()
	if isinstance(result, tuple):
		evt_type, timestamp, x, y = result
		print("evt_type:", evt_type) # 1=mouse_btn1 2=mouse_btn2 3=mouse_btn1+2 4=mouse_wheel
		print("timestamp:", timestamp)
		print("x:", x)
		print("y:", y)


print("d3gl update success")


print("\n---------------------------------------\n\n")